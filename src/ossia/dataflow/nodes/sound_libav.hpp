#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/libav.hpp>
#include <ossia/detail/pod_vector.hpp>

#include <algorithm>
#include <type_traits>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libswresample/swresample.h>
}

namespace ossia::nodes
{
class sound_libav final : public ossia::sound_node
{
  AVPacket* packet{};
  AVFrame* frame{};

public:
  sound_libav()
      : packet{av_packet_alloc()}
      , frame{av_frame_alloc()}
  {
    m_outlets.push_back(&audio_out);
  }

  ~sound_libav()
  {
    m_handle.cleanup();

    av_frame_free(&frame);
    av_packet_free(&packet);
  }

  std::string label() const noexcept override { return "sound_libav"; }

  void set_start(std::size_t v) { start = v; }

  void set_upmix(std::size_t v) { upmix = v; }

  void set_sound(libav_handle hdl)
  {
    using namespace snd;
    m_handle.cleanup();
    m_handle = std::move(hdl);

    m_tmp.clear();
    m_channel_q = boost::circular_buffer<float>(8192 * m_handle.channels());
  }

  void transport(time_value flicks) override
  {
    m_channel_q.clear();
    ossia::seek_to_flick(
        m_handle.format, m_handle.codec, m_handle.stream, flicks.impl, AVSEEK_FLAG_ANY);
  }

  void fetch_from_libav(int samples_to_write)
  {
    const std::size_t channels = this->channels();
    if(channels == 0)
      return;

    auto floats_to_write = channels * samples_to_write;
    while(m_channel_q.size() < floats_to_write)
    {
      // FIXME this is buggy
      if(m_channel_q.capacity() < 4 * floats_to_write)
      {
        m_channel_q.set_capacity(4 * floats_to_write);
      }

      // Need to fetch more data
      auto fmt_ctx = m_handle.format;
      auto codec_ctx = m_handle.codec;
      auto stream = m_handle.stream;
      {
        int ret{};
        {
          av_packet_unref(packet);
          ret = av_read_frame(fmt_ctx, packet);

          while(ret >= 0 && ret != AVERROR(EOF) && packet->stream_index != stream->index)
          {
            av_packet_unref(packet);
            ret = av_read_frame(fmt_ctx, packet);
          }
          if(ret == AVERROR(EOF))
          {
            break;
          }
        }
        if(ret < 0)
        {
          return;
        }

        ret = avcodec_send_packet(codec_ctx, packet);
        if(ret == 0)
        {
          ret = avcodec_receive_frame(codec_ctx, frame);
          if(ret == 0)
          {
            const int samples = frame->nb_samples;
            m_tmp.resize(samples * channels, boost::container::default_init);
            float* out_ptr = m_tmp.data();
            const int read_samples = swr_convert(
                m_handle.resample, (uint8_t**)&out_ptr, samples,
                (const uint8_t**)frame->extended_data, samples);

            m_channel_q.insert(
                m_channel_q.end(), out_ptr, out_ptr + read_samples * channels);
          }
        }
      }
    }
  }

  template <typename T>
  void
  fetch_audio(int64_t start, int64_t samples_to_write, T** audio_array_base) noexcept
  {
    const std::size_t channels = this->channels();
    if(channels == 0)
      return;

    fetch_from_libav(samples_to_write);

    // FIXME start offset
    for(int k = 0; k < samples_to_write; k++)
    {
      for(std::size_t chan = 0; chan < channels; chan++)
      {
        if(m_channel_q.size() > 0)
        {
          audio_array_base[chan][k] = m_channel_q.front();
          m_channel_q.pop_front();
        }
        else
        {
          audio_array_base[chan][k] = 0.;
        }
      }
    }
  }

