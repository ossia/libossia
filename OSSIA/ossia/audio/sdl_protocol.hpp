#pragma once
#if __has_include(<SDL/SDL_audio.h>)
#include <ossia/audio/audio_protocol.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

namespace ossia
{
class sdl_protocol final
    : public audio_engine
{
  static constexpr int inputs = 0;
  static constexpr int outputs = 2;
  public:
    sdl_protocol(int& rate, int& bs)
    {
      std::cerr << "SDL START\n";
      SDL_Init(SDL_INIT_AUDIO);
      m_desired.freq = rate;
      m_desired.format = AUDIO_S16SYS;
      m_desired.channels = outputs;
      m_desired.samples = bs;
      m_desired.callback = SDLCallback;
      m_desired.userdata = this;

      if (SDL_OpenAudio(&m_desired, &m_obtained) < 0)
      {
        std::cerr << "Couldn't open audio: " << SDL_GetError() << std::endl;
        return;
      }

      std::cerr << "SDL OPEN\n";
      rate = m_obtained.freq;
      bs = m_obtained.samples;

      SDL_PauseAudio(0);
      std::cerr << "SDL GO\n";
    }

    ~sdl_protocol() override
    {
      stop();
      SDL_CloseAudio();
      SDL_Quit();
    }

    void stop() override
    {
      std::cerr << "SDL STOP\n";
      stop_processing = true;
      protocol = nullptr;

      while(processing) std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

      proto.setup_tree(inputs, outputs);

      stop_processing = false;
    }

  private:
    static void SDLCallback(void* userData, Uint8* data, int nframes)
    {
      std::cerr << "SDL TICK" << nframes << "\n";
      auto& self = *static_cast<sdl_protocol*>(userData);
      auto samples = reinterpret_cast<int16_t*>(data);

      const auto n_samples = nframes / self.outputs;
      if(self.stop_processing)
      {
        for(int i = 0; i < n_samples; i++)
        {
          samples[i] = 0;
        }
        return;
      }

      auto proto = self.protocol.load();
      if(proto)
      {
        auto float_data = (float*) alloca(sizeof(float) * nframes);
        auto float_output = (float**) alloca(sizeof(float*) * self.outputs);

        for(int i = 0; i < self.outputs; i++)
        {
          float_output[i] = float_data + i * n_samples;
        }

        int l_i = 0;
        int r_i = 0;
        for(int j = 0; j < n_samples; )
        {
          float_output[0][l_i++] = samples[j++] / 32768.;
          float_output[1][r_i++] = samples[j++] / 32768.;
        }

        self.processing = true;

        proto->process_generic(*proto, nullptr, float_output, (int)self.inputs, (int)self.outputs, nframes / self.outputs);
        proto->audio_tick(nframes / self.outputs, 0);

        // Run a tick
        if(proto->replace_tick)
        {
          proto->audio_tick = std::move(proto->ui_tick);
          proto->ui_tick = {};
          proto->replace_tick = false;
        }

        self.processing = false;
      }
    }

    SDL_AudioSpec m_desired, m_obtained;
};

}

#endif
