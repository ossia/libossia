#pragma once
#include <ossia/detail/flicks.hpp>
#include <ossia/detail/math.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/editor/scenario/time_signature.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <cassert>
#include <cmath>
#include <optional>

#if defined(_LIBCPP_CONSTEXPR_SINCE_CXX23) || defined(_GLIBCXX23_CONSTEXPR)
#define ossia_constexpr_msvc_workaround constexpr
#else
#define ossia_constexpr_msvc_workaround
#endif
namespace ossia
{
using quarter_note = double;

//! One quantification point inside a tick: when it happens, and which
//! subdivision it is.
struct quantification_point
{
  ossia::time_value date{};
  int64_t index{};

  friend bool
  operator==(const quantification_point&, const quantification_point&) noexcept
      = default;
};
using quantification_points = ossia::small_vector<quantification_point, 8>;

struct token_request
{
  constexpr token_request() noexcept = default;
  constexpr token_request(const token_request&) noexcept = default;
  constexpr token_request(token_request&&) noexcept = default;
  constexpr token_request& operator=(const token_request&) noexcept = default;
  constexpr token_request& operator=(token_request&&) noexcept = default;

  constexpr token_request(
      ossia::time_value prev_d, ossia::time_value d, ossia::time_value parent_duration,
      ossia::time_value off, double s, time_signature sig, double tempo) noexcept
      : prev_date{prev_d}
      , date{d}
      , parent_duration{parent_duration}
      , offset{off}
      , speed{s}
      , tempo{tempo}
      , signature{sig}
  {
    if(offset.impl < 0)
    {
      offset.impl = 0;
    }
  }

  [[nodiscard]] constexpr token_request add_offset(ossia::time_value t) const noexcept
  {
    token_request other = *this;
    other.prev_date += t;
    other.date += t;
    return other;
  }

  template <typename Exec, typename Transport>
  constexpr void loop(
      ossia::time_value start_offset, ossia::time_value loop_duration, Exec f,
      Transport transport) const noexcept
  {
    if(loop_duration.impl <= 0)
    {
      token_request other = add_offset(start_offset);
      f(other);
      return;
    }

    ossia::token_request other = *this;
    ossia::time_value orig_from = other.prev_date;
    ossia::time_value tick_amount = other.date - other.prev_date;

    if(tick_amount >= 0_tv)
    {
      // Forward playback
      while(tick_amount > 0_tv)
      {
        const time_value cur_from{orig_from % loop_duration};
        if(cur_from + tick_amount < loop_duration)
        {
          other.prev_date = cur_from + start_offset;
          other.date = other.prev_date + tick_amount;
          f(other);
          break;
        }
        else
        {
          auto this_tick = loop_duration - cur_from;

          tick_amount -= this_tick;
          orig_from += this_tick;
          other.prev_date = cur_from + start_offset;
          other.date = other.prev_date + this_tick;

          f(other);

          transport(start_offset);
          other.offset += this_tick;
        }
      }
    }
    else
    {
      // Backward playback
      const int64_t loop_dur = loop_duration.impl;
      int64_t remaining = -tick_amount.impl;

      while(remaining > 0)
      {
        int64_t cur_from = orig_from.impl % loop_dur;
        if(cur_from < 0)
          cur_from += loop_dur;

        if(cur_from == 0)
        {
          transport(start_offset + loop_duration);
          cur_from = loop_dur;
        }

        const int64_t this_tick = remaining < cur_from ? remaining : cur_from;

        other.prev_date = time_value{cur_from} + start_offset;
        other.date = other.prev_date - time_value{this_tick};

        f(other);

        remaining -= this_tick;
        orig_from -= time_value{this_tick};
        other.offset += time_value{this_tick};
      }
    }
  }

  //! How much we read from our data model
  [[nodiscard]] constexpr time_value model_read_duration() const noexcept
  {
    return date - prev_date;
  }

  [[nodiscard]] static constexpr double abs_speed(double s) noexcept
  {
    return s < 0. ? -s : s;
  }

  //! The date of the first sample in the context of the parent.
  //! e.g. if we're at the start of our third buffer of 256 samples for
  //! a given time_interval, this will give 768.
  [[nodiscard]] constexpr physical_time
  start_date_to_physical(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    assert(speed != 0.);
    return this->prev_date.impl * ratio / abs_speed(speed);
  }

