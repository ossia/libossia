#pragma once
#if __has_include(<portaudio.h>)
#include <ossia/audio/audio_engine.hpp>

#include <portaudio.h>

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
    if (Pa_Initialize() != paNoError)
      throw std::runtime_error("Audio error");

    int card_in_idx = paNoDevice;
    int card_out_idx = paNoDevice;

    auto hostApiIndex = Pa_HostApiTypeIdToHostApiIndex(hostApi);

    for (int i = 0; i < Pa_GetDeviceCount(); i++)
    {
      auto info = Pa_GetDeviceInfo(i);
      if(info->hostApi != hostApiIndex && hostApiIndex != paInDevelopment)
        continue;

      auto raw_name = info->name;
      //std::cerr << " - device " << i << " has name: " << raw_name << "\n";
      if (raw_name == card_in && info->maxInputChannels > 0)
      {
        //std::cerr << " its the input" << inputs << " " << info->maxInputChannels << "\n";
        card_in_idx = i;
      }
      if (raw_name == card_out && info->maxOutputChannels > 0)
      {
        //std::cerr << " its the output" << outputs << " " << info->maxOutputChannels << "\n";
        card_out_idx = i;
      }
      if(card_in_idx != paNoDevice && card_out_idx != paNoDevice)
        break;
    }

    auto devInInfo = Pa_GetDeviceInfo(card_in_idx);
    if (!devInInfo)
    {
      std::cerr << "Audio error: no input device" << std::endl;
      inputs = 0;
    }
    else
    {
      inputs = std::min(inputs, devInInfo->maxInputChannels);
    }

    auto devOutInfo = Pa_GetDeviceInfo(card_out_idx);
    if (!devOutInfo)
    {
      std::cerr << "Audio error: no output device" << std::endl;
      outputs = 0;
    }
    else
    {
      outputs = std::min(outputs, devOutInfo->maxOutputChannels);
    }

    PaStreamParameters inputParameters;
    inputParameters.device = card_in_idx;
    inputParameters.channelCount = inputs;
    inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    inputParameters.suggestedLatency = 0.01;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputParameters;
    outputParameters.device = card_out_idx;
    outputParameters.channelCount = outputs;
    outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    outputParameters.suggestedLatency = 0.01;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters* actualInput{};
    if (card_in_idx != paNoDevice && inputs > 0)
      actualInput = &inputParameters;
    PaStreamParameters* actualOutput{};
    if (card_out_idx != paNoDevice && outputs > 0)
      actualOutput = &outputParameters;
/*
    std::cerr << "input: \n"
         << bool(actualInput) << " "
        << card_in_idx<< " "
        << inputs << "\n";
    std::cerr << "output: \n"
              << bool(actualOutput) << " "
        << card_out_idx<< " "
        << outputs << "\n";
        */
    PaStream* stream;
    auto ec = Pa_OpenStream(
        &stream, actualInput, actualOutput, rate,
        bs, // paFramesPerBufferUnspecified,
        paNoFlag, &PortAudioCallback, this);
    m_stream.store(stream);
    if (ec == PaErrorCode::paNoError)
    {
      ec = Pa_StartStream(stream);

      if (ec != PaErrorCode::paNoError)
      {
        std::cerr << "Error while starting audio stream: "
                  << Pa_GetErrorText(ec) << std::endl;
        Pa_CloseStream(stream);
        m_stream.store(nullptr);
      }
      else
      {
        auto info = Pa_GetStreamInfo(stream);
        this->effective_sample_rate = info->sampleRate;
        this->effective_buffer_size = bs;
        this->effective_inputs = actualInput ? actualInput->channelCount : 0;
        this->effective_outputs = actualOutput ? actualOutput->channelCount : 0;
      }
    }
    else
    {
      std::cerr << "Error while opening audio stream: " << Pa_GetErrorText(ec)
                << std::endl;
      m_stream.store(nullptr);
    }

    if (!m_stream)
    {
      Pa_Terminate();
      throw std::runtime_error("Could not start PortAudio stream");
    }
  }

  bool running() const override
  {
    auto s = m_stream.load();
    return s && Pa_IsStreamActive(m_stream);
  }

  ~portaudio_engine() override
  {
    stop();

    if (auto stream = m_stream.load())
    {
      auto ec = Pa_StopStream(stream);
      std::cerr << "=== stream stop ===\n";

      if (ec != PaErrorCode::paNoError)
      {
        std::cerr << "Error while stopping audio stream: "
                  << Pa_GetErrorText(ec) << std::endl;
      }
    }
    Pa_Terminate();
  }


private:
  static int clearBuffers(float** float_output, unsigned long nframes, int outs)
  {
    for (int i = 0; i < outs; i++)
    {
      auto chan = float_output[i];
      for (std::size_t j = 0; j < nframes; j++)
        chan[j] = 0.f;
    }

    return paContinue;
  }

  static int PortAudioCallback(
      const void* input, void* output, unsigned long nframes,
      const PaStreamCallbackTimeInfo* timeInfo,
      PaStreamCallbackFlags statusFlags, void* userData)
  {
    // auto t0 = std::chrono::steady_clock::now();
    auto& self = *static_cast<portaudio_engine*>(userData);
    self.tick_start();
    auto clt = self.m_stream.load();

    if (self.stop_processing || !clt)
    {
      self.tick_clear();
      return clearBuffers(((float**)output), nframes, self.effective_outputs);
    }

    auto float_input = ((float* const*)input);
    auto float_output = ((float**)output);

    ossia::audio_tick_state ts{float_input, float_output, self.effective_inputs, self.effective_outputs, nframes, timeInfo->currentTime};
    self.audio_tick(ts);

    self.tick_end();

    // auto t1 = std::chrono::steady_clock::now();
    //
    // std::cerr << nframes << " => " << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() << std::endl;
    return paContinue;
  }

  std::atomic<PaStream*> m_stream{};
};
}

#endif
