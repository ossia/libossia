#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_SDL)
#if __has_include(<SDL2/SDL_audio.h>)
#include <SDL2/SDL_config.h>
#if !defined(SDL_AUDIO_DISABLED)
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/thread.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

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
    m_desired.format = AUDIO_F32SYS;
    m_desired.channels = outputs;
    m_desired.samples = bs;
    m_desired.callback = SDLCallback;
    m_desired.userdata = this;

    m_deviceId = SDL_OpenAudioDevice(nullptr, 0, &m_desired, &m_obtained, 0);

    if(m_deviceId < 2)
    {
      using namespace std::literals;
      throw std::runtime_error("SDL: Couldn't open audio: "s + SDL_GetError());
    }

    this->effective_sample_rate = m_obtained.freq;
    this->effective_buffer_size = m_obtained.samples;
    this->effective_inputs = 0;
    this->effective_outputs = m_obtained.channels;

    SDL_PauseAudioDevice(m_deviceId, 0);
    m_activated = true;
  }

  ~sdl_protocol() override { stop(); }

  bool running() const override
  {
    return m_activated && SDL_GetAudioDeviceStatus(m_deviceId) == SDL_AUDIO_PLAYING;
  }

  void stop() override
  {
    audio_engine::stop();
    if(m_activated)
    {
      SDL_CloseAudioDevice(m_deviceId);
      m_activated = false;
    }
    SDL_Quit();
  }

private:
  static void SDLCallback(void* userData, Uint8* data, int bytes)
  {
    [[maybe_unused]]
    static const thread_local auto _
        = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
      return 0;
    }();

    auto& self = *static_cast<sdl_protocol*>(userData);
    self.tick_start();
    if(!self.m_start)
      self.m_start = std::chrono::steady_clock::now();

    auto audio_out = reinterpret_cast<float*>(data);
    const int out_chan = self.m_obtained.channels;
    const int frames = self.m_obtained.samples;
    assert(out_chan > 0);
    assert(frames > 0);
    assert(frames * out_chan * sizeof(float) == bytes);

    if(self.stop_processing)
    {
      self.tick_clear();
      memset(data, 0, bytes);
      return;
    }

    {
      auto float_data = (float*)alloca(sizeof(float) * frames * out_chan);
      memset(float_data, 0, sizeof(sizeof(float) * frames * out_chan));

      auto float_output = (float**)alloca(sizeof(float*) * out_chan);

      for(int c = 0; c < out_chan; c++)
      {
        float_output[c] = float_data + c * frames;
      }

      // if one day there's input... samples[j++] / 32768.;

      auto now = std::chrono::steady_clock::now();
      auto nsecs
          = std::chrono::duration_cast<std::chrono::nanoseconds>(now - *self.m_start)
                .count()
            / 1e9;

      ossia::audio_tick_state ts{nullptr,  float_output,     0,
                                 out_chan, (uint64_t)frames, nsecs};
      self.audio_tick(ts);

      for(int j = 0; j < frames; j++)
        for(int c = 0; c < out_chan; c++)
          *audio_out++ = float_output[c][j];

      self.tick_end();
      self.m_total_frames += frames;
    }
  }

  SDL_AudioDeviceID m_deviceId{};
  SDL_AudioSpec m_desired, m_obtained;
  uint64_t m_total_frames{};
  std::optional<std::chrono::steady_clock::time_point> m_start;
  bool m_activated{};
};
}

#endif
#endif
#endif