  //! The sample a position in this tick's model time maps to, measured from
  //! the start of the buffer. Non-decreasing, so spans taken as differences of
  //! it can neither overlap nor leave a hole: the end of one is the start of
  //! the next by construction, not by agreement.
  [[nodiscard]] constexpr physical_time
  sample_at(ossia::time_value tick_position, double ratio) const noexcept
  {
    assert(speed != 0.);
    return constexpr_floor(tick_position.impl * ratio / abs_speed(speed));
  }

  //! Where we must start to read / write in our physical buffers
  [[nodiscard]] constexpr physical_time physical_start(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    return sample_at(this->offset, ratio);
  }

  //! Given a sound file at 44100 and a system rate at 44100,
  //! this is the amount of samples that we must process from the sound file
  //! during this tick
  [[nodiscard]] constexpr physical_time
  physical_read_duration(double ratio) const noexcept
  {
    // A difference of one map over absolute model time, so consecutive ticks
    // read consecutive samples with neither a gap nor an overlap.
    const auto a = prev_date.impl < date.impl ? prev_date.impl : date.impl;
    const auto b = prev_date.impl < date.impl ? date.impl : prev_date.impl;
    return constexpr_floor(b * ratio) - constexpr_floor(a * ratio);
  }

  //! Given a sound file at 44100 and a system rate at 44100,
  //! this is the amount of samples that we must write in the audio buffer
  [[nodiscard]] constexpr physical_time
  physical_write_duration(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    return sample_at(this->offset + abs(date - prev_date), ratio)
           - sample_at(this->offset, ratio);
  }

  //! This is an upper bound on what we can write to a buffer.
  [[nodiscard]] constexpr physical_time
  safe_physical_write_duration(double ratio, int bufferSize) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    return bufferSize - sample_at(this->offset, ratio);
  }

  //! Is the given value in the tick defined by this token_request
  [[nodiscard]] constexpr bool in_range(ossia::time_value global_time) const noexcept
  {
    return global_time.impl >= prev_date.impl && global_time.impl < date.impl;
  }

  //! Maps a time value in the frame of reference of this tick's node to a time
  //! value inside its physical buffers
  [[nodiscard]] constexpr physical_time
  to_physical_time_in_tick(ossia::time_value global_time, double ratio) const noexcept
  {
    assert(speed != 0.);
    // How far into the tick this date sits, counted forwards in both
    // directions, then through the same map the span endpoints use.
    const int64_t in_tick
        = speed < 0. ? (prev_date - global_time).impl : (global_time - prev_date).impl;
    return sample_at(this->offset + ossia::time_value{in_tick}, ratio);
  }

  //! Maps a time value in the frame of reference of this tick's node to a time
  //! value inside its physical buffers
  [[nodiscard]] constexpr physical_time
  to_physical_time_in_tick(int64_t global_time, double ratio) const noexcept
  {
    return to_physical_time_in_tick(ossia::time_value{global_time}, ratio);
  }

  //! Maps a time value in the frame of reference of the physical buffers, to a
  //! model time
  [[nodiscard]] constexpr time_value
  from_physical_time_in_tick(ossia::physical_time phys_time, double ratio) const noexcept
  {
    assert(speed != 0.);
    const double in_tick = phys_time - physical_start(ratio);
    return time_value{constexpr_floor(in_tick * (speed / ratio) + prev_date.impl)};
  }

  //! If we are in a kind of hierarchical object, return where we are at the
  //! end of this tick.
  [[nodiscard]] constexpr double position() const noexcept
  {
    return parent_duration.impl > 0 ? date.impl / double(parent_duration.impl) : 0.;
  }

  //! Does the tick go forward
  [[nodiscard]] constexpr bool forward() const noexcept { return date > prev_date; }

  //! Is the tick not advancing
  [[nodiscard]] constexpr bool paused() const noexcept { return date == prev_date; }

  //! Does the tick go backward (e.g. speed < 0)
  [[nodiscard]] constexpr bool backward() const noexcept { return date < prev_date; }

