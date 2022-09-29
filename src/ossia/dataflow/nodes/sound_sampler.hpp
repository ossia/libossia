#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/nodes/sound_utils.hpp>

namespace ossia::nodes
{

struct sound_sampler
{
  void set_start(std::size_t v) { start = v; }

  void set_upmix(std::size_t v) { upmix = v; }

  [[nodiscard]] std::size_t channels() const { return m_data.size(); }

  [[nodiscard]] std::size_t duration() const
  {
    return m_data.empty() ? 0 : m_data[0].size();
  }

  void transport(time_value date)
  {
    info->m_resampler.transport(to_sample(date, m_dataSampleRate));
  }

  // Used for testing only
  void set_sound(audio_array data)
  {
    m_handle = std::make_shared<audio_data>();
    m_handle->data = std::move(data);
    m_data.clear();
    {
      m_dataSampleRate = 44100;
      m_data.assign(m_handle->data.begin(), m_handle->data.end());
      info->m_resampler.reset(
          0, audio_stretch_mode::None, m_handle->data.size(), m_dataSampleRate);
    }
  }

  void set_sound(const audio_handle& hdl, int channels, int sampleRate)
  {
    m_handle = hdl;
    m_data.clear();
    if(hdl)
    {
      m_dataSampleRate = sampleRate;
      m_data.assign(m_handle->data.begin(), m_handle->data.end());
    }
  }

  template <typename T>
  void fetch_audio(
      const int64_t start, const int64_t samples_to_write,
      T** const audio_array) const noexcept
  {
    read_audio_from_buffer(
        m_data, start, samples_to_write, info->m_start_offset_samples,
        info->m_loop_duration_samples, info->m_loops, audio_array);
  }

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept
  {
    if(m_data.empty())
      return;

    // TODO do the backwards play head
    if(!t.forward())
      return;

    const std::size_t chan = m_data.size();
    const std::size_t len = m_data[0].size();
    ossia::audio_port& ap = *audio_out;
    ap.set_channels(std::max(this->upmix, chan));

    const auto [samples_to_read, samples_to_write]
        = snd::sample_info(e.bufferSize(), e.modelToSamples(), t);
    if(samples_to_read == 0)
      return;
    if(samples_to_write <= 0)
      return;

    assert(samples_to_write > 0);

    const auto samples_offset = t.physical_start(e.modelToSamples());
    if(t.tempo > 0)
    {
      if(t.prev_date < info->m_prev_date)
      {
        // Sentinel: we never played.
        if(info->m_prev_date == ossia::time_value{ossia::time_value::infinite_min})
        {
          if(t.prev_date != 0_tv)
          {
            transport(t.prev_date);
          }
          else
          {
            // Otherwise we don't need transport, everything is already at 0
            info->m_prev_date = 0_tv;
          }
        }
        else
        {
          transport(t.prev_date);
        }
      }

      for(std::size_t i = 0; i < chan; ++i)
      {
        ap.channel(i).resize(e.bufferSize());
      }

      double stretch_ratio = info->update_stretch(t, e);

      // Resample
      info->m_resampler.run(
          *this, t, e, stretch_ratio, chan, len, samples_to_read, samples_to_write,
          samples_offset, ap);

      for(std::size_t i = 0; i < chan; i++)
      {
        ossia::snd::do_fade(
            t.start_discontinuous, t.end_discontinuous, ap.channel(i), samples_offset,
            samples_to_write);
      }

      ossia::snd::perform_upmix(this->upmix, chan, ap);
      ossia::snd::perform_start_offset(this->start, ap);

      info->m_prev_date = t.date;
    }
  }

  sound_processing_info* info{};
  ossia::audio_port* audio_out{};

  audio_span<float> m_data{};

  std::size_t start{};
  std::size_t upmix{};

  std::size_t m_dataSampleRate{};
  audio_handle m_handle{};
};
}
