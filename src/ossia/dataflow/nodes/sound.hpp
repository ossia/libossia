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

inline void read_s16(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
{
  const auto channels = ap.samples.size();
  auto d = reinterpret_cast<int16_t*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap.samples[i][j + t.offset.impl]
          = convert_sample<int16_t, 16>(d[j * channels + i]);
    }
  }
}


inline void read_s24(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
{
  const auto channels = ap.samples.size();
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

      ap.samples[i][j + t.offset.impl]
          = convert_sample<int32_t, 24>(sample);
    }
    bytes += frame_bytes;
  }
}

inline void read_s32(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
{
  const auto channels = ap.samples.size();
  auto d = reinterpret_cast<int32_t*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap.samples[i][j + t.offset.impl] = convert_sample<int32_t, 32>(d[j * channels + i]);
    }
  }
}

inline void read_f32(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
{
  const auto channels = ap.samples.size();
  auto d = reinterpret_cast<float*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap.samples[i][j + t.offset.impl] = d[j * channels + i];
    }
  }
}

inline void read_f64(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
{
  const auto channels = ap.samples.size();
  auto d = reinterpret_cast<double*>(data);

  for (int64_t j = 0; j < samples; j++)
  {
    for (std::size_t i = 0; i < channels; i++)
    {
      ap.samples[i][j + t.offset.impl] = d[j * channels + i];
    }
  }
}


inline auto sample_info(std::size_t len, int bufferSize, const ossia::token_request& t)
{
  struct {
    int64_t samples_to_read{};
    int64_t samples_to_write{};
  } _{};

  const int64_t max_N = std::min(t.date.impl, (int64_t)(len));
  if (max_N <= 0)
    return _;

  if(t.speed == 0.0)
    return _;

  _.samples_to_read = max_N - t.prev_date + t.offset.impl;

  if(t.speed < 1)
    _.samples_to_write = std::min(int64_t(lrint(ceil(_.samples_to_read / t.speed))), bufferSize - t.offset);
  else if(t.speed > 1)
    _.samples_to_write = std::max(int64_t(lrint(ceil(_.samples_to_read / t.speed))), bufferSize - t.offset);
  else
    _.samples_to_write = _.samples_to_read;

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

template<typename SoundNode_T>
class sound_process
    : public ossia::node_process
{
protected:
  void offset_impl(time_value date) override
  {
    static_cast<SoundNode_T&>(*this->node).reset_resampler(date);
  }
  void transport_impl(time_value date) override
  {
    static_cast<SoundNode_T&>(*this->node).reset_resampler(date);
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
        m_stretch.emplace<raw_stretcher>();
        break;
      }
      case audio_stretch_mode::Repitch:
      {
        m_stretch.emplace<repitch_stretcher>(channels, 1024);
        break;
      }
      case audio_stretch_mode::RubberBandStandard:
      {
        m_stretch.emplace<rubberband_stretcher>(RubberBand::RubberBandStretcher::PresetOption::DefaultOptions, channels, fileSampleRate);
        break;
      }
      case audio_stretch_mode::RubberBandPercussive:
      {
        m_stretch.emplace<rubberband_stretcher>(RubberBand::RubberBandStretcher::PresetOption::PercussiveOptions, channels, fileSampleRate);
        break;
      }
    }
  }

  void run(
      ossia::audio_span& data,
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      std::size_t chan,
      std::size_t len,
      int64_t samples_to_read,
      int64_t samples_to_write,
      ossia::audio_port& ap)
  {
    switch(m_stretch.index())
    {
      case 0:
      {
        std::get_if<ossia::raw_stretcher>(&m_stretch)->run(data, t, e, chan, len, samples_to_read, samples_to_write, ap);
        return;
      }
      case 1:
      {
        std::get_if<ossia::rubberband_stretcher>(&m_stretch)->run(data, t, e, chan, len, samples_to_read, samples_to_write, ap);
        break;
      }
      case 2:
      {
        std::get_if<ossia::repitch_stretcher>(&m_stretch)->run(data, t, e, chan, len, samples_to_read, samples_to_write, ap);
        break;
      }
    }
  }

  std::variant<raw_stretcher, rubberband_stretcher, repitch_stretcher> m_stretch;
};


}
//
// namespace ossia::nodes
// {
// class sound final : public ossia::nonowning_graph_node
// {
// public:
//   sound()
//   {
//     m_outlets.push_back(&audio_out);
//   }
//
//   ~sound()
//   {
//   }
//
//   void set_start(std::size_t v)
//   {
//     start = v;
//   }
//   void set_start_offset(std::size_t v)
//   {
//     start_offset = v;
//   }
//   void set_upmix(std::size_t v)
//   {
//     upmix = v;
//   }
//   void set_sound(const std::vector<ossia::float_vector>& vec)
//   {
//     m_data.resize(vec.size());
//     for (std::size_t i = 0; i < vec.size(); i++)
//     {
//       m_data[i].assign(vec[i].begin(), vec[i].end());
//     }
//   }
//   void set_sound(std::vector<ossia::double_vector> vec)
//   {
//     m_data = std::move(vec);
//   }
//   void
//   run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
//   {
//     if (m_data.empty())
//       return;
//     const std::size_t chan = m_data.size();
//     const std::size_t len = m_data[0].size();
//
//     ossia::audio_port& ap = *audio_out.data.target<ossia::audio_port>();
//     ap.samples.resize(chan);
//     int64_t max_N = std::min(t.date.impl, (int64_t)(len - start_offset));
//     if (max_N <= 0)
//       return;
//     const auto samples = max_N - t.prev_date + t.offset.impl;
//     if (samples <= 0)
//       return;
//
//     if (t.date > t.prev_date)
//     {
//       for (std::size_t i = 0; i < chan; i++)
//       {
//         ap.samples[i].resize(samples);
//         for (int64_t j = t.prev_date; j < max_N; j++)
//         {
//           ap.samples[i][j - t.prev_date + t.offset.impl]
//               = m_data[i][j + start_offset];
//         }
//         do_fade(
//             t.start_discontinuous, t.end_discontinuous, ap.samples[i],
//             t.offset.impl, samples);
//       }
//     }
//     else
//     {
//       // TODO rewind correctly and add rubberband
//       for (std::size_t i = 0; i < chan; i++)
//       {
//         ap.samples[i].resize(samples);
//         for (int64_t j = t.prev_date; j < max_N; j++)
//         {
//           ap.samples[i][max_N - (j - t.prev_date) + t.offset.impl]
//               = m_data[i][j];
//         }
//
//         do_fade(
//             t.start_discontinuous, t.end_discontinuous, ap.samples[i],
//             max_N + t.offset.impl, t.prev_date + t.offset.impl);
//       }
//     }
//
//     // Upmix
//     if (upmix != 0)
//     {
//       if (upmix < chan)
//       {
//         /* //TODO
//     // Downmix
//     switch(upmix)
//     {
//       case 1:
//       {
//         for(std::size_t i = 1; i < chan; i++)
//         {
//           if(ap.samples[0].size() < ap.samples[i].size())
//             ap.samples[0].resize(ap.samples[i].size());
//
//           for(std::size_t j = 0; j < ap.samples[i].size(); j++)
//             ap.samples[0][j] += ap.samples[i][j];
//         }
//       }
//       default:
//         // TODO
//         break;
//     }
//     */
//       }
//       else if (upmix > chan)
//       {
//         switch (chan)
//         {
//           case 1:
//           {
//             ap.samples.resize(upmix);
//             for (std::size_t i = 1; i < upmix; i++)
//             {
//               ap.samples[i] = ap.samples[0];
//             }
//             break;
//           }
//           default:
//             // TODO
//             break;
//         }
//       }
//     }
//
//     // Move channels
//     if (start != 0)
//     {
//       ap.samples.insert(ap.samples.begin(), start, ossia::audio_channel{});
//     }
//   }
//   std::size_t channels() const
//   {
//     return m_data.size();
//   }
//   std::size_t duration() const
//   {
//     return m_data.empty() ? 0 : m_data[0].size();
//   }
//
// private:
//   std::vector<ossia::double_vector> m_data;
//   std::size_t start{};
//   std::size_t start_offset{};
//   std::size_t upmix{};
//   ossia::outlet audio_out{ossia::audio_port{}};
// };
// }
//