  [[nodiscard]] ossia_constexpr_msvc_workaround std::optional<time_value>
  get_quantification_date_for_bars_or_longer(double rate) const noexcept
  {
    std::optional<time_value> quantification_date;
    const double bars_per_quantization = 1.0 / rate;

    // Convert positions to bar numbers from the last signature
    const double start_bar_position
        = (musical_start_position - musical_start_last_signature)
          / (4.0 * signature.upper / signature.lower);
    const double end_bar_position = (musical_end_position - musical_start_last_signature)
                                    / (4.0 * signature.upper / signature.lower);

    // Check if we're exactly on a quantization point at the start
    const double start_remainder = std::fmod(start_bar_position, bars_per_quantization);
    if(std::abs(start_remainder) < 0.0001 && musical_start_position >= 0)
    {
      quantification_date = prev_date;
    }
    else
    {
      // Find the next quantization bar after start
      const double start_quant_bar
          = std::floor(start_bar_position / bars_per_quantization);
      const double next_quant_bar_number = (start_quant_bar + 1) * bars_per_quantization;

      // Check if this quantization point falls within our tick (but NOT at the end)
      if(next_quant_bar_number > start_bar_position
         && next_quant_bar_number < end_bar_position)
      {
        // Calculate the musical position of this quantization point
        const double quant_musical_position
            = musical_start_last_signature
              + next_quant_bar_number * (4.0 * signature.upper / signature.lower);

        // Map this to a time value
        const double musical_tick_duration
            = musical_end_position - musical_start_position;
        const double ratio
            = (quant_musical_position - musical_start_position) / musical_tick_duration;
        const time_value dt = date - prev_date;

        time_value potential_date = prev_date + dt * ratio;

        // Extra safety check: ensure we're not at the boundary
        if(potential_date < date)
        {
          quantification_date = potential_date;
        }
        else
        {
          return std::nullopt;
        }
      }
    }
    return quantification_date;
  }

  [[nodiscard]] ossia_constexpr_msvc_workaround std::optional<time_value>
  get_quantification_date_for_shorter_than_bars(double rate) const noexcept
  {
    // Quantize relative to quarter divisions
    // TODO ! if there is a bar change,
    // and no prior quantization date before that, we have to quantize to the
    // bar change
    const double start_quarter = (musical_start_position - musical_start_last_bar);
    const double end_quarter = (musical_end_position - musical_start_last_bar);

    // duration of what we quantify in terms of quarters
    const double musical_quant_dur = rate / 4.;
    const double start_quant = std::floor(start_quarter * musical_quant_dur);
    const double end_quant = std::floor(end_quarter * musical_quant_dur);

    if(start_quant != end_quant)
    {
      if(end_quant == end_quarter * musical_quant_dur)
      {
        // We want quantization on start, not on end
        return std::nullopt;
      }
      // Date to quantify is the next one :
      const double musical_tick_duration = musical_end_position - musical_start_position;
      const double quantified_duration
          = (musical_start_last_bar + (start_quant + 1) * 4. / rate)
            - musical_start_position;
      const double ratio = (date - prev_date).impl / musical_tick_duration;

      return prev_date + quantified_duration * ratio;
    }
    else if(start_quant == start_quarter * musical_quant_dur)
    {
      // We start on a signature change
      return prev_date;
    }
    else
    {
      return std::nullopt;
    }
  }

  //! Given a quantification rate (1 for bars, 2 for half, 4 for quarters...)
  //! return the next occurring quantification date, if such date is in the tick
  //! defined by this token_request.
  [[nodiscard]] ossia_constexpr_msvc_workaround std::optional<time_value>
  get_quantification_date(double rate) const noexcept
  {
    if(prev_date == date)
      return std::nullopt;

    if(backward())
      return std::nullopt;

    if(rate <= 0.)
      return prev_date;

    const double musical_tick_duration = musical_end_position - musical_start_position;
    if(musical_tick_duration <= 0.)
      return prev_date;

    if(rate <= 1.)
    {
      return get_quantification_date_for_bars_or_longer(rate);
    }
    else
    {
      return get_quantification_date_for_shorter_than_bars(rate);
    }
  }

