#pragma once
#include <ossia/dataflow/sample_to_float.hpp>
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
          for(std::size_t chan = 1; chan < upmix; ++chan)
            ap.samples[chan].assign(ap.samples[0].begin(), ap.samples[0].end());
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

struct resampler
{
  enum {
    RawStretcher = 0, RubberbandStretcher = 1, RepitchStretcher = 2
  };
  int64_t next_sample_to_read() const noexcept
  {
    switch(m_stretch.index())
    {
      default:
      case RawStretcher:
      {
        auto& s = *std::get_if<RawStretcher>(&m_stretch);
        return s.next_sample_to_read;
      }
#if __has_include(<RubberBandStretcher.h>)
      case RubberbandStretcher:
      {
        auto& s = *std::get_if<RubberbandStretcher>(&m_stretch);
        return s.next_sample_to_read;
      }
#endif

#if __has_include(<samplerate.h>)
      case RepitchStretcher:
      {
        auto& s = *std::get_if<RepitchStretcher>(&m_stretch);
        return s.next_sample_to_read;
      }
#endif
    }
    return 0;
  }

  void transport(int64_t date)
  {
    switch(m_stretch.index())
    {
      default:
      case RawStretcher:
      {
        auto& s = *std::get_if<RawStretcher>(&m_stretch);
        s.next_sample_to_read = date;
        break;
      }
#if __has_include(<RubberBandStretcher.h>)
      case RubberbandStretcher:
      {
        auto& s = *std::get_if<RubberbandStretcher>(&m_stretch);
        s.m_rubberBand->reset();
        s.next_sample_to_read = date;
        break;
      }
#endif
#if __has_include(<samplerate.h>)
      case RepitchStretcher:
      {
        auto& s = *std::get_if<RepitchStretcher>(&m_stretch);
        s.next_sample_to_read = date;
        break;
      }
#endif
    }
  }

  void reset(int64_t date, ossia::audio_stretch_mode mode, std::size_t channels, std::size_t fileSampleRate)
  {
    // TODO use the date parameter to buffer ! else transport won't work
    switch(mode)
    {
      default:
      case ossia::audio_stretch_mode::None:
      {
        if(auto s = std::get_if<RawStretcher>(&m_stretch))
        {
          s->next_sample_to_read = date;
        }
        else
        {
          m_stretch.emplace<RawStretcher>(date);
        }
        break;
      }

#if __has_include(<RubberBandStretcher.h>)
      case ossia::audio_stretch_mode::RubberBandStandard:
      case ossia::audio_stretch_mode::RubberBandPercussive:
      {
        using preset_t = RubberBand::RubberBandStretcher::PresetOption;
        const auto preset = mode == audio_stretch_mode::RubberBandStandard
            ? preset_t::DefaultOptions
            : preset_t::PercussiveOptions;

        if(auto s = std::get_if<RubberbandStretcher>(&m_stretch); s && s->options == preset)
        {
          s->m_rubberBand->reset();
          s->next_sample_to_read = date;
        }
        else
        {
          m_stretch.emplace<rubberband_stretcher>(preset, channels, fileSampleRate, date);
        }
        break;
      }
#endif

#if __has_include(<samplerate.h>)
      case ossia::audio_stretch_mode::Repitch:
      {
        if(auto s = std::get_if<RepitchStretcher>(&m_stretch);
           s && s->repitchers.size() == channels)
        {
          s->next_sample_to_read = date;
        }
        else
        {
          // FIXME why 1024 here ?!
          m_stretch.emplace<repitch_stretcher>(channels, 1024, date);
        }
        break;
      }
#endif
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
      default:
      case RawStretcher:
      {
        std::get_if<RawStretcher>(&m_stretch)->run(audio_fetcher, t, e, tempo_ratio, chan, len, samples_to_read, samples_to_write, samples_offset, ap);
        return;
      }

#if __has_include(<RubberBandStretcher.h>)
      case RubberbandStretcher:
      {
        std::get_if<RubberbandStretcher>(&m_stretch)->run(audio_fetcher, t, e, tempo_ratio, chan, len, samples_to_read, samples_to_write, samples_offset, ap);
        break;
      }
#endif

#if __has_include(<samplerate.h>)
      case RepitchStretcher:
      {
        std::get_if<RepitchStretcher>(&m_stretch)->run(audio_fetcher, t, e, tempo_ratio, chan, len, samples_to_read, samples_to_write, samples_offset, ap);
        break;
      }
#endif
    }
  }

  bool stretch() const noexcept { return m_stretch.index() != 0; }

  std::variant<raw_stretcher, rubberband_stretcher, repitch_stretcher> m_stretch;
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

  void set_resampler(ossia::resampler&& r)
  {
    auto date = m_resampler.next_sample_to_read();
    m_resampler = std::move(r);
    m_resampler.transport(date);
  }

  void set_native_tempo(double v)
  {
    tempo = v;
  }

  virtual void transport(time_value date) = 0;

protected:
  double update_stretch(const ossia::token_request& t, const ossia::exec_state_facade& e) noexcept
  {
    double stretch_ratio = 1.;
    double model_ratio = 1.;
    if(tempo != 0.) {
      switch(m_resampler.m_stretch.index())
      {
        case 0:
          model_ratio = ossia::root_tempo / t.tempo;
        break;
        default:
          model_ratio = ossia::root_tempo / this->tempo;
          stretch_ratio = this->tempo / t.tempo;
          break;
      }
    }

    m_loop_duration_samples = m_loop_duration.impl * e.modelToSamples() * model_ratio;
    m_start_offset_samples = m_start_offset.impl * e.modelToSamples() * model_ratio;
    return stretch_ratio;
  }

  time_value m_prev_date{time_value::infinite_min};

  time_value m_loop_duration{};
  time_value m_start_offset{};

  double tempo{};

  int64_t m_loop_duration_samples{};
  int64_t m_start_offset_samples{};

  ossia::resampler m_resampler{};

  bool m_loops{};
};

class dummy_sound_node final : public sound_node
{
public:
  ossia::audio_outlet audio_out;
  dummy_sound_node()
  {
    // Add a dummy outlet so that interval can connect propagation to it
    m_outlets.push_back(&audio_out);
  }

  void transport(time_value date) override
  {
  }

  void
  run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
  }
};

class sound_process final
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
    static_cast<sound_node&>(*this->node).transport(date);
  }
  void transport_impl(time_value date) override
  {
    static_cast<sound_node&>(*this->node).transport(date);
  }
};

}
