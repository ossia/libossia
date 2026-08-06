#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_SDL)
#if __has_include(<SDL3/SDL_audio.h>)
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/thread.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

#include <algorithm>
#include <string>
#include <vector>

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

    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, std::to_string(bs).c_str());

    m_spec.freq = rate;
    m_spec.format = SDL_AUDIO_F32;
    m_spec.channels = outputs;

    m_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_spec, SDLCallback, this);

    if(!m_stream)
    {
      using namespace std::literals;
      throw std::runtime_error("SDL: Couldn't open audio: "s + SDL_GetError());
    }

    this->effective_sample_rate = m_spec.freq;
    this->effective_buffer_size = bs;
    this->effective_inputs = 0;
    this->effective_outputs = m_spec.channels;

    m_interleaved.resize(std::size_t(bs) * m_spec.channels);
    m_planar.resize(std::size_t(bs) * m_spec.channels);
    m_channels.resize(m_spec.channels);

    SDL_ResumeAudioStreamDevice(m_stream);
    m_activated = true;
  }

  ~sdl_protocol() override { stop(); }

  bool running() const override
  {
    return m_activated && !SDL_AudioStreamDevicePaused(m_stream);
  }

  void stop() override
  {
    audio_engine::stop();
    if(m_activated)
    {
      SDL_DestroyAudioStream(m_stream);
      m_stream = nullptr;
      m_activated = false;
    }
    SDL_Quit();
  }

private:
  static void SDLCallback(
      void* userData, SDL_AudioStream* stream, int additional_amount, int total_amount)
  {
    [[maybe_unused]]
    static const thread_local auto _
        = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
      return 0;
    }();

    if(additional_amount <= 0)
      return;

    auto& self = *static_cast<sdl_protocol*>(userData);
    const int out_chan = self.m_spec.channels;
    const int frames = self.effective_buffer_size;
    assert(out_chan > 0);
    assert(frames > 0);

    const int block_bytes = int(frames * out_chan * sizeof(float));

    while(additional_amount > 0)
    {
      self.tick_start();
      if(!self.m_start)
        self.m_start = std::chrono::steady_clock::now();

      if(self.stop_processing)
      {
        self.tick_clear();
        std::fill(self.m_interleaved.begin(), self.m_interleaved.end(), 0.f);
        SDL_PutAudioStreamData(stream, self.m_interleaved.data(), block_bytes);
        return;
      }

      float* const float_data = self.m_planar.data();
      float** const float_output = self.m_channels.data();
      std::fill_n(float_data, std::size_t(frames) * out_chan, 0.f);

      for(int c = 0; c < out_chan; c++)
        float_output[c] = float_data + c * frames;

      // if one day there's input... samples[j++] / 32768.;

      auto now = std::chrono::steady_clock::now();
      auto nsecs
          = std::chrono::duration_cast<std::chrono::nanoseconds>(now - *self.m_start)
                .count()
            / 1e9;

      ossia::audio_tick_state ts{nullptr,  float_output,     0,
                                 out_chan, (uint64_t)frames, nsecs};
      self.audio_tick(ts);

      float* audio_out = self.m_interleaved.data();
      for(int j = 0; j < frames; j++)
        for(int c = 0; c < out_chan; c++)
          *audio_out++ = float_output[c][j];

      SDL_PutAudioStreamData(stream, self.m_interleaved.data(), block_bytes);

      self.tick_end();
      self.m_total_frames += frames;

      additional_amount -= block_bytes;
    }
  }

  SDL_AudioStream* m_stream{};
  SDL_AudioSpec m_spec{};
  std::vector<float> m_interleaved;
  std::vector<float> m_planar;
  std::vector<float*> m_channels;
  uint64_t m_total_frames{};
  std::optional<std::chrono::steady_clock::time_point> m_start;
  bool m_activated{};
};
}

#endif
#endif