  //! Every quantification date occurring in this tick, in order.
  //!
  //! get_quantification_date only reports the first one, which silently drops
  //! steps whenever a tick spans more than one - a small division, a large
  //! buffer or a fast tempo are enough. Nodes that must not miss a step
  //! iterate this instead. `index` counts the point from the last bar, or from
  //! the last signature change for rates of a bar or longer.
  [[nodiscard]] quantification_points
  get_quantification_dates(double rate) const noexcept
  {
    quantification_points res;

    if(prev_date == date)
      return res;

    const double musical_tick_duration = musical_end_position - musical_start_position;
    const bool rewinding = date < prev_date;

    // A musical duration that disagrees with the direction of the tick did not
    // come from it - subdividing it would place every point at prev_date.
    if(rate <= 0. || musical_tick_duration == 0.
       || (musical_tick_duration < 0.) != rewinding)
    {
      res.push_back({prev_date, 0});
      return res;
    }

    // A point falling exactly on the end of the tick belongs to the next one,
    // so the interval is [start; end[ - which is also what makes the first
    // element agree with get_quantification_date().
    constexpr double eps = 1e-9;
    const time_value tick_duration = date - prev_date;
    const bool valid_sig = signature.upper > 0 && signature.lower > 0;
    const double quarters_in_bar = valid_sig ? 4. * signature.upper / signature.lower : 4.;

    // A point is kept if it lands inside the tick; false means we walked past
    // the end and can stop.
    const auto try_push = [&](double musical_position, int64_t index) {
      const double ratio
          = (musical_position - musical_start_position) / musical_tick_duration;
      time_value d = prev_date + tick_duration * ratio;

      if(rewinding)
      {
        if(d > prev_date)
          d = prev_date;
        if(d <= date)
          return false;
      }
      else
      {
        if(d < prev_date)
          d = prev_date;
        if(d >= date)
          return false;
      }

      res.push_back({d, index});
      // A tick spanning this many points means the rate is nonsense: stop
      // rather than fill memory.
      return res.size() < 1024;
    };

    if(rate <= 1.)
    {
      // A bar or longer: the rate is a fraction of a bar, counted from the last
      // signature change, and no bar line subdivides it.
      const double unit = quarters_in_bar / rate;
      if(!(unit > 0.))
        return res;

      const double origin = musical_start_last_signature;
      const double start = (musical_start_position - origin) / unit;
      const double end = (musical_end_position - origin) / unit;
      const int64_t first = rewinding ? int64_t(std::floor(start + eps))
                                      : int64_t(std::ceil(start - eps));

      for(int64_t k = first; rewinding ? (k > end + eps) : (k < end - eps);
          k += rewinding ? -1 : 1)
      {
        if(!try_push(k * unit + origin, k))
          break;
      }
      return res;
    }

    // Shorter than a bar: a subdivision of the quarter note, counted from the
    // bar it falls in. The grid restarts at every bar line, so a bar whose
    // length is not a whole number of divisions (7/8 against a half-note grid)
    // does not carry a stale phase into the next one, and the bar line itself
    // is always a point.
    const double unit = 4. / rate;
    if(!(unit > 0.) || !(quarters_in_bar > 0.))
      return res;

    const double lo = rewinding ? musical_end_position : musical_start_position;
    const double hi = rewinding ? musical_start_position : musical_end_position;
    const double bar0 = rewinding ? musical_end_last_bar : musical_start_last_bar;

    int n_bars = int((hi - bar0) / quarters_in_bar) + 1;
    if(n_bars > 1024)
      n_bars = 1024;
    const int divs = int(std::ceil(quarters_in_bar / unit));

    const auto walk_bar = [&](double bar_line) {
      if(!rewinding)
      {
        for(int64_t k = 0; k <= divs; k++)
        {
          const double p = bar_line + k * unit;
          if(p >= bar_line + quarters_in_bar - eps || p > hi + eps)
            return true;
          if(p < lo - eps)
            continue;
          if(!try_push(p, k))
            return false;
        }
      }
      else
      {
        for(int64_t k = divs; k >= 0; k--)
        {
          const double p = bar_line + k * unit;
          if(p >= bar_line + quarters_in_bar - eps || p > hi + eps)
            continue;
          if(p < lo - eps)
            return true;
          if(!try_push(p, k))
            return false;
        }
      }
      return true;
    };

    if(!rewinding)
    {
      for(int b = 0; b <= n_bars; b++)
      {
        const double bar_line = bar0 + b * quarters_in_bar;
        if(bar_line > hi + eps)
          break;
        if(!walk_bar(bar_line))
          break;
      }
    }
    else
    {
      for(int b = n_bars; b >= 0; b--)
      {
        if(!walk_bar(bar0 + b * quarters_in_bar))
          break;
      }
    }
    return res;
  }

