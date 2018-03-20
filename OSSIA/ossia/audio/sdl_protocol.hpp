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
  public:
    sdl_protocol()
    {
      SDL_Init(SDL_INIT_AUDIO);
    }

    ~sdl_protocol() override
    {
      stop();
      SDL_Quit();
    }

    void reload(ossia::audio_protocol*) override
    {/*
      stop();
      m_desired.freq = rate;
      m_desired.format = AUDIO_S16SYS;
      m_desired.channels = 2;
      m_desired.samples = 64;
      m_desired.callback = SDLCallback;
      m_desired.userdata = this;

      inputs = 0;
      outputs = 2;

      if (SDL_OpenAudio(&m_desired, &m_obtained) < 0){
        std::cerr << "Couldn't open audio: " << SDL_GetError() << std::endl;
        return;
      }

      SDL_PauseAudio(0);*/
    }

    void stop() override
    {
      SDL_CloseAudio();
    }

  private:
    static void SDLCallback(void* userData, Uint8* data, int nframes)
    {/*
      auto& self = *static_cast<sdl_protocol*>(userData);
      auto samples = reinterpret_cast<int16_t*>(data);

      auto float_data = (float*) alloca(sizeof(float) * self.outputs * nframes);
      auto float_output = (float**) alloca(sizeof(float*) * self.outputs);

      for(int i = 0; i < self.outputs; i++)
      {
        float_output[i] = float_data + i * nframes;

        for(int j = 0; j < nframes; j++)
        {
          float_output[i][j] = samples[nframes * i + j] / 32768.;
        }
      }

      process_generic(self, nullptr, float_output, nframes);
      self.audio_tick(nframes, 0);
      */
    }

    SDL_AudioSpec m_desired, m_obtained;
};

}

#endif
