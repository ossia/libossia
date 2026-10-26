#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_PORTAUDIO)
#if __has_include(<portaudio.h>)
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/thread.hpp>

#include <portaudio.h>
#if defined(__linux__) && __has_include(<pa_linux_alsa.h>)
#include <pa_linux_alsa.h>
#endif

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

#define OSSIA_AUDIO_PORTAUDIO 1

namespace ossia
{
class portaudio_engine final : public audio_engine
{
public:
  portaudio_engine(
      std::string name, std::string card_in, std::string card_out, int inputs,
      int outputs, int rate, int bs, PaHostApiTypeId hostApi)
  {
    if(auto ec = Pa_Initialize(); ec != paNoError)
      throw std::runtime_error(
          std::string("PortAudio: cannot initialize: ") + Pa_GetErrorText(ec));

    const auto hostApiIndex = Pa_HostApiTypeIdToHostApiIndex(hostApi);
    const int card_in_idx = find_device(card_in, hostApiIndex, true);
    const int card_out_idx = find_device(card_out, hostApiIndex, false);

    auto devInInfo = Pa_GetDeviceInfo(card_in_idx);
    inputs = devInInfo ? std::min(inputs, devInInfo->maxInputChannels) : 0;
    auto devOutInfo = Pa_GetDeviceInfo(card_out_idx);
    outputs = devOutInfo ? std::min(outputs, devOutInfo->maxOutputChannels) : 0;

    if(inputs <= 0 && outputs <= 0)
    {
      Pa_Terminate();
      throw std::runtime_error(
          "PortAudio: device not found: "
          + (card_out.empty() ? card_in : card_out));
    }

    auto latency = [&](const PaDeviceInfo* dev) {
      // The ALSA host derives its period from the latency: two buffers give a
      // period of bs frames and three periods. Full-speed USB devices (1 ms
      // packets) under-run continuously with less than 3 ms buffered.
      if(dev && Pa_GetHostApiInfo(dev->hostApi)->type == paALSA)
        return std::max(2. * bs / rate, 0.003);
      return 0.01;
    };

    PaStreamParameters inputParameters;
    inputParameters.device = card_in_idx;
    inputParameters.channelCount = inputs;
    inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    inputParameters.suggestedLatency = latency(devInInfo);
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputParameters;
    outputParameters.device = card_out_idx;
    outputParameters.channelCount = outputs;
    outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    outputParameters.suggestedLatency = latency(devOutInfo);
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters* actualInput = inputs > 0 ? &inputParameters : nullptr;
    PaStreamParameters* actualOutput = outputs > 0 ? &outputParameters : nullptr;

    PaStream* stream{};
    auto ec = Pa_OpenStream(
        &stream, actualInput, actualOutput, rate, bs, paNoFlag, &PortAudioCallback,
        this);
    if(ec != paNoError)
    {
      auto err = error_text("cannot open", ec, card_in, card_out);
      Pa_Terminate();
      throw std::runtime_error(err);
    }

    m_stream.store(stream);
#if defined(__linux__) && __has_include(<pa_linux_alsa.h>)
    if(hostApi == paALSA)
      PaAlsa_EnableRealtimeScheduling(stream, 1);
#endif

    if(ec = Pa_StartStream(stream); ec != paNoError)
    {
      auto err = error_text("cannot start", ec, card_in, card_out);
      Pa_CloseStream(stream);
      m_stream.store(nullptr);
      Pa_Terminate();
      throw std::runtime_error(err);
    }

    auto info = Pa_GetStreamInfo(stream);
    this->effective_sample_rate = info->sampleRate;
    this->effective_buffer_size = bs;
    this->effective_inputs = inputs;
    this->effective_outputs = outputs;
    this->effective_input_latency = int(info->inputLatency * info->sampleRate);
    this->effective_output_latency = int(info->outputLatency * info->sampleRate);
  }

  bool running() const override
  {
    auto s = m_stream.load();
    return s && Pa_IsStreamActive(s) == 1;
  }