  //! Like physical_quantification_date, but returns a date mapped to this tick
  [[nodiscard]] ossia_constexpr_msvc_workaround std::optional<physical_time>
  get_physical_quantification_date(double rate, double modelToSamples) const noexcept
  {
    if(auto d = get_quantification_date(rate))
      return to_physical_time_in_tick(*d, modelToSamples);
    return {};
  }

  template <typename Tick, typename Tock>
  constexpr void
  metronome(double modelToSamplesRatio, Tick tick, Tock tock) const noexcept
  {
    const double musical_tick_duration = musical_end_position - musical_start_position;
    const bool rewinding = backward();

    // A musical duration that disagrees with the direction of the tick did not
    // come from it, and interpolating in it would land outside the buffer.
    if(musical_tick_duration == 0. || (musical_tick_duration < 0.) != rewinding)
      return;

    // A tick shorter than a sample covers no whole sample, but the grid point
    // in it still belongs to one: the sample its span starts on, which is
    // offset 0. Placement depends on the length, emission must not.
    const int64_t samples_tick_duration = physical_write_duration(modelToSamplesRatio);
    if(samples_tick_duration < 0)
      return;

    // Where a musical date the tick crosses falls in the samples it covers. The
    // ratio is positive in both directions: rewinding, both the distance to the
    // date and the tick duration are negative. A date sitting exactly on the end
    // of the tick would give the one-past-the-end sample.
    const auto sample_of = [&](double musical_position) {
      if(samples_tick_duration == 0)
        return int64_t(0);
      const double ratio
          = (musical_position - musical_start_position) / musical_tick_duration;
      const int64_t s = samples_tick_duration * ratio;
      return s < 0 ? int64_t(0)
                   : (s >= samples_tick_duration ? samples_tick_duration - 1 : s);
    };

    const double quarters_in_bar = 4. * signature.upper / signature.lower;
    if(!(quarters_in_bar > 0.))
      return;

    // Walk every grid point the tick covers rather than only the last one: in
    // 7/8 the third beat and the following bar line are half a quarter apart,
    // so a tick of a moderate length steps over both.
    const double lo = rewinding ? musical_end_position : musical_start_position;
    const double hi = rewinding ? musical_start_position : musical_end_position;
    const double bar0 = rewinding ? musical_end_last_bar : musical_start_last_bar;

    // A grid point sitting exactly on a tick boundary belongs to the tick that
    // starts on it, where it is sample 0, not to the one that ends on it, where
    // it would be the last sample and so a sample early. Half-open at the end
    // the tick is heading towards, in both directions.
    const auto emit = [&](double p, bool is_bar) {
      if(rewinding ? (p > hi || p <= lo) : (p < lo || p >= hi))
        return;
      if(is_bar)
        tick(sample_of(p));
      else
        tock(sample_of(p));
    };

    int n_bars = int((hi - bar0) / quarters_in_bar) + 1;
    if(n_bars > 1024)
      n_bars = 1024;

    if(!rewinding)
    {
      for(int b = 0; b <= n_bars; b++)
      {
        const double bar_line = bar0 + b * quarters_in_bar;
        if(bar_line > hi)
          break;
        emit(bar_line, true);
        for(int q = 1; q < quarters_in_bar; q++)
          emit(bar_line + q, false);
      }
    }
    else
    {
      for(int b = n_bars; b >= 0; b--)
      {
        const double bar_line = bar0 + b * quarters_in_bar;
        for(int q = int(std::ceil(quarters_in_bar)) - 1; q >= 1; q--)
          emit(bar_line + q, false);
        emit(bar_line, true);
      }
    }
  }

  [[nodiscard]] constexpr bool unexpected_bar_change() const noexcept
  {
    double bar_difference = musical_end_last_bar - musical_start_last_bar;
    if(bar_difference != 0.)
    {
      // If the difference is divisble by the signature,
      // then the bar change is expected.
      // e.g. start = 4 -> end = 8  ; signature = 4/4 : good
      // e.g. start = 4 -> end = 8  ; signature = 6/8 : bad
      // e.g. start = 4 -> end = 7  ; signature = 6/8 : good

      if(bar_difference < 0.)
        bar_difference = -bar_difference;

      double quarters_sig = 4. * double(signature.upper) / signature.lower;
      double div = bar_difference / quarters_sig;
      bool unexpected = div - int64_t(div) > 0.000001;
      return unexpected;
    }
    return false;
  }

