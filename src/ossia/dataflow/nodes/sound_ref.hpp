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

  ossia::audio_span<float> fetch_audio(int64_t start, int64_t dur) const noexcept
  {
    if(start >= m_data[0].size())
      return {};

    auto source = m_data;
    for(auto& chan : source)
    {
      chan = {chan.data() + start, chan.size() - start};
    }
    return source;
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

    const auto [samples_to_read, samples_to_write] = ossia::snd::sample_info(len, e.bufferSize(), t);

    if (samples_to_read <= 0)
      return;

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

