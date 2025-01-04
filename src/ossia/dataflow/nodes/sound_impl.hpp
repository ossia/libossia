#pragma once
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/dataflow/nodes/timestretch/raw_stretcher.hpp>
#include <ossia/dataflow/nodes/timestretch/repitch_stretcher.hpp>
#include <ossia/dataflow/nodes/timestretch/rubberband_stretcher.hpp>
#include <ossia/detail/variant.hpp>

namespace ossia
{
struct resampler
{
  void reset(
      time_value date, ossia::audio_stretch_mode mode, std::size_t channels,
      std::size_t fileSampleRate)
  {
    // TODO use the date parameter to buffer ! else transport won't work
    switch(mode)
    {
      case audio_stretch_mode::None: {
        if(auto s = ossia::get_if<ossia::raw_stretcher>(&m_stretch))
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
      case audio_stretch_mode::RepitchMediumQ:
      case audio_stretch_mode::RepitchFastestQ: {
        if(auto s = ossia::get_if<ossia::repitch_stretcher>(&m_stretch);
           s && s->repitchers.size() == channels)
        {
          s->next_sample_to_read = date.impl;
        }
        else
        {
          m_stretch.emplace<repitch_stretcher>(
              get_samplerate_preset(mode), channels, 1024, date.impl);
        }
        break;
      }
      case audio_stretch_mode::RubberBandStandard:
      case audio_stretch_mode::RubberBandPercussive:
      case ossia::audio_stretch_mode::RubberBandStandardHQ:
      case ossia::audio_stretch_mode::RubberBandPercussiveHQ: {
        m_stretch.emplace<rubberband_stretcher>(
            get_rubberband_preset(mode), channels, fileSampleRate, date.impl);
        break;
      }
    }
  }

  template <typename T>
  void
  run(T& audio_fetcher, const ossia::token_request& t, ossia::exec_state_facade e,
      double tempo_ratio, std::size_t chan, std::size_t len, int64_t samples_to_read,
      int64_t samples_to_write, int64_t samples_offset, ossia::audio_port& ap)
  {
    ossia::visit([&](auto& stretcher) {
      stretcher.run(
          audio_fetcher, t, e, tempo_ratio, chan, len, samples_to_read, samples_to_write,
          samples_offset, ap);
    });
  }

  ossia::variant<raw_stretcher, rubberband_stretcher, repitch_stretcher> m_stretch;
};

}
