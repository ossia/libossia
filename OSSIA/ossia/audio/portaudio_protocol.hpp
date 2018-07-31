#pragma once
#if __has_include(<portaudio.h>)
#include <ossia/audio/audio_protocol.hpp>
#include <portaudio.h>

#if __has_include(<pa_jack.h>) && !defined(_MSC_VER)
#include <pa_jack.h>
#endif
namespace ossia
{
class portaudio_engine final
    : public audio_engine
{
  public:
    int m_ins{}, m_outs{};
    portaudio_engine(std::string name, std::string card_in, std::string card_out,
                     int& inputs, int& outputs, int& rate, int& bs)
    {
      if(Pa_Initialize() != paNoError)
        throw std::runtime_error("Audio error");

#if __has_include(<pa_jack.h>) && !defined(_MSC_VER)
      PaJack_SetClientName(name.c_str());
#endif
      int card_in_idx = paNoDevice;
      int card_out_idx = paNoDevice;

      for(int i = 0; i < Pa_GetDeviceCount(); i++)
      {
        auto raw_name = Pa_GetDeviceInfo(i)->name;
        if(raw_name == card_in)
        {
          card_in_idx = i;
        }
        if(raw_name == card_out)
        {
          card_out_idx = i;
        }
      }
      if(card_in_idx == paNoDevice)
        card_in_idx = Pa_GetDefaultInputDevice();
      if(card_out_idx == paNoDevice)
        card_out_idx = Pa_GetDefaultOutputDevice();

      if(card_out_idx != card_in_idx)
      {
        auto in = Pa_GetDeviceInfo(card_in_idx);
        auto out = Pa_GetDeviceInfo(card_out_idx);

        if(in->hostApi != paMME || out->hostApi != paMME)
        {
          card_in_idx = card_out_idx;
        }
      }


      auto devInInfo = Pa_GetDeviceInfo(card_in_idx);
      if(!devInInfo)
      {
        std::cerr << "Audio error: no input device" << std::endl;
        inputs = 0;
      }
      else
      {
        inputs = std::min(inputs, devInInfo->maxInputChannels);
      }

      auto devOutInfo = Pa_GetDeviceInfo(card_out_idx);
      if(!devOutInfo)
      {
        std::cerr << "Audio error: no output device" << std::endl;
        outputs = 0;
      }
      else
      {
        outputs = std::min(outputs, devOutInfo->maxOutputChannels);
      }



      m_ins = inputs;
      m_outs = outputs;

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

      std::cerr << "=== stream start ===\n";
      PaStream* stream;
      auto ec = Pa_OpenStream(&stream,
                              card_in_idx != paNoDevice ? &inputParameters : nullptr,
                              card_out_idx != paNoDevice ? &outputParameters : nullptr,
                              rate,
                              bs, //paFramesPerBufferUnspecified,
                              paNoFlag,
                              &PortAudioCallback,
                              this);
      m_stream.store(stream);
      if(ec == PaErrorCode::paNoError)
      {
        ec = Pa_StartStream( stream );
        if(ec != PaErrorCode::paNoError)
        {
          std::cerr << "Error while starting audio stream: " << Pa_GetErrorText(ec) << std::endl;
          Pa_CloseStream(stream);
          m_stream.store(nullptr);
        }
      }
      else
      {
        std::cerr << "Error while opening audio stream: " << Pa_GetErrorText(ec) << std::endl;
        m_stream.store(nullptr);
      }

      if(!m_stream)
      {
        Pa_Terminate();
        throw std::runtime_error("Could not start PortAudio stream");
      }
    }

    ~portaudio_engine() override
    {
      stop();
      if(auto p = protocol.load())
        p->engine = nullptr;

      if(auto stream = m_stream.load())
      {
        auto ec = Pa_StopStream(stream);
        std::cerr << "=== stream stop ===\n";

        if(ec != PaErrorCode::paNoError)
        {
          std::cerr << "Error while stopping audio stream: " << Pa_GetErrorText(ec) << std::endl;
        }
      }
      Pa_Terminate();
    }

    void reload(ossia::audio_protocol* p) override
    {
      if(this->protocol)
        this->protocol.load()->engine = nullptr;
      stop();

      this->protocol = p;
      if(!p)
        return;
      auto& proto = *p;
      proto.engine = this;

      proto.setup_tree(m_ins, m_outs);

      stop_processing = false;
    }

    void stop() override
    {
      stop_processing = true;
      protocol = nullptr;

      while(processing) std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  private:
    static int PortAudioCallback(
        const void* input,
        void* output,
        unsigned long nframes,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData)
    {
      auto& self = *static_cast<portaudio_engine*>(userData);

      if(self.stop_processing)
      {
        auto float_output = ((float **) output);
        for(int i = 0; i < self.m_outs; i++)
        {
          auto chan = float_output[i];
          for(std::size_t j = 0; j < nframes; j++)
            chan[j] = 0.;
        }
        return 0;
      }

      auto clt = self.m_stream.load();
      auto proto = self.protocol.load();
      if(clt && proto)
      {
        self.processing = true;
        auto float_input = ((float *const *) input);
        auto float_output = ((float **) output);

        proto->process_generic(*proto, float_input, float_output, (int)self.m_ins, (int)self.m_outs, nframes);
        proto->audio_tick(nframes, timeInfo->currentTime);

        // Run a tick
        if(proto->replace_tick)
        {
          proto->audio_tick = std::move(proto->ui_tick);
          proto->ui_tick = {};
          proto->replace_tick = false;
        }

        self.processing = false;
      }

      return paContinue;
    }

    std::atomic<PaStream*> m_stream{};
};

}

#endif
