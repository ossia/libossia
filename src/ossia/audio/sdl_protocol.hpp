#pragma once
#include <ossia/detail/config.hpp>
#if __has_include(<SDL/SDL_audio.h>) && !defined(OSSIA_PROTOCOL_JOYSTICK)
#include <ossia/audio/audio_engine.hpp>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#define OSSIA_AUDIO_SDL 1

namespace ossia
{
class sdl_protocol final : public audio_engine
{
  static constexpr int inputs = 0;
  static constexpr int outputs = 2;

public:
  sdl_protocol(int rate, int bs)
  {
    SDL_Init(SDL_INIT_AUDIO);
    m_desired.freq = rate;
    m_desired.format = AUDIO_S16SYS;
    m_desired.channels = outputs;
    m_desired.samples = bs;
    m_desired.callback = SDLCallback;
    m_desired.userdata = this;

    if (SDL_OpenAudio(&m_desired, &m_obtained) < 0)
    {
      using namespace std::literals;
      throw std::runtime_error("SDL: Couldn't open audio: "s + SDL_GetError());
    }

    this->effective_sample_rate = m_obtained.freq;
    this->effective_buffer_size = m_obtained.samples;
    this->effective_inputs = 0;
    this->effective_outputs = m_obtained.channels;

    SDL_PauseAudio(0);
  }

  ~sdl_protocol() override
  {
    stop();
    SDL_CloseAudio();
    SDL_Quit();
  }

  bool running() const override
  {
    return SDL_GetAudioStatus() == SDL_AUDIO_PLAYING;
  }

private:
  static void SDLCallback(void* userData, Uint8* data, int nframes)
  {
    auto& self = *static_cast<sdl_protocol*>(userData);
    self.tick_start();

    auto samples = reinterpret_cast<int16_t*>(data);

    const auto n_samples = nframes / self.outputs;
    if (self.stop_processing)
    {
      self.tick_clear();
      for (int i = 0; i < n_samples; i++)
      {
        samples[i] = 0;
      }
      return;
    }

    {
      auto float_data = (float*)alloca(sizeof(float) * nframes);
      auto float_output = (float**)alloca(sizeof(float*) * self.outputs);

      for (int i = 0; i < self.outputs; i++)
      {
        float_output[i] = float_data + i * n_samples;
      }

      // if one day there's input... samples[j++] / 32768.;
      int k = 0;
      for (int j = 0; j < n_samples;)
      {
        float_output[0][k] = 0.; j++;
        float_output[1][k] = 0.; j++;
        k++;
      }

      // TODO time in seconds !
      ossia::audio_tick_state ts{nullptr, float_output, (int)self.inputs, (int)self.outputs, uint64_t(nframes / self.outputs), 0};
      self.audio_tick(ts);

      k = 0;
      for (int j = 0; j < n_samples;)
      {
        samples[j++] = float_output[0][k] * 32768.;
        samples[j++] = float_output[1][k] * 32768.;
        k++;
      }
      self.tick_end();
    }
  }

  SDL_AudioSpec m_desired, m_obtained;
};
}

#endif
