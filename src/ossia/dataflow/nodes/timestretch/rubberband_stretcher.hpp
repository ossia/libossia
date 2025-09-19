#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_RUBBERBAND)
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/token_request.hpp>

#if __has_include(<RubberBandStretcher.h>)
#include <RubberBandStretcher.h>
#elif __has_include(<rubberband/RubberBandStretcher.h>)
#include <rubberband/RubberBandStretcher.h>
#endif

namespace ossia
{
static constexpr auto get_rubberband_preset(ossia::audio_stretch_mode mode)
{
  using opt_t = RubberBand::RubberBandStretcher::Option;
  using preset_t = RubberBand::RubberBandStretcher::PresetOption;
  uint32_t preset = opt_t::OptionProcessRealTime | opt_t::OptionThreadingNever;
  switch(mode)
  {
    case ossia::audio_stretch_mode::RubberBandStandard:
      break;

    case ossia::audio_stretch_mode::RubberBandPercussive:
      preset |= preset_t::PercussiveOptions;
      break;

    case ossia::audio_stretch_mode::RubberBandStandardHQ:
      preset |= RubberBand::RubberBandStretcher::OptionEngineFiner;
      preset |= RubberBand::RubberBandStretcher::OptionPitchHighConsistency;
      break;

    case ossia::audio_stretch_mode::RubberBandPercussiveHQ:
      preset |= preset_t::PercussiveOptions;
      preset |= RubberBand::RubberBandStretcher::OptionEngineFiner;
      preset |= RubberBand::RubberBandStretcher::OptionPitchHighConsistency;
      break;

    default:
      break;
  }

  return preset;
}

struct rubberband_stretcher
{
  rubberband_stretcher(
      uint32_t opt, std::size_t channels, std::size_t sampleRate, int64_t pos)
      : m_rubberBand{std::make_unique<RubberBand::RubberBandStretcher>(
          sampleRate, channels, opt)}
      , next_sample_to_read{pos}
      , options{opt}

  {
  }

  rubberband_stretcher(const rubberband_stretcher&) = delete;
  rubberband_stretcher& operator=(const rubberband_stretcher&) = delete;
  rubberband_stretcher(rubberband_stretcher&&) = default;
  rubberband_stretcher& operator=(rubberband_stretcher&&) = default;

  std::unique_ptr<RubberBand::RubberBandStretcher> m_rubberBand;
  int64_t next_sample_to_read = 0;
  uint32_t options{};

  void transport(int64_t date)
  {
    m_rubberBand->reset();
    next_sample_to_read = date;
  }

  template <typename T>
  void
  run(T& audio_fetcher, const ossia::token_request& t, ossia::exec_state_facade e,
      double tempo_ratio, const std::size_t chan, const std::size_t len,
      int64_t samples_to_read, const int64_t samples_to_write,
      const int64_t samples_offset, const ossia::mutable_audio_span<double>& ap) noexcept
  {
    if(tempo_ratio != m_rubberBand->getTimeRatio())
    {
      m_rubberBand->setTimeRatio(tempo_ratio);
    }

    if(t.forward())
    {
      // TODO : if T::sample_type == float we could leverage it directly as
      // input
      const int max_chan = std::max(chan, m_rubberBand->getChannelCount());
      const int frames = std::max((int64_t)16, samples_to_read);
      float** const input = (float**)alloca(sizeof(float*) * max_chan);
      float** const output = (float**)alloca(sizeof(float*) * max_chan);
      for(std::size_t i = 0; i < chan; i++)
      {
        input[i] = (float*)alloca(sizeof(float) * frames);
        output[i] = (float*)alloca(sizeof(float) * samples_to_write);
      }
      for(std::size_t i = chan; i < m_rubberBand->getChannelCount(); i++)
      {
        input[i] = (float*)alloca(sizeof(float) * frames);
        std::fill_n(input[i], frames, 0.f);
        output[i] = (float*)alloca(sizeof(float) * samples_to_write);
      }

      while(m_rubberBand->available() < samples_to_write)
      {
        audio_fetcher.fetch_audio(next_sample_to_read, samples_to_read, input);

        m_rubberBand->process(input, samples_to_read, false);

        next_sample_to_read += samples_to_read;
        samples_to_read = 16;
      }

      m_rubberBand->retrieve(
          output, std::min((int)samples_to_write, m_rubberBand->available()));

      for(std::size_t i = 0; i < chan; i++)
      {
        for(int64_t j = 0; j < samples_to_write; j++)
        {
          ap[i][j + samples_offset] = double(output[i][j]);
        }
      }
    }
    else
    {
      // TODO
    }
  }
};
}
#else
#include <ossia/dataflow/nodes/timestretch/raw_stretcher.hpp>

namespace ossia
{
static constexpr uint32_t get_rubberband_preset(ossia::audio_stretch_mode mode)
{
  return 0;
}
using rubberband_stretcher = raw_stretcher;
}
#endif
