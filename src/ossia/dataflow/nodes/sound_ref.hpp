#pragma once
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/graph_node.hpp>

namespace ossia::nodes
{
class sound_ref final : public ossia::nonowning_graph_node
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

  void reset_resampler(time_value date)
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

  void
  run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
  {
    if (m_data.empty())
    {
      return;
    }

    const std::size_t chan = m_data.size();
    const std::size_t len = m_data[0].size();
    ossia::audio_port& ap = *audio_out.data.target<ossia::audio_port>();
    ap.samples.resize(chan);
    int64_t max_N = std::min(t.date.impl, (int64_t)(len));
    if (max_N <= 0)
      return;

    const int64_t samples_to_read = max_N - t.prev_date + t.offset.impl;
    const int64_t samples_to_write =
        [&] {
      using namespace std;
      if(t.speed < 1)
        return std::min(int64_t(lrint(ceil(samples_to_read / t.speed))), e.bufferSize() - t.offset);
      else if(t.speed > 1)
        return std::max(int64_t(lrint(ceil(samples_to_read / t.speed))), e.bufferSize() - t.offset);
      else
        return samples_to_read;
    }();

    if (samples_to_read <= 0)
      return;

    m_resampler.run(m_data, t, e, chan, len, samples_to_read, samples_to_write, ap);

    // Upmix
    if (upmix != 0)
    {
      if (upmix < chan)
      {
        /* TODO
    // Downmix
    switch(upmix)
    {
      case 1:
      {
        for(std::size_t i = 1; i < chan; i++)
        {
          if(ap.samples[0].size() < ap.samples[i].size())
            ap.samples[0].resize(ap.samples[i].size());

          for(std::size_t j = 0; j < ap.samples[i].size(); j++)
            ap.samples[0][j] += ap.samples[i][j];
        }
      }
      default:
        // TODO
        break;
    }
    */
      }
      else if (upmix > chan)
      {
        switch (chan)
        {
          case 1:
          {
            ap.samples.resize(upmix);
            for (std::size_t i = 1; i < upmix; i++)
            {
              ap.samples[i] = ap.samples[0];
            }
            break;
          }
          default:
            // TODO
            break;
        }
      }
    }

    // Move channels
    if (start != 0)
    {
      ap.samples.insert(ap.samples.begin(), start, ossia::audio_channel{});
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
  audio_span m_data;
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