  template <typename T>
  void fetch_audio_backward(
      int64_t start, int64_t samples_to_write, T** audio_array_base) noexcept
  {
    const std::size_t channels = this->channels();
    if(channels == 0)
      return;

    // For backward playback with libav:
    // 1. Calculate the position we need to start reading from (going backwards)
    // 2. Seek to that earlier position
    // 3. Read forward
    // 4. Reverse the samples

    // Calculate backward start position
    int64_t backward_start = start - samples_to_write + 1;
    if(backward_start < 0)
      backward_start = 0;

    // Seek to the backward position
    // Convert sample position to flicks for seeking
    const int64_t sample_rate = m_handle.stream->codecpar->sample_rate;
    if(sample_rate > 0)
    {
      // flicks = samples * flicks_per_second / sample_rate
      constexpr int64_t flicks_per_second = 705600000LL;
      int64_t flicks_pos = backward_start * flicks_per_second / sample_rate;

      m_channel_q.clear();
      ossia::seek_to_flick(
          m_handle.format, m_handle.codec, m_handle.stream, flicks_pos, AVSEEK_FLAG_BACKWARD);
    }

    // Fetch the audio forward
    fetch_from_libav(samples_to_write);

    // Read into output, then reverse
    for(int64_t k = 0; k < samples_to_write; k++)
    {
      for(std::size_t chan = 0; chan < channels; chan++)
      {
        if(m_channel_q.size() > 0)
        {
          audio_array_base[chan][k] = m_channel_q.front();
          m_channel_q.pop_front();
        }
        else
        {
          audio_array_base[chan][k] = 0.;
        }
      }
    }

    // Reverse each channel in-place
    for(std::size_t chan = 0; chan < channels; chan++)
    {
      std::reverse(audio_array_base[chan], audio_array_base[chan] + samples_to_write);
    }
  }

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_handle)
      return;

    const auto channels = m_handle.channels();
    const auto len = m_handle.totalPCMFrameCount();

    ossia::audio_port& ap = *audio_out;
    ap.set_channels(std::max((std::size_t)upmix, (std::size_t)channels));

    const auto [samples_to_read, samples_to_write]
        = snd::sample_info(e.bufferSize(), e.modelToSamples(), t);
    if(samples_to_write <= 0)
      return;

    assert(samples_to_write > 0);

    const auto samples_offset = t.physical_start(e.modelToSamples());

    // Handle transport for both forward and backward playback
    if(t.forward())
    {
      if(t.prev_date < m_prev_date)
      {
        // Sentinel: we never played.
        if(m_prev_date == ossia::time_value{ossia::time_value::infinite_min})
        {
          if(t.prev_date != 0_tv)
          {
            transport(t.prev_date);
          }
          else
          {
            // Otherwise we don't need transport, everything is already at 0
            m_prev_date = 0_tv;
          }
        }
        else
        {
          transport(t.prev_date);
        }
      }
    }
    else
    {
      // Backward playback transport handling
      if(t.prev_date > m_prev_date)
      {
        // Sentinel: we never played.
        if(m_prev_date == ossia::time_value{ossia::time_value::infinite_min})
        {
          transport(t.prev_date);
        }
        else
        {
          transport(t.prev_date);
        }
      }
    }

    for(int chan = 0; chan < channels; chan++)
    {
      ap.channel(chan).resize(e.bufferSize());
    }

    const double stretch_ratio = update_stretch(t, e);
    const double abs_stretch_ratio = std::abs(stretch_ratio);

    // Resample (handles both forward and backward internally)
    m_resampler.run(
        *this, t, e, stretch_ratio, channels, len, samples_to_read, samples_to_write,
        samples_offset, ap);

    const bool start_discontinuous = t.start_discontinuous || (m_last_stretch > 70.);
    const bool end_discontinuous = t.end_discontinuous || (abs_stretch_ratio > 70.);
    if(abs_stretch_ratio > 70. && m_last_stretch > 70.)
    {
      [[unlikely]];
      for(std::size_t i = 0; i < channels; i++)
      {
        ossia::snd::do_zero(ap.channel(i), samples_offset, samples_to_write);
      }
    }
    else
    {
      [[likely]];
      for(int chan = 0; chan < channels; chan++)
      {
        // fade
        snd::do_fade(
            start_discontinuous, end_discontinuous, ap.channel(chan), samples_offset,
            samples_to_write);
      }
    }

    ossia::snd::perform_upmix(this->upmix, channels, ap);
    ossia::snd::perform_start_offset(this->start, ap);

    m_prev_date = t.date;
    m_last_stretch = abs_stretch_ratio;
  }

  [[nodiscard]] std::size_t channels() const
  {
    return m_handle ? m_handle.channels() : 0;
  }
  [[nodiscard]] std::size_t duration() const
  {
    return m_handle ? m_handle.totalPCMFrameCount() : 0;
  }

private:
  libav_handle m_handle{};

  ossia::audio_outlet audio_out;

  std::size_t start{};
  std::size_t upmix{};

  ossia::pod_vector<float> m_tmp{};
  boost::circular_buffer<float> m_channel_q;
};

}
