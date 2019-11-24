#pragma once
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/nodes/timestretch/raw_stretcher.hpp>
#include <ossia/dataflow/nodes/timestretch/rubberband_stretcher.hpp>
#include <ossia/dataflow/nodes/timestretch/repitch_stretcher.hpp>
#include <variant>

namespace ossia
{
namespace snd
{


// TODO refactor with AudioDecoder
template <typename SampleFormat, int N>
constexpr audio_sample convert_sample(SampleFormat i);

template <>
constexpr audio_sample convert_sample<int16_t, 16>(int16_t i)
{
  // TODO division -> multiplication
  if constexpr (std::is_same_v<ossia::audio_sample, float>)
      return (i + .5f) / (0x7FFF + .5f);
  else
  return (i + .5) / (0x7FFF + .5);
}

template <>
constexpr audio_sample convert_sample<int32_t, 24>(int32_t i)
{
  if constexpr (std::is_same_v<ossia::audio_sample, float>)
      return ((int32_t)i >> 8)
      / ((audio_sample)std::numeric_limits<int32_t>::max() / 256.f);
  else
  return ((int32_t)i >> 8)
      / ((audio_sample)std::numeric_limits<int32_t>::max() / 256.);
}

template <>
constexpr audio_sample convert_sample<int32_t, 32>(int32_t i)
{
  return i / (audio_sample)(std::numeric_limits<int32_t>::max());
}

template <>
constexpr audio_sample convert_sample<float, 32>(float i)
{
  return i;
}

inline void read_s16(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<int16_t*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap[i][j]
          = convert_sample<int16_t, 16>(d[j * channels + i]);
    }
  }
}


inline void read_s24(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  const auto frame_bytes = channels * 3;

  auto bytes = reinterpret_cast<uint8_t*>(data);
  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      int32_t sample = 0;
      sample += (bytes[3 * i] << 8);
      sample += (bytes[3 * i + 1] << 16);
      sample += (bytes[3 * i + 2] << 24);

      ap[i][j]
          = convert_sample<int32_t, 24>(sample);
    }
    bytes += frame_bytes;
  }
}

inline void read_s32(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<int32_t*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = convert_sample<int32_t, 32>(d[j * channels + i]);
    }
  }
}

inline void read_f32(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<float*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = d[j * channels + i];
    }
  }
}

inline void read_f64(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<double*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = d[j * channels + i];
    }
  }
}



struct sample_read_info {
  int64_t samples_to_read{};
  int64_t samples_to_write{};
};

inline auto sample_info(int64_t bufferSize, double durationRatio, const ossia::token_request& t)
{
  sample_read_info _;
  if (t.paused())
    return _;

  if(t.speed == 0.0)
    return _;

  _.samples_to_read = t.physical_read_duration(durationRatio);
  _.samples_to_write = std::min(t.physical_write_duration(durationRatio),
                                t.safe_physical_write_duration(durationRatio, bufferSize)
                                );

  return _;
}

inline void perform_upmix(const std::size_t upmix, const std::size_t chan, ossia::audio_port& ap)
{
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
}

inline void perform_start_offset(const std::size_t start, ossia::audio_port& ap)
{
  if (start != 0)
  {
    ap.samples.insert(ap.samples.begin(), start, ossia::audio_channel{});
  }
}

OSSIA_EXPORT
void do_fade(
    bool start_discontinuous, bool end_discontinuous,
    audio_channel& ap,
    std::size_t start, std::size_t end);

}

template<typename T>
struct at_end
{
  T func;
  at_end(T t): func{t} { }
  ~at_end() { func(); }
};

class sound_node : public ossia::nonowning_graph_node
{
public:
  void set_loop_info(ossia::time_value loop_duration, ossia::time_value start_offset, bool loops)
  {
    m_loop_duration = loop_duration;
    m_start_offset = start_offset;
    m_loops = loops;
  }
  virtual void reset_resampler(time_value date) = 0;

protected:
  time_value m_loop_duration{};
  time_value m_start_offset{};
  bool m_loops{};
};

class sound_process
    : public ossia::node_process
{
public:
  using ossia::node_process::node_process;
protected:
  void state(const ossia::token_request& req) override
  {
    // TODO here we should also pass the execution state so that we can
    // leverage the timing info & transform loop_duration / start_offset in samples right here...
    static_cast<sound_node&>(*this->node).set_loop_info(m_loop_duration, m_start_offset, m_loops);

    // Start offset and looping are done manually inside the sound nodes
    // since it is much more efficient in this case
    // (see fetch_audio)
    node->request(req);
  }

  void offset_impl(time_value date) override
  {
    static_cast<sound_node&>(*this->node).reset_resampler(date);
  }
  void transport_impl(time_value date) override
  {
    static_cast<sound_node&>(*this->node).reset_resampler(date);
  }
};

struct resampler
{
  void reset(time_value date, ossia::audio_stretch_mode mode, std::size_t channels, std::size_t fileSampleRate)
  {
    // TODO use the date parameter to buffer ! else transport won't work
    switch(mode)
    {
      case audio_stretch_mode::None:
      {
        if(auto s = std::get_if<ossia::raw_stretcher>(&m_stretch))
        {
          s->next_sample_to_read = date.impl;
        }
        else
        {
          m_stretch.emplace<raw_stretcher>(date.impl);
        }
        break;
      }
      case audio_stretch_mode::Repitch:
      {
        if(auto s = std::get_if<ossia::repitch_stretcher>(&m_stretch);
           s && s->repitchers.size() == channels)
        {
          s->next_sample_to_read = date.impl;
        }
        else
        {
          m_stretch.emplace<repitch_stretcher>(channels, 1024, date.impl);
        }
        break;
      }
      case audio_stretch_mode::RubberBandStandard:
      {
        m_stretch.emplace<rubberband_stretcher>(RubberBand::RubberBandStretcher::PresetOption::DefaultOptions, channels, fileSampleRate, date.impl);
        break;
      }
      case audio_stretch_mode::RubberBandPercussive:
      {
        m_stretch.emplace<rubberband_stretcher>(RubberBand::RubberBandStretcher::PresetOption::PercussiveOptions, channels, fileSampleRate, date.impl);
        break;
      }
    }
  }

  template<typename T>
  void run(
      T& audio_fetcher,
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      double tempo_ratio,
      std::size_t chan,
      std::size_t len,
      int64_t samples_to_read,
      int64_t samples_to_write,
      int64_t samples_offset,
      ossia::audio_port& ap)
  {
    switch(m_stretch.index())
    {
      case 0:
      {
        std::get_if<ossia::raw_stretcher>(&m_stretch)->run(audio_fetcher, t, e, tempo_ratio, chan, len, samples_to_read, samples_to_write, samples_offset, ap);
        return;
      }
      case 1:
      {
        std::get_if<ossia::rubberband_stretcher>(&m_stretch)->run(audio_fetcher, t, e, tempo_ratio, chan, len, samples_to_read, samples_to_write, samples_offset, ap);
        break;
      }
      case 2:
      {
        std::get_if<ossia::repitch_stretcher>(&m_stretch)->run(audio_fetcher, t, e, tempo_ratio, chan, len, samples_to_read, samples_to_write, samples_offset, ap);
        break;
      }
    }
  }

  std::variant<raw_stretcher, rubberband_stretcher, repitch_stretcher> m_stretch;
};
}