  constexpr void set_end_time(time_value t) noexcept
  // C++23: [[ expects: t <= this->date && t > this->prev_date ]]
  {
    const auto old_date = date;
    date = t;

    if(old_date.impl > 0)
    {
      double ratio = t.impl / double(old_date.impl);
      musical_end_position *= ratio;
    }

    // TODO what if musical_end_position is now before musical_end_last_bar
  }

  constexpr void set_start_time(time_value t) noexcept
  // C++23: [[ expects: t <= this->date && t > this->prev_date ]]
  {
    const auto old_date = prev_date;
    prev_date = t;

    if(old_date.impl > 0)
    {
      double ratio = t.impl / double(old_date.impl);
      musical_start_position *= ratio;
    }

    // TODO what if musical_start_position is now after end_position /
    // end_last_bar ?
  }

  ossia::time_value prev_date{};       // Sample we are at
  ossia::time_value date{};            // Sample we are finishing at
  ossia::time_value parent_duration{}; // Duration of the parent item of the
                                       // one being ticked

  //! Where in the tick we are located.
  //! e.g. if we read 9 time_value across two intervals,
  //! the second interval's first token_request will have
  //! - prev_date = 0
  //! - date = 5
  //! - offset = 3
  //!
  //!    [  |     [ : 9
  //! -----------------
  //!   7   |     10
  //!
  //! Note that the offset is independent of the actual execution speed
  ossia::time_value offset{};

  double speed{1.};
  double tempo{ossia::root_tempo};
  time_signature signature{}; // Time signature at start

  ossia::quarter_note musical_start_last_signature{}; // Position of the last bar
                                                      // signature change in quarter
                                                      // notes (at prev_date)
  ossia::quarter_note musical_start_last_bar{}; // Position of the last bar start in
                                                // quarter notes (at prev_date)
  ossia::quarter_note musical_start_position{}; // Current position in quarter notes
  ossia::quarter_note musical_end_last_bar{};   // Position of the last bar start in
                                                // quarter notes (at date)
  ossia::quarter_note musical_end_position{};   // Current position in quarter notes
  bool start_discontinuous{};
  bool end_discontinuous{};
};

inline bool operator==(const token_request& lhs, const token_request& rhs)
{
  return lhs.prev_date == rhs.prev_date && lhs.date == rhs.date
         && lhs.parent_duration == rhs.parent_duration && lhs.offset == rhs.offset
         && lhs.speed == rhs.speed && lhs.tempo == rhs.tempo
         && lhs.signature == rhs.signature
         && lhs.musical_start_last_bar == rhs.musical_start_last_bar
         && lhs.musical_start_position == rhs.musical_start_position
         && lhs.musical_end_last_bar == rhs.musical_end_last_bar
         && lhs.musical_end_position == rhs.musical_end_position
         && lhs.start_discontinuous == rhs.start_discontinuous
         && lhs.end_discontinuous == rhs.end_discontinuous;
}

inline bool operator!=(const token_request& lhs, const token_request& rhs)
{
  return !(lhs == rhs);
}

// To be used only for simple examples
struct simple_token_request
{
  time_value prev_date{};
  time_value date{};
  time_value parent_duration{};
  time_value offset{};

  operator token_request() const noexcept
  {
    return ossia::token_request{prev_date, date, parent_duration, offset, 1.0,
                                {4, 4},    120.};
  }

  friend bool operator==(const token_request& lhs, const simple_token_request& self)
  {
    return lhs.prev_date == self.prev_date && lhs.date == self.date
           && lhs.offset == self.offset;
  }
  friend bool operator==(const simple_token_request& self, const token_request& rhs)
  {
    return rhs == self;
  }
  friend bool operator!=(const token_request& lhs, const simple_token_request& self)
  {
    return !(lhs == self);
  }
  friend bool operator!=(const simple_token_request& self, const token_request& rhs)
  {
    return !(rhs == self);
  }
};
}
