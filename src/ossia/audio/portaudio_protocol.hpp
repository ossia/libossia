#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_PORTAUDIO)
#if __has_include(<portaudio.h>)
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/thread.hpp>

#include <portaudio.h>
#if defined(__linux__) && __has_include(<alsa/asoundlib.h>)
#include <ossia/audio/libasound.hpp>
#define OSSIA_PORTAUDIO_ALSA_PERIOD 1
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
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

    // paInDevelopment: any host API.
    const int hostApiIndex
        = hostApi == paInDevelopment ? -1 : Pa_HostApiTypeIdToHostApiIndex(hostApi);
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

    auto latency = [&](const PaDeviceInfo* dev, bool input, int channels) {
      if(!dev || Pa_GetHostApiInfo(dev->hostApi)->type != paALSA)
        return 0.01;
      // The ALSA host (pa_linux_alsa.c) asks for a period of (bs + latency) / 4
      // rounded up to a multiple of bs, at least the device's minimum, and a
      // buffer of latency + period. With latency = (n - 1) periods: n periods
      // of max(bs, device minimum) rounded up to a multiple of bs. Unlike the
      // ALSA engine it cannot retry with more periods, so periods shorter
      // than 2 ms get 4 (an HDA codec at 0.67 ms under-runs with 3 here).
      int period = std::max(bs, alsa_min_period(dev->name, input, rate, channels));
      period = (period + bs - 1) / bs * bs;
      const int periods = period * 1000 < 2 * rate ? 4 : 3;
      return double(periods - 1) * period / rate;
    };

    PaStreamParameters inputParameters;
    inputParameters.device = card_in_idx;
    inputParameters.channelCount = inputs;
    inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    inputParameters.suggestedLatency = latency(devInInfo, true, inputs);
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputParameters;
    outputParameters.device = card_out_idx;
    outputParameters.channelCount = outputs;
    outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    outputParameters.suggestedLatency = latency(devOutInfo, false, outputs);
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

    // Before the stream starts: the callback reads them.
    auto info = Pa_GetStreamInfo(stream);
    this->effective_sample_rate = info->sampleRate;
    this->effective_buffer_size = bs;
    this->effective_inputs = inputs;
    this->effective_outputs = outputs;
    this->effective_input_latency = int(std::lround(info->inputLatency * info->sampleRate));
    this->effective_output_latency
        = int(std::lround(info->outputLatency * info->sampleRate));
    m_stall_timeout = std::max(
        std::chrono::milliseconds(1000),
        std::chrono::milliseconds(int64_t(20) * 1000 * bs / std::max(rate, 1)));

    // PortAudio's own real-time mode (PaAlsa_EnableRealtimeScheduling) runs
    // the callback at SCHED_FIFO 1: the callback sets the priority instead.
    m_stream.store(stream);

    if(ec = Pa_StartStream(stream); ec != paNoError)
    {
      auto err = error_text("cannot start", ec, card_in, card_out);
      Pa_CloseStream(stream);
      m_stream.store(nullptr);
      Pa_Terminate();
      throw std::runtime_error(err);
    }

  }

  bool running() const override
  {
    auto s = m_stream.load();
    return s && Pa_IsStreamActive(s) == 1;
  }

  //! Called from the thread that stops the engine. Besides a stream that
  //! ended, an active stream whose callback no longer runs: the ALSA host
  //! keeps polling without calling back when every period under-runs, or
  //! when the device was unplugged.
  bool failed() const noexcept override
  {
    auto s = m_stream.load();
    if(!s || stop_processing)
      return false;
    if(Pa_IsStreamActive(s) == 0)
      return true;
    const auto now = std::chrono::steady_clock::now();
    const auto calls = m_callbacks.load(std::memory_order_relaxed);
    if(calls != m_seenCallbacks || m_seenAt == std::chrono::steady_clock::time_point{})
    {
      m_seenCallbacks = calls;
      m_seenAt = now;
      return false;
    }
    return now - m_seenAt > m_stall_timeout;
  }

  void stop() override
  {
    audio_engine::stop();

    // A stream that is stopped but not closed keeps the device open: opening
    // it again, e.g. when the settings change, fails until it is closed.
    // Aborting: stopping waits for the host's poll loop, seconds on a stalled
    // device, and the ALSA host drops the buffer either way.
    if(auto stream = m_stream.exchange(nullptr))
    {
      if(auto ec = Pa_AbortStream(stream); ec != paNoError)
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
  //! The smallest period the ALSA device accepts at this rate, 0 when it is
  //! not a hardware device or cannot be asked (e.g. busy).
  static int alsa_min_period(std::string_view name, bool input, int rate, int channels)
  {
#if OSSIA_PORTAUDIO_ALSA_PERIOD
    // "Card: device (hw:N,M)"
    auto pos = name.rfind(" (hw:");
    if(pos == std::string_view::npos || !name.ends_with(')'))
      return 0;
    const std::string hw{name.substr(pos + 2, name.size() - pos - 3)};
    try
    {
      auto& snd = ossia::libasound::instance();
      snd_pcm_t* pcm{};
      if(snd.pcm_open(
             &pcm, hw.c_str(), input ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK,
             SND_PCM_NONBLOCK)
         < 0)
        return 0;
      snd_pcm_hw_params_t* params{};
      snd_alloca(&params, snd, pcm_hw_params);
      snd_pcm_uframes_t min{};
      int dir{};
      unsigned r = rate, c = std::max(channels, 1);
      if(snd.pcm_hw_params_any(pcm, params) >= 0
         && snd.pcm_hw_params_set_rate_near(pcm, params, &r, nullptr) >= 0
         && snd.pcm_hw_params_set_channels_near(pcm, params, &c) >= 0)
        snd.pcm_hw_params_get_period_size_min(params, &min, &dir);
      snd.pcm_close(pcm);
      return int(min);
    }
    catch(...)
    {
      return 0;
    }
#else
    return 0;
#endif
  }

  //! ALSA device names end with the card index, e.g. "MM-1: USB Audio (hw:3,0)",
  //! which changes when USB devices enumerate in another order: when the exact
  //! name is gone, a device whose name only differs by that index matches.
  static int find_device(const std::string& name, int hostApiIndex, bool input)
  {
    if(name.empty())
      return paNoDevice;

    auto without_index = [](std::string_view n) {
      for(auto suffix : {" (hw:", " (plughw:"})
        if(auto pos = n.rfind(suffix); pos != std::string_view::npos && n.ends_with(')'))
          return n.substr(0, pos);
      return n;
    };

    int fallback = paNoDevice;
    for(int i = 0; i < Pa_GetDeviceCount(); i++)
    {
      auto info = Pa_GetDeviceInfo(i);
      if(hostApiIndex >= 0 && info->hostApi != hostApiIndex)
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
      ossia::set_current_thread_realtime(95, true, false);
#endif
      return 0;
    }();

    auto& self = *static_cast<portaudio_engine*>(userData);
    self.m_callbacks.fetch_add(1, std::memory_order_relaxed);
    // The flags of one host period reach every callback that period is split
    // into; they share its time stamp.
    if(statusFlags
       & (paInputUnderflow | paInputOverflow | paOutputUnderflow | paOutputOverflow))
    {
      if(timeInfo->currentTime == 0. || timeInfo->currentTime != self.m_lastXrunTime)
        self.xruns.fetch_add(1, std::memory_order_relaxed);
      self.m_lastXrunTime = timeInfo->currentTime;
    }

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
  std::atomic<uint64_t> m_callbacks{};
  double m_lastXrunTime{-1.};
  std::chrono::milliseconds m_stall_timeout{1000};
  mutable uint64_t m_seenCallbacks{};
  mutable std::chrono::steady_clock::time_point m_seenAt{};
};
}

#endif
#endif
