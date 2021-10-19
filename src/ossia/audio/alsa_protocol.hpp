#pragma once
#if __has_include(<alsa/asoundlib.h>) && !defined(__EMSCRIPTEN__)

#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/dylib_loader.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/dataflow/float_to_sample.hpp>

#include <alsa/asoundlib.h>

#include <thread>
#include <vector>
#include <atomic>
#include <string_view>

#define OSSIA_AUDIO_ALSA 1

namespace ossia
{
#define snd_alloca(ptr, lib, type) do { *ptr = (snd_##type##_t *) alloca(lib.type##_sizeof()); memset(*ptr, 0, lib.type##_sizeof()); } while (0)
class libasound
{
public:
    decltype(&::snd_pcm_open) pcm_open{};
    decltype(&::snd_pcm_hw_params_any) pcm_hw_params_any{};
    decltype(&::snd_pcm_hw_params_sizeof) pcm_hw_params_sizeof{};
    decltype(&::snd_pcm_hw_params_set_access) pcm_hw_params_set_access{};
    decltype(&::snd_pcm_hw_params_set_format) pcm_hw_params_set_format{};
    decltype(&::snd_pcm_hw_params_set_channels) pcm_hw_params_set_channels{};
    decltype(&::snd_pcm_hw_params_set_period_size) pcm_hw_params_set_period_size{};
    decltype(&::snd_pcm_hw_params_set_buffer_size) pcm_hw_params_set_buffer_size{};
    decltype(&::snd_pcm_hw_params_set_periods) pcm_hw_params_set_periods{};
    decltype(&::snd_pcm_hw_params_set_periods_near) pcm_hw_params_set_periods_near{};
    decltype(&::snd_pcm_hw_params_set_rate_near) pcm_hw_params_set_rate_near{};
    decltype(&::snd_pcm_hw_params_get_channels) pcm_hw_params_get_channels{};
    decltype(&::snd_pcm_hw_params_get_channels_min) pcm_hw_params_get_channels_min{};
    decltype(&::snd_pcm_hw_params_get_channels_max) pcm_hw_params_get_channels_max{};
    decltype(&::snd_pcm_hw_params_get_rate) pcm_hw_params_get_rate{};
    decltype(&::snd_pcm_hw_params_get_rate_min) pcm_hw_params_get_rate_min{};
    decltype(&::snd_pcm_hw_params_get_rate_max) pcm_hw_params_get_rate_max{};
    decltype(&::snd_pcm_hw_params_get_buffer_size) pcm_hw_params_get_buffer_size{};
    decltype(&::snd_pcm_hw_params_get_buffer_size_min) pcm_hw_params_get_buffer_size_min{};
    decltype(&::snd_pcm_hw_params_get_buffer_size_max) pcm_hw_params_get_buffer_size_max{};
    decltype(&::snd_pcm_hw_params_get_period_size) pcm_hw_params_get_period_size{};
    decltype(&::snd_pcm_hw_params_get_period_size_min) pcm_hw_params_get_period_size_min{};
    decltype(&::snd_pcm_hw_params_get_period_size_max) pcm_hw_params_get_period_size_max{};
    decltype(&::snd_pcm_hw_params_get_periods) pcm_hw_params_get_periods{};
    decltype(&::snd_pcm_hw_params) pcm_hw_params{};
    decltype(&::snd_pcm_sw_params_current) pcm_sw_params_current{};
    decltype(&::snd_pcm_sw_params_sizeof) pcm_sw_params_sizeof{};
    decltype(&::snd_pcm_sw_params_set_start_threshold) pcm_sw_params_set_start_threshold{};
    decltype(&::snd_pcm_sw_params_set_avail_min) pcm_sw_params_set_avail_min{};
    decltype(&::snd_pcm_sw_params) pcm_sw_params{};
    decltype(&::snd_pcm_name) pcm_name{};
    decltype(&::snd_pcm_state) pcm_state{};
    decltype(&::snd_pcm_state_name) pcm_state_name{};
    decltype(&::snd_pcm_writei) pcm_writei{};
    decltype(&::snd_pcm_prepare) pcm_prepare{};
    decltype(&::snd_pcm_start) pcm_start{};
    decltype(&::snd_pcm_recover) pcm_recover{};
    decltype(&::snd_pcm_drain) pcm_drain{};
    decltype(&::snd_pcm_close) pcm_close{};
    decltype(&::snd_strerror) strerror{};
    decltype(&::snd_device_name_hint) device_name_hint{};
    decltype(&::snd_device_name_get_hint) device_name_get_hint{};
    decltype(&::snd_device_name_free_hint) device_name_free_hint{};

