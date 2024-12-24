#pragma once
#include <ossia/detail/config.hpp>

//#if 1 || defined(OSSIA_ENABLE_MINIAUDIO)
#define MINIAUDIO_IMPLEMENTATION
#if defined(__APPLE__)
#define MA_NO_RUNTIME_LINKING 1
#endif
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS 1
#define MA_ENABLE_COREAUDIO 1
#define MA_ENABLE_ALSA 1
#define MA_NO_WAV 1
#define MA_NO_FLAC 1
#define MA_NO_MP3 1
#define MA_NO_RESOURCE_MANAGER 1
#define MA_NO_NODE_GRAPH 1
#define MA_NO_GENERATION 1
#define MA_API
#if __has_include(<miniaudio.h>)
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/thread.hpp>

#include <miniaudio.h>

#include <iostream>

#define OSSIA_AUDIO_MINIAUDIO 1

namespace ossia
{
class miniaudio_engine final : public audio_engine
{
public:
  miniaudio_engine(
      std::string name, std::string card_in, std::string card_out, int inputs,
      int outputs, int rate, int bs)
  {
    ma_device_config config = ma_device_config_init(ma_device_type_duplex);
    config.sampleRate = rate;
    config.periodSizeInFrames = bs;
    config.playback.channels = outputs;
    config.playback.format = ma_format_f32;
    config.capture.channels = inputs;
    config.capture.format = ma_format_f32;

    config.dataCallback = callback;

    config.performanceProfile = ma_performance_profile_low_latency;
    config.noFixedSizedCallback = false;
    config.noClip = false;
    config.noDisableDenormals = false;
    config.noPreSilencedOutputBuffer = false;
    config.pUserData = this;

    if(ma_device_init(NULL, &config, &m_stream) != MA_SUCCESS)
      throw std::runtime_error("Cannot initialize miniaudio");
    ma_device_start(&m_stream);
  }

  bool running() const override
  {
    return ma_device_get_state(&m_stream) == ma_device_state_started;
  }

  ~miniaudio_engine() override
  {
    stop();

    ma_device_stop(&m_stream);
    ma_device_uninit(&m_stream);
  }

private:
  static void
  callback(ma_device* pDevice, void* output, const void* input, ma_uint32 nframes)
  {
    [[maybe_unused]]
    static const thread_local auto _
        = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
      return 0;
    }();

    auto& self = *static_cast<miniaudio_engine*>(pDevice->pUserData);
    self.tick_start();

    if(self.stop_processing)
    {
      self.tick_clear();
      return;
    }

    auto float_input = ((float* const*)input);
    auto float_output = ((float**)output);

    ossia::audio_tick_state ts{float_input,
                               float_output,
                               self.effective_inputs,
                               self.effective_outputs,
                               nframes,
                               0};
    self.audio_tick(ts);

    self.tick_end();
  }

  ma_device m_stream;
};
}

#endif
// #endif