  void stop() override
  {
    audio_engine::stop();

    // A stream that is stopped but not closed keeps the device open: opening
    // it again, e.g. when the settings change, fails until it is closed.
    if(auto stream = m_stream.exchange(nullptr))
    {
      if(auto ec = Pa_StopStream(stream); ec != paNoError)
        ossia::logger().warn("PortAudio: stopping the stream: {}", Pa_GetErrorText(ec));
      if(auto ec = Pa_CloseStream(stream); ec != paNoError)
        ossia::logger().warn("PortAudio: closing the stream: {}", Pa_GetErrorText(ec));
    }
  }

  ~portaudio_engine() override
  {
    stop();
    Pa_Terminate();
  }

private:
  //! ALSA device names end with the card index, e.g. "MM-1: USB Audio (hw:3,0)",
  //! which changes when USB devices enumerate in another order: when the exact
  //! name is gone, a device whose name only differs by that index matches.
  static int find_device(const std::string& name, int hostApiIndex, bool input)
  {
    if(name.empty())
      return paNoDevice;

    auto without_index = [](std::string_view n) {
      if(auto pos = n.rfind(" (hw:"); pos != std::string_view::npos && n.ends_with(')'))
        return n.substr(0, pos);
      return n;
    };

    int fallback = paNoDevice;
    for(int i = 0; i < Pa_GetDeviceCount(); i++)
    {
      auto info = Pa_GetDeviceInfo(i);
      if(info->hostApi != hostApiIndex && hostApiIndex != paInDevelopment)
        continue;
      if((input ? info->maxInputChannels : info->maxOutputChannels) <= 0)
        continue;
      if(info->name == name)
        return i;
      if(fallback == paNoDevice && without_index(info->name) == without_index(name))
        fallback = i;
    }
    return fallback;
  }

  static std::string error_text(
      const char* what, PaError ec, const std::string& card_in,
      const std::string& card_out)
  {
    std::string err = std::string("PortAudio: ") + what + " ";
    err += card_out.empty() ? card_in : card_out;
    if(!card_in.empty() && !card_out.empty() && card_in != card_out)
      err += " / " + card_in;
    err += ": ";
    err += Pa_GetErrorText(ec);
    if(ec == paUnanticipatedHostError)
      if(auto host = Pa_GetLastHostErrorInfo(); host && host->errorText)
        err += std::string(" (") + host->errorText + ")";
    return err;
  }

  static int clearBuffers(float** float_output, unsigned long nframes, int outs)
  {
    for(int i = 0; i < outs; i++)
    {
      auto chan = float_output[i];
      for(std::size_t j = 0; j < nframes; j++)
        chan[j] = 0.f;
    }

    return paContinue;
  }

  static int PortAudioCallback(
      const void* input, void* output, unsigned long nframes,
      const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
      void* userData)
  {
    [[maybe_unused]]
    static const thread_local auto _
        = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
#if defined(__linux__)
      ossia::set_current_thread_realtime();
#endif
      return 0;
    }();

    auto& self = *static_cast<portaudio_engine*>(userData);
    if(statusFlags
       & (paInputUnderflow | paInputOverflow | paOutputUnderflow | paOutputOverflow))
      self.xruns.fetch_add(1, std::memory_order_relaxed);

    self.tick_start();
    auto clt = self.m_stream.load();

    if(self.stop_processing || !clt)
    {
      self.tick_clear();
      return clearBuffers(((float**)output), nframes, self.effective_outputs);
    }

    auto float_input = ((float* const*)input);
    auto float_output = ((float**)output);

    ossia::audio_tick_state ts{
        float_input, float_output,         self.effective_inputs, self.effective_outputs,
        nframes,     timeInfo->currentTime};
    self.audio_tick(ts);

    self.tick_end();
    return paContinue;
  }

  std::atomic<PaStream*> m_stream{};
};
}

#endif
#endif