    static const libasound& instance() {
        static const libasound self;
        return self;
    }

private:
    dylib_loader library;

    libasound()
        :library("libasound.so.2")
    {
        // in terms of regex:
        // decltype\(&::([a-z_]+)\) [a-z_]+{};
        // \1 = library.symbol<decltype(&::\1)>("\1");

        pcm_open = library.symbol<decltype(&::snd_pcm_open)>("snd_pcm_open");
        pcm_hw_params_any = library.symbol<decltype(&::snd_pcm_hw_params_any)>("snd_pcm_hw_params_any");
        pcm_hw_params_sizeof = library.symbol<decltype(&::snd_pcm_hw_params_sizeof)>("snd_pcm_hw_params_sizeof");
        pcm_hw_params_set_access = library.symbol<decltype(&::snd_pcm_hw_params_set_access)>("snd_pcm_hw_params_set_access");
        pcm_hw_params_set_format = library.symbol<decltype(&::snd_pcm_hw_params_set_format)>("snd_pcm_hw_params_set_format");
        pcm_hw_params_set_channels = library.symbol<decltype(&::snd_pcm_hw_params_set_channels)>("snd_pcm_hw_params_set_channels");
        pcm_hw_params_set_period_size = library.symbol<decltype(&::snd_pcm_hw_params_set_period_size)>("snd_pcm_hw_params_set_period_size");
        pcm_hw_params_set_buffer_size = library.symbol<decltype(&::snd_pcm_hw_params_set_buffer_size)>("snd_pcm_hw_params_set_buffer_size");
        pcm_hw_params_set_periods = library.symbol<decltype(&::snd_pcm_hw_params_set_periods)>("snd_pcm_hw_params_set_periods");
        pcm_hw_params_set_periods_near = library.symbol<decltype(&::snd_pcm_hw_params_set_periods_near)>("snd_pcm_hw_params_set_periods_near");
        pcm_hw_params_set_rate_near = library.symbol<decltype(&::snd_pcm_hw_params_set_rate_near)>("snd_pcm_hw_params_set_rate_near");
        pcm_hw_params_get_channels = library.symbol<decltype(&::snd_pcm_hw_params_get_channels)>("snd_pcm_hw_params_get_channels");
        pcm_hw_params_get_channels_min = library.symbol<decltype(&::snd_pcm_hw_params_get_channels_min)>("snd_pcm_hw_params_get_channels_min");
        pcm_hw_params_get_channels_max = library.symbol<decltype(&::snd_pcm_hw_params_get_channels_max)>("snd_pcm_hw_params_get_channels_max");
        pcm_hw_params_get_rate = library.symbol<decltype(&::snd_pcm_hw_params_get_rate)>("snd_pcm_hw_params_get_rate");
        pcm_hw_params_get_rate_min = library.symbol<decltype(&::snd_pcm_hw_params_get_rate_min)>("snd_pcm_hw_params_get_rate_min");
        pcm_hw_params_get_rate_max = library.symbol<decltype(&::snd_pcm_hw_params_get_rate_max)>("snd_pcm_hw_params_get_rate_max");
        pcm_hw_params_get_buffer_size = library.symbol<decltype(&::snd_pcm_hw_params_get_buffer_size)>("snd_pcm_hw_params_get_buffer_size");
        pcm_hw_params_get_buffer_size_min = library.symbol<decltype(&::snd_pcm_hw_params_get_buffer_size_min)>("snd_pcm_hw_params_get_buffer_size_min");
        pcm_hw_params_get_buffer_size_max = library.symbol<decltype(&::snd_pcm_hw_params_get_buffer_size_max)>("snd_pcm_hw_params_get_buffer_size_max");
        pcm_hw_params_get_period_size = library.symbol<decltype(&::snd_pcm_hw_params_get_period_size)>("snd_pcm_hw_params_get_period_size");
        pcm_hw_params_get_period_size_min = library.symbol<decltype(&::snd_pcm_hw_params_get_period_size_min)>("snd_pcm_hw_params_get_period_size_min");
        pcm_hw_params_get_period_size_max = library.symbol<decltype(&::snd_pcm_hw_params_get_period_size_max)>("snd_pcm_hw_params_get_period_size_max");
        pcm_hw_params_get_periods = library.symbol<decltype(&::snd_pcm_hw_params_get_periods)>("snd_pcm_hw_params_get_periods");
        pcm_hw_params = library.symbol<decltype(&::snd_pcm_hw_params)>("snd_pcm_hw_params");
        pcm_sw_params_current = library.symbol<decltype(&::snd_pcm_sw_params_current)>("snd_pcm_sw_params_current");
        pcm_sw_params_sizeof = library.symbol<decltype(&::snd_pcm_sw_params_sizeof)>("snd_pcm_sw_params_sizeof");
        pcm_sw_params_set_start_threshold = library.symbol<decltype(&::snd_pcm_sw_params_set_start_threshold)>("snd_pcm_sw_params_set_start_threshold");
        pcm_sw_params_set_avail_min = library.symbol<decltype(&::snd_pcm_sw_params_set_avail_min)>("snd_pcm_sw_params_set_avail_min");
        pcm_sw_params = library.symbol<decltype(&::snd_pcm_sw_params)>("snd_pcm_sw_params");
        pcm_name = library.symbol<decltype(&::snd_pcm_name)>("snd_pcm_name");
        pcm_state = library.symbol<decltype(&::snd_pcm_state)>("snd_pcm_state");
        pcm_state_name = library.symbol<decltype(&::snd_pcm_state_name)>("snd_pcm_state_name");
        pcm_writei = library.symbol<decltype(&::snd_pcm_writei)>("snd_pcm_writei");
        pcm_prepare = library.symbol<decltype(&::snd_pcm_prepare)>("snd_pcm_prepare");
        pcm_start = library.symbol<decltype(&::snd_pcm_start)>("snd_pcm_start");
        pcm_recover = library.symbol<decltype(&::snd_pcm_recover)>("snd_pcm_recover");
        pcm_drain = library.symbol<decltype(&::snd_pcm_drain)>("snd_pcm_drain");
        pcm_close = library.symbol<decltype(&::snd_pcm_close)>("snd_pcm_close");
        strerror = library.symbol<decltype(&::snd_strerror)>("snd_strerror");
        device_name_hint = library.symbol<decltype(&::snd_device_name_hint)>("snd_device_name_hint");
        device_name_get_hint = library.symbol<decltype(&::snd_device_name_get_hint)>("snd_device_name_get_hint");
        device_name_free_hint = library.symbol<decltype(&::snd_device_name_free_hint)>("snd_device_name_free_hint");

        // in terms of regex:
        // decltype\(&::([a-z_]+)\) [a-z_]+{};
        // assert(\1);
        assert(pcm_open);
        assert(pcm_hw_params_any);
        assert(pcm_hw_params_sizeof);
        assert(pcm_hw_params_set_access);
        assert(pcm_hw_params_set_format);
        assert(pcm_hw_params_set_channels);
        assert(pcm_hw_params_set_period_size);
        assert(pcm_hw_params_set_buffer_size);
        assert(pcm_hw_params_set_periods);
        assert(pcm_hw_params_set_periods_near);
        assert(pcm_hw_params_set_rate_near);
        assert(pcm_hw_params_get_channels);
        assert(pcm_hw_params_get_channels_min);
        assert(pcm_hw_params_get_channels_max);
        assert(pcm_hw_params_get_rate);
        assert(pcm_hw_params_get_rate_min);
        assert(pcm_hw_params_get_rate_max);
        assert(pcm_hw_params_get_buffer_size);
        assert(pcm_hw_params_get_buffer_size_min);
        assert(pcm_hw_params_get_buffer_size_max);
        assert(pcm_hw_params_get_period_size);
        assert(pcm_hw_params_get_period_size_min);
        assert(pcm_hw_params_get_period_size_max);
        assert(pcm_hw_params_get_periods);
        assert(pcm_hw_params);
        assert(pcm_sw_params_current);
        assert(pcm_sw_params_sizeof);
        assert(pcm_sw_params_set_start_threshold);
        assert(pcm_sw_params_set_avail_min);
        assert(pcm_sw_params);
        assert(pcm_name);
        assert(pcm_state);
        assert(pcm_state_name);
        assert(pcm_writei);
        assert(pcm_prepare);
        assert(pcm_start);
        assert(pcm_recover);
        assert(pcm_drain);
        assert(pcm_close);
        assert(strerror);
        assert(device_name_hint);
        assert(device_name_get_hint);
        assert(device_name_free_hint);
    }
};

struct exception : std::runtime_error {
  template<typename... Args>
  exception(fmt::format_string<Args...> format,Args&&... args)
    : std::runtime_error{fmt::format(format, std::forward<Args>(args)...)}
  {

  }
};

template<auto Format>
static constexpr void snd_interleave(const float* const* in, char* out, int channels, int bs)
{
  switch(Format) {
    case SND_PCM_FORMAT_S16_LE:
      return ossia::interleave<int16_t, 16>(in, reinterpret_cast<int16_t*>(out), channels, bs);
    case SND_PCM_FORMAT_S24_LE:
      return ossia::interleave<int32_t, 24>(in, reinterpret_cast<int32_t*>(out), channels, bs);
    case SND_PCM_FORMAT_S32_LE:
      return ossia::interleave<int32_t, 32>(in, reinterpret_cast<int32_t*>(out), channels, bs);
    case SND_PCM_FORMAT_FLOAT_LE:
      return ossia::interleave<float, 32>(in, reinterpret_cast<float*>(out), channels, bs);
    default:
      return;
  }
}

template<auto Format>
static constexpr void snd_convert(const float* const* in, char* out, int channels, int bs)
{
  switch(Format) {
    case SND_PCM_FORMAT_S16_LE:
      return ossia::convert<int16_t, 16>(in, reinterpret_cast<int16_t*>(out), channels, bs);
    case SND_PCM_FORMAT_S24_LE:
      return ossia::convert<int32_t, 24>(in, reinterpret_cast<int32_t*>(out), channels, bs);
    case SND_PCM_FORMAT_S32_LE:
      return ossia::convert<int32_t, 32>(in, reinterpret_cast<int32_t*>(out), channels, bs);
    case SND_PCM_FORMAT_FLOAT_LE:
      return ossia::convert<float, 32>(in, reinterpret_cast<float*>(out), channels, bs);
    default:
      return;
  }
}

template<auto Format>
static constexpr int snd_bytes_per_sample()
{
  switch(Format) {
    case SND_PCM_FORMAT_S16_LE: return 2;
    case SND_PCM_FORMAT_S24_LE: return 4;
    case SND_PCM_FORMAT_S32_LE: return 4;
    case SND_PCM_FORMAT_FLOAT_LE: return 4;
    default: return 4;
  }
}

class alsa_engine final : public audio_engine
{
public:
  alsa_engine(std::string /* card_in */, std::string card_out, int inputs, int outputs, int rate, int bs)
  {
    const auto &snd = libasound::instance();

    {
      if (int ret = snd.pcm_open(&m_client, card_out.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
          ret < 0)
        throw ossia::exception("alsa_engine: error when opening device '{}': {}", card_out, snd.strerror(ret));

      snd_pcm_hw_params_t *hwparams;
      snd_alloca(&hwparams, snd, pcm_hw_params);
      snd.pcm_hw_params_any(m_client, hwparams);

      // snd_pcm_sw_params_t *swparams;
      // snd_pcm_sw_params_alloca(&swparams);

      auto access = SND_PCM_ACCESS_RW_NONINTERLEAVED;
      if (int ret = snd.pcm_hw_params_set_access(m_client, hwparams, access);
          ret < 0)
      {
        ossia::logger().error("alsa_engine: can't set noninterleaved mode: {}", snd.strerror(ret));

        access = SND_PCM_ACCESS_RW_INTERLEAVED;
      }

      auto format = SND_PCM_FORMAT_FLOAT_LE;
      if (int ret = snd.pcm_hw_params_set_format(m_client, hwparams, format); ret < 0)
      {
        format = SND_PCM_FORMAT_S32_LE;
        if (int ret = snd.pcm_hw_params_set_format(m_client, hwparams, format); ret < 0)
        {
          format = SND_PCM_FORMAT_S24_LE;
          if (int ret = snd.pcm_hw_params_set_format(m_client, hwparams, format); ret < 0)
          {
            format = SND_PCM_FORMAT_S16_LE;
            if (int ret = snd.pcm_hw_params_set_format(m_client, hwparams, format); ret < 0)
            {
              ossia::logger().error("alsa_engine: can't set format: {}", snd.strerror(ret));
            }
          }
        }
      }

      if (int ret = snd.pcm_hw_params_set_channels(m_client, hwparams, outputs);
          ret < 0)
        ossia::logger().error("alsa_engine: can't set channels number. {}", snd.strerror(ret));

      if (int ret = snd.pcm_hw_params_set_period_size(m_client, hwparams, bs, 0);
            ret < 0)
          ossia::logger().error("alsa_engine: can't set period size. {}", snd.strerror(ret));

      unsigned int pnear = 1;
      if (int ret = snd.pcm_hw_params_set_periods_near(m_client, hwparams, &pnear, 0);
          ret < 0)
        ossia::logger().error("alsa_engine: can't set periods. {}", snd.strerror(ret));

      //if (int ret = snd.pcm_hw_params_set_buffer_size(m_client, hwparams, 2 * bs);
      //    ret < 0)
      //  ossia::logger().error("alsa_engine: can't set buffer size. {}", snd.strerror(ret));

      unsigned int urate = rate;
      if (int ret = snd.pcm_hw_params_set_rate_near(m_client, hwparams, &urate, 0);
          ret < 0)
        ossia::logger().error("alsa_engine: can't set rate. {}", snd.strerror(ret));

      if (int ret = snd.pcm_hw_params(m_client, hwparams); ret < 0)
        ossia::logger().error("alsa_engine: snd_pcm_hw_params: {}", snd.strerror(ret));

      this->effective_inputs = 0;
      {
        unsigned int tmp{};
        snd.pcm_hw_params_get_channels(hwparams, &tmp);
        this->effective_outputs = tmp;
      }

      {
        unsigned int tmp{};
        snd.pcm_hw_params_get_rate(hwparams, &tmp, 0);
        this->effective_sample_rate = tmp;
      }

      {
        snd_pcm_uframes_t tmp{};
        snd.pcm_hw_params_get_period_size(hwparams, &tmp, 0);
        this->effective_buffer_size = tmp;
      }
      {
        snd_pcm_uframes_t tmp_bufsize{};
        snd.pcm_hw_params_get_buffer_size(hwparams, &tmp_bufsize);
        unsigned int tmp_periods{};
        snd.pcm_hw_params_get_periods(hwparams, &tmp_periods, 0);

        fmt::print("Device: {}\n", card_out);
        fmt::print("Expected: {} : {} : {}\n", outputs, rate, bs);
        fmt::print("Got: {} : {} : {} => {} ; {}\n", effective_outputs, effective_sample_rate, effective_buffer_size, tmp_bufsize, tmp_periods);
      }

      if(access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
      {
        switch(format)
        {
          case SND_PCM_FORMAT_S16_LE:
            m_thread = std::thread([this] { run_thread_deinterleaved<SND_PCM_FORMAT_S16_LE>(); });
            m_activated = true;
            break;
          case SND_PCM_FORMAT_S24_LE:
            m_thread = std::thread([this] { run_thread_deinterleaved<SND_PCM_FORMAT_S24_LE>(); });
            m_activated = true;
            break;
          case SND_PCM_FORMAT_S32_LE:
            m_thread = std::thread([this] { run_thread_deinterleaved<SND_PCM_FORMAT_S32_LE>(); });
            m_activated = true;
            break;
          case SND_PCM_FORMAT_FLOAT_LE:
            m_thread = std::thread([this] { run_thread_deinterleaved<SND_PCM_FORMAT_FLOAT_LE>(); });
            m_activated = true;
            break;
          default:
            m_activated = false;
            break;
        }
      }
      else
      {
        switch(format)
        {
          case SND_PCM_FORMAT_S16_LE:
            m_thread = std::thread([this] { run_thread_interleaved<SND_PCM_FORMAT_S16_LE>(); });
            m_activated = true;
            break;
          case SND_PCM_FORMAT_S24_LE:
            m_thread = std::thread([this] { run_thread_interleaved<SND_PCM_FORMAT_S24_LE>(); });
            m_activated = true;
            break;
          case SND_PCM_FORMAT_S32_LE:
            m_thread = std::thread([this] { run_thread_interleaved<SND_PCM_FORMAT_S32_LE>(); });
            m_activated = true;
            break;
          case SND_PCM_FORMAT_FLOAT_LE:
            m_thread = std::thread([this] { run_thread_interleaved<SND_PCM_FORMAT_FLOAT_LE>(); });
            m_activated = true;
            break;
          default:
            m_activated = false;
            break;
        }
      }
    }
  }

  ~alsa_engine() override
  {
    stop();

    if(m_client)
    {
        m_stop_token = true;
        assert(m_thread.joinable());
        m_thread.join();

        snd.pcm_drain(m_client);
        snd.pcm_close(m_client);
    }
  }

  bool running() const override
  {
    if(!m_client)
      return false;
    return m_activated;
  }

private:
  void clear_buffers()
  {
    ossia::fill(this->m_temp_buffer, 0.f);
    ossia::fill(this->m_deinterleaved_buffer, 0.f);
  }

  bool submit(void* data)
  {
    if (int ret = snd.pcm_writei(m_client, data, this->effective_buffer_size); ret == -EPIPE) {
      ossia::logger().error("alsa_engine: snd_pcm_writei: buffer underrun.");
      snd.pcm_prepare(m_client);
    } else if (ret < 0) {
      return true;
      m_activated = false;
      m_stop_token = true;
      this->stop_processing = true;
      return false;
    } else {
      snd.pcm_start(m_client);
    }
    return true;
  }

  template<auto Format>
  void run_thread_interleaved()
  {
    m_temp_buffer.resize(this->effective_outputs * this->effective_buffer_size * snd_bytes_per_sample<Format>());
    m_deinterleaved_buffer.resize(this->effective_outputs * this->effective_buffer_size);

    float** outs = (float**)alloca(sizeof(float*) * this->effective_outputs);
    for(int c = 0; c < this->effective_outputs; c++)
      outs[c] = m_deinterleaved_buffer.data() + c * this->effective_buffer_size;

    m_start_time = clk::now();
    m_last_time = m_start_time;
    while(!this->m_stop_token)
    {
      process(nullptr, outs, this->effective_buffer_size);

      snd_interleave<Format>(outs, m_temp_buffer.data(), this->effective_outputs, this->effective_buffer_size);

      if(!submit(m_temp_buffer.data()))
        break;
    }
  }

  template<auto Format>
  void run_thread_deinterleaved()
  {
    // In the float case we skip the temp buffer
    if constexpr(Format != SND_PCM_FORMAT_FLOAT_LE)
    {
      m_temp_buffer.resize(this->effective_outputs * this->effective_buffer_size * snd_bytes_per_sample<Format>());
    }
    m_deinterleaved_buffer.resize(this->effective_outputs * this->effective_buffer_size);

    float** outs = (float**)alloca(sizeof(float*) * this->effective_outputs);
    for(int c = 0; c < this->effective_outputs; c++)
      outs[c] = m_deinterleaved_buffer.data() + c * this->effective_buffer_size;

    m_start_time = clk::now();
    m_last_time = m_start_time;
    while(!this->m_stop_token)
    {
      process(nullptr, outs, this->effective_buffer_size);

      if constexpr(Format != SND_PCM_FORMAT_FLOAT_LE)
      {
        // Convert into the temp buffer
        snd_convert<Format>(outs, m_temp_buffer.data(), this->effective_outputs, this->effective_buffer_size);

        if(!submit(m_temp_buffer.data()))
          break;
      }
      else
      {
        // Directly submit our buffer
        if(!submit(m_deinterleaved_buffer.data()))
          break;
      }
    }
  }

  void process(float** , float** float_output, uint64_t nframes)
  {
    auto& self = *static_cast<alsa_engine*>(this);
    self.tick_start();

    const auto inputs = this->effective_inputs;
    const auto outputs = this->effective_outputs;
    if (self.stop_processing)
    {
      self.tick_clear();
      clear_buffers();
      return;
    }

    using namespace std::chrono;
    ossia::audio_tick_state ts{
      nullptr, float_output,
      (int)inputs, (int)outputs,
      nframes, duration_cast<nanoseconds>(m_last_time - m_start_time).count() / 1e9
    };
    self.audio_tick(ts);

    self.tick_end();
    m_last_time = clk::now();
  }

  using clk = std::chrono::steady_clock;

  const libasound& snd{libasound::instance()};

  snd_pcm_t* m_client{};
  std::thread m_thread;
  ossia::pod_vector<char> m_temp_buffer;
  ossia::float_vector m_deinterleaved_buffer;

  clk::time_point m_start_time{};
  clk::time_point m_last_time{};

  std::atomic_bool m_stop_token{};
  bool m_activated = false;
};


}

#endif
