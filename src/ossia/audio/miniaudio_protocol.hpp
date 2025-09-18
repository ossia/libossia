#pragma once
#include <ossia/detail/config.hpp>

#if __has_include(<miniaudio.h>)
#define OSSIA_ENABLE_MINIAUDIO 1
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
#define MA_MAX_CHANNELS 1024
#define MA_API

#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/thread.hpp>

#include <kfr/base/conversion.hpp>

#include <miniaudio.h>

#define OSSIA_AUDIO_MINIAUDIO 1

namespace ossia
{
struct miniaudio_context
{
  ma_context context;
};

class miniaudio_engine final : public audio_engine
{

public:
  std::vector<const ma_device_info*> devices;
  bool is_duplex(const ma_device_id& card_in, const ma_device_id& card_out)
  {
#if defined(__APPLE__)
    return true;
    std::string_view i = card_in.coreaudio;
    std::string_view o = card_out.coreaudio;
    if(i.length() != o.length() || i.empty() || o.empty())
      return false;
    if(i.substr(0, i.size() - 1) == o.substr(0, o.size() - 1))
      return true;

#endif
    return memcmp(&card_in, &card_out, sizeof(ma_device_id)) == 0;
  }
  miniaudio_engine(
      std::shared_ptr<miniaudio_context> ctx, std::string name,
      const ma_device_id& card_in, const ma_device_id& card_out, int inputs, int outputs,
      int rate, int bs)
      : m_ctx{std::move(ctx)}
  {
    ma_device_type dtype = ma_device_type_duplex;
    if(inputs == 0)
      dtype = ma_device_type_playback;
    else if(outputs == 0)
      dtype = ma_device_type_capture;

    ma_device_config config = ma_device_config_init(dtype);

    config.sampleRate = rate;
    config.periodSizeInFrames = bs;

    if(outputs > 0)
    {
      config.playback.pDeviceID = &card_out;
      config.playback.channels = outputs;
      config.playback.format = ma_format_f32;
      // config.playback.shareMode = ma_share_mode_exclusive;
    }

    if(inputs > 0)
    {
      config.capture.pDeviceID = &card_in;
      config.capture.channels = inputs;
      config.capture.format = ma_format_f32;
      // config.capture.shareMode = ma_share_mode_exclusive;
    }

    config.dataCallback = callback;

    config.performanceProfile = ma_performance_profile_low_latency;
    config.noFixedSizedCallback = false;
    config.noClip = false;
    config.noDisableDenormals = false;
    config.noPreSilencedOutputBuffer = false;

    config.pUserData = this;

    this->effective_buffer_size = bs;
    this->effective_sample_rate = rate;
    this->effective_inputs = inputs;
    this->effective_outputs = outputs;

    ins_data.resize(effective_inputs * bs + 16);
    outs_data.resize(effective_outputs * bs + 16);
    ins.resize(effective_inputs + 2);
    outs.resize(effective_outputs + 2);

    if(ma_device_init(&m_ctx->context, &config, &m_stream) != MA_SUCCESS)
    {
      config.performanceProfile = ma_performance_profile_conservative;
      if(ma_device_init(&m_ctx->context, &config, &m_stream) != MA_SUCCESS)
      {
        throw std::runtime_error("Cannot initialize miniaudio");
      }
    }

    if(ma_device_start(&m_stream) != MA_SUCCESS)
      throw std::runtime_error("Cannot start miniaudio");
    m_active = true;
  }

  bool running() const override
  {
    return m_active && ma_device_get_state(&m_stream) == ma_device_state_started;
  }

  void stop() override
  {
    audio_engine::stop();

    if(m_active)
    {
      ma_device_stop(&m_stream);
      ma_device_uninit(&m_stream);
    }
    m_active = false;
  }

  ~miniaudio_engine() override { stop(); }

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
    if(!self.m_start)
      self.m_start = std::chrono::steady_clock::now();

    if(self.stop_processing)
    {
      self.tick_clear();
      return;
    }

    auto ins = self.ins.data();
    auto ins_data = self.ins_data.data();
    for(int i = 0; i < self.effective_inputs; i++)
      ins[i] = ins_data + i * nframes;
    kfr::deinterleave(ins, (float*)input, self.effective_inputs, nframes);

    auto outs = self.outs.data();
    auto outs_data = self.outs_data.data();
    std::memset(outs_data, 0, sizeof(float) * self.effective_outputs * nframes);
    for(int i = 0; i < self.effective_outputs; i++)
      outs[i] = outs_data + i * nframes;

    auto now = std::chrono::steady_clock::now();
    auto nsecs
        = std::chrono::duration_cast<std::chrono::nanoseconds>(now - *self.m_start)
              .count()
          / 1e9;

    ossia::audio_tick_state ts{(float* const*)ins,     outs,    self.effective_inputs,
                               self.effective_outputs, nframes, nsecs};
    self.audio_tick(ts);

    self.tick_end();

    kfr::interleave(
        (float*)output, (const float**)outs, self.effective_outputs, nframes);
  }

  std::shared_ptr<miniaudio_context> m_ctx;
  ma_device m_stream;
  std::optional<std::chrono::steady_clock::time_point> m_start;

  boost::container::vector<float> ins_data;
  boost::container::vector<float*> ins;
  boost::container::vector<float> outs_data;
  boost::container::vector<float*> outs;

  bool m_active{};
};
}

#endif
// #endif
