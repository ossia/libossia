#pragma once
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/graph_node.hpp>

namespace ossia::nodes
{
class sound_ref final : public ossia::sound_node
{
public:
  sound_ref()
  {
    m_outlets.push_back(&audio_out);
  }

  ~sound_ref()
  {
  }

  std::string label() const noexcept override
  {
    return "sound_ref";
  }

  void set_start(std::size_t v)
  {
    start = v;
  }
  void set_upmix(std::size_t v)
  {
    upmix = v;
  }
  void set_native_tempo(double v)
  {
    tempo = v;
  }

  void set_stretch_mode(ossia::audio_stretch_mode mode)
  {
    if(m_mode != mode)
    {
      m_mode = mode;
      m_resampler.reset(0_tv, m_mode, channels(), m_fileSampleRate);
    }
  }

  void reset_resampler(time_value date) override
  {
    m_resampler.reset(date, m_mode, channels(), m_fileSampleRate);
  }

  void set_sound(const audio_handle& hdl, int channels, int sampleRate)
  {
    m_handle = hdl;
    m_data.clear();
    if (hdl)
    {
      m_fileSampleRate = sampleRate;
      m_data.assign(m_handle->data.begin(), m_handle->data.end());
      m_resampler.reset(0_tv, m_mode, channels, m_fileSampleRate);
    }
  }

  template<typename T>
  void fetch_audio(int64_t start, int64_t samples_to_write, T** audio_array) const noexcept
  {
    const int channels = this->channels();
    const int file_duration = this->duration();
    if(m_loops)
    {
      for(int i = 0; i < channels; i++)
      {
        auto& src = m_data[i];
        T* dst = audio_array[i];

        for(int k = 0; k < samples_to_write; k++)
        {
          int pos =  m_start_offset + ((start + k) % m_loop_duration);
          if(pos < file_duration)
            dst[k] = src[pos];
          else
            dst[k] = 0;
        }
      }
    }
    else
    {
      for(int i = 0; i < channels; i++)
      {
        const auto& src = m_data[i];
        T* dst = audio_array[i];

        if(file_duration >= start + samples_to_write + m_start_offset)
        {
          for(int k = 0, pos = start + m_start_offset;
              k < samples_to_write;
              k++, pos++)
          {
            dst[k] = src[pos];
          }
        }
        else
        {
          const int max = start + samples_to_write + m_start_offset - file_duration;
          for(int k = 0, pos = start + m_start_offset;
              k < max;
              k++, pos++)
          {
            dst[k] = src[pos];
          }
          for(int k = max; k < samples_to_write; k++)
          {
            dst[k] = 0;
          }
        }
      }
    }
  }

  void
  run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
  {
    if (m_data.empty())
      return;

    const std::size_t chan = m_data.size();
    const std::size_t len = m_data[0].size();
    ossia::audio_port& ap = *audio_out.data.target<ossia::audio_port>();
    ap.samples.resize(chan);

    // auto [samples_to_read, samples_to_write] = ossia::snd::sample_info(len, e.bufferSize(), t);

    auto samples_to_read = std::abs(t.date - t.prev_date);
    if(samples_to_read == 0)
      return;

    auto samples_to_write = std::abs(e.bufferSize() - t.offset);
    if(t.speed > 0)
    {
      for (std::size_t i = 0; i < chan; ++i)
      {
        ap.samples[i].resize(t.offset.impl + samples_to_write);
      }

      m_resampler.run(*this, t, e, chan, len, samples_to_read, samples_to_write, ap);

      for(std::size_t i = 0; i < chan; i++)
      {
          ossia::snd::do_fade(
              t.start_discontinuous, t.end_discontinuous, ap.samples[i],
              t.offset.impl, samples_to_write);
      }

      ossia::snd::perform_upmix(this->upmix, chan, ap);
      ossia::snd::perform_start_offset(this->start, ap);
    }
  }

  std::size_t channels() const
  {
    return m_data.size();
  }
  std::size_t duration() const
  {
    return m_data.empty() ? 0 : m_data[0].size();
  }

private:
  audio_span<float> m_data;
  resampler m_resampler;
  ossia::outlet audio_out{ossia::audio_port{}};

  std::size_t start{};
  std::size_t upmix{};
  double tempo{};

  std::size_t m_fileSampleRate{};
  audio_handle m_handle{};
  audio_stretch_mode m_mode{};
};
}

