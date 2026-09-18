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
#include <optional>
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
    m_channel_q = boost::circular_buffer<float>(
        m_handle ? 8192 * m_handle.channels() : 0);
    m_window_start = 0;
    m_positioned = false;
  }

  void transport(time_value flicks) override { transport_scaled(flicks, 0.); }

  void transport(time_value flicks, const ossia::tick_transport_info& tinfo) override
  {
    transport_scaled(flicks, tinfo.current_tempo);
  }

  //! Only moves the stretcher's position: fetch_audio() seeks to whatever it
  //! is then asked for, which is not necessarily this.
  void transport_scaled(time_value flicks, double timeline_tempo)
  {
    if(!m_handle)
      return;
    m_resampler.transport(
        file_sample_for_model_time(flicks, timeline_tempo, m_handle.out_rate()));
  }

  //! Decodes one frame into the window. False at end of stream; `pts` is the
  //! frame's timestamp in the stream's time base, unset if it had none.
  bool decode_one(std::optional<int64_t>& pts) noexcept
  {
    auto fmt_ctx = m_handle.format;
    auto codec_ctx = m_handle.codec;
    auto stream = m_handle.stream;
    const std::size_t channels = this->channels();
    if(!fmt_ctx || !codec_ctx || !stream || channels == 0)
      return false;

    for(;;)
    {
      // Drain first: a packet can hold several frames, and sending again
      // before taking them out only ever returns EAGAIN.
      int ret = avcodec_receive_frame(codec_ctx, frame);
      if(ret == 0)
      {
        const int in_samples = frame->nb_samples;
        if(frame->best_effort_timestamp != AV_NOPTS_VALUE)
          pts = frame->best_effort_timestamp;

        const int capacity = m_handle.out_capacity_for(in_samples);
        m_tmp.resize(std::size_t(capacity) * channels, boost::container::default_init);
        float* out_ptr = m_tmp.data();
        const int out_samples = swr_convert(
            m_handle.resample, (uint8_t**)&out_ptr, capacity,
            (const uint8_t**)frame->extended_data, in_samples);

        if(out_samples > 0)
          push_window(out_ptr, out_samples);
        return true;
      }
      if(ret != AVERROR(EAGAIN))
        return false;

      if(m_drained)
        return false;

      av_packet_unref(packet);
      ret = av_read_frame(fmt_ctx, packet);
      while(ret >= 0 && packet->stream_index != stream->index)
      {
        av_packet_unref(packet);
        ret = av_read_frame(fmt_ctx, packet);
      }

      if(ret < 0)
      {
        // Flush the decoder so the tail of the file is not lost.
        m_drained = true;
        avcodec_send_packet(codec_ctx, nullptr);
      }
      else if(avcodec_send_packet(codec_ctx, packet) < 0)
      {
        return false;
      }
    }
  }

  //! Appends `frames` output-rate frames, dropping the oldest to fit.
  void push_window(const float* data, int64_t frames) noexcept
  {
    const int64_t channels = int64_t(this->channels());
    const int64_t capacity = int64_t(m_channel_q.capacity()) / channels;
    const int64_t held = int64_t(m_channel_q.size()) / channels;

    if(frames >= capacity)
    {
      // Larger than the whole window: keep its tail.
      const int64_t keep = capacity;
      m_channel_q.clear();
      m_window_start += held + (frames - keep);
      m_channel_q.insert(
          m_channel_q.end(), data + (frames - keep) * channels,
          data + frames * channels);
      return;
    }

    const int64_t overflow = std::max<int64_t>(0, held + frames - capacity);
    if(overflow > 0)
    {
      m_channel_q.erase_begin(std::size_t(overflow * channels));
      m_window_start += overflow;
    }
    m_channel_q.insert(m_channel_q.end(), data, data + frames * channels);
  }

  //! Positions the window at or before `frame`. False if it cannot be seeked.
  bool seek_window(int64_t frame) noexcept
  {
    m_channel_q.clear();
    m_window_start = 0;
    m_positioned = false;
    m_drained = false;

    const int64_t orate = m_handle.out_rate();
    if(orate <= 0 || !m_handle.stream)
      return false;

    // At or before: packets rarely begin on the requested sample, and
    // ensure_window() can only walk forward from the one that contains it.
    const int64_t flicks = int64_t(
        std::llround(ossia::flicks_per_second<double> * double(frame) / double(orate)));
    if(!ossia::seek_to_flick(
           m_handle.format, m_handle.codec, m_handle.stream, flicks,
           AVSEEK_FLAG_BACKWARD))
      return false;

    m_handle.flush_resampler();

    // The timestamp only lines up with the first output sample right after
    // the flush; from here the position is counted.
    std::optional<int64_t> pts;
    if(!decode_one(pts) || !pts)
      return false;

    const int64_t start_time = (m_handle.stream->start_time != AV_NOPTS_VALUE)
                                   ? m_handle.stream->start_time
                                   : 0;
    const int64_t pos = av_rescale_q(
        *pts - start_time, m_handle.stream->time_base, AVRational{1, int(orate)});
    if(pos < 0)
      return false;

    // decode_one may already have trimmed the front to make its chunk fit;
    // m_window_start holds how much, and the seek position is on top of it.
    m_window_start += pos;
    m_positioned = true;
    return true;
  }

  //! Makes [frame, frame + count) readable, seeking only when out of reach.
  void ensure_window(int64_t frame, int64_t count) noexcept
  {
    const int64_t channels = int64_t(this->channels());
    if(channels == 0 || count <= 0)
      return;

    // Request plus lookahead plus history, so that a stretcher re-reading
    // behind what it consumed costs an index rather than a seek.
    const std::size_t want = std::size_t((2 * count + 32768) * channels);
    if(m_channel_q.capacity() < want)
      m_channel_q.set_capacity(want);

    const int64_t held = int64_t(m_channel_q.size()) / channels;
    const bool in_reach = m_positioned && frame >= m_window_start
                          && frame <= m_window_start + held + count;
    if(!in_reach && !seek_window(frame))
      return;

    // Bounded: a stream whose timestamps jump backwards would otherwise decode
    // the rest of the file inside one audio callback.
    const int64_t limit = int64_t(m_channel_q.capacity()) / channels;
    int64_t decoded = 0;
    while(m_window_start + int64_t(m_channel_q.size()) / channels < frame + count)
    {
      std::optional<int64_t> pts;
      if(!decode_one(pts))
        break;
      if(++decoded > limit)
        break;
    }
  }

  //! Copies [frame, frame + count) out of the window, zeroing what it misses.
  template <typename T>
  void read_window(int64_t frame, int64_t count, T** out) const noexcept
  {
    const int64_t channels = int64_t(this->channels());
    if(channels == 0)
      return;
    const int64_t held = int64_t(m_channel_q.size()) / channels;
    const int64_t offset = frame - m_window_start;

    for(int64_t k = 0; k < count; k++)
    {
      const int64_t i = offset + k;
      if(i >= 0 && i < held)
      {
        auto it = m_channel_q.begin() + std::size_t(i * channels);
        for(int64_t chan = 0; chan < channels; chan++)
          out[chan][k] = T(*it++);
      }
      else
      {
        for(int64_t chan = 0; chan < channels; chan++)
          out[chan][k] = T(0);
      }
    }
  }

  template <typename T>
  void
  fetch_audio(int64_t start, int64_t samples_to_write, T** audio_array_base) noexcept
  {
    if(this->channels() == 0 || samples_to_write <= 0)
      return;
    if(start < 0)
      start = 0;

    ensure_window(start, samples_to_write);
    read_window(start, samples_to_write, audio_array_base);
  }

  template <typename T>
  void fetch_audio_backward(
      int64_t start, int64_t samples_to_write, T** audio_array_base) noexcept
  {
    const int64_t channels = int64_t(this->channels());
    if(channels == 0 || samples_to_write <= 0)
      return;

    // `start` is the newest sample of the span. read_window() zeroes whatever
    // of it falls before the start of the file, so `first` is not clamped.
    const int64_t first = start - samples_to_write + 1;

    ensure_window(std::max<int64_t>(0, first), samples_to_write);
    read_window(first, samples_to_write, audio_array_base);

    for(int64_t chan = 0; chan < channels; chan++)
      std::reverse(audio_array_base[chan], audio_array_base[chan] + samples_to_write);
  }

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_handle)
      return;

    const auto channels = m_handle.channels();
    const auto len = int64_t(this->duration());

    ossia::audio_port& ap = *audio_out;
    ap.set_channels(std::max((std::size_t)upmix, (std::size_t)channels));

    const auto [samples_to_read, samples_to_write]
        = snd::sample_info(e.bufferSize(), e.modelToSamples(), t);
    if(samples_to_write <= 0)
      return;

    assert(samples_to_write > 0);

    const auto samples_offset = t.physical_start(e.modelToSamples());

    if(t.forward())
    {
      if(t.prev_date < m_prev_date)
      {
        // First run after add_time_process() left the stretcher already
        // primed; calling transport() again would reset it.
        if(m_prev_date == ossia::time_value{ossia::time_value::infinite_min})
          m_prev_date = t.prev_date;
        else
          transport(t.prev_date);
      }
    }
    else
    {
      if(t.prev_date > m_prev_date)
      {
        if(m_prev_date == ossia::time_value{ossia::time_value::infinite_min})
          m_prev_date = t.prev_date;
        else
          transport(t.prev_date);
      }
    }

    for(int chan = 0; chan < channels; chan++)
    {
      ap.channel(chan).resize(e.bufferSize());
    }

    const double stretch_ratio = update_stretch(t, e);
    const double abs_stretch_ratio = std::abs(stretch_ratio);

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
  //! In this node's output rate, which is what positions count in.
  [[nodiscard]] std::size_t duration() const
  {
    if(!m_handle)
      return 0;
    const int64_t frames = m_handle.totalPCMFrameCount();
    const int64_t in_rate = m_handle.rate();
    const int64_t out_rate = m_handle.out_rate();
    if(in_rate <= 0 || out_rate <= 0 || in_rate == out_rate)
      return std::size_t(frames);
    return std::size_t(av_rescale_rnd(frames, out_rate, in_rate, AV_ROUND_DOWN));
  }

private:
  libav_handle m_handle{};

  ossia::audio_outlet audio_out;

  std::size_t start{};
  std::size_t upmix{};

  ossia::pod_vector<float> m_tmp{};

  //! Window over the decoded stream: the stretchers do not read strictly
  //! forwards, so what was handed out has to stay readable. m_window_start is
  //! its first frame, in the handle's output rate.
  boost::circular_buffer<float> m_channel_q;
  int64_t m_window_start{};
  bool m_positioned{};
  //! The decoder has been sent its flush packet; there is nothing left to read.
  bool m_drained{};
};

}
