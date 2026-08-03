#pragma once
#include <ossia/detail/flicks.hpp>
#include <ossia/detail/math.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/editor/scenario/time_signature.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <cassert>
#include <cmath>
#include <optional>
#include <type_traits>

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

    // The pieces of a looped tick must tile the parent's carried sample span
    // the way the parent tiles the buffer. Cutting on the accumulated model
    // amount with a single rounding keeps the cuts monotone, so consecutive
    // pieces share their boundary sample by construction.
    const int64_t total_amount
        = tick_amount.impl >= 0 ? tick_amount.impl : -tick_amount.impl;
    const bool has_span
        = start_sample >= 0 && length_sample >= 0 && total_amount > 0;
    int64_t consumed = 0;
    const auto set_piece_span = [&](int64_t piece) constexpr {
      if(has_span)
      {
        const int64_t s0 = start_sample
                           + (int64_t{length_sample} * consumed + total_amount / 2)
                                 / total_amount;
        const int64_t s1
            = start_sample
              + (int64_t{length_sample} * (consumed + piece) + total_amount / 2)
                    / total_amount;
        other.start_sample = int32_t(s0);
        other.length_sample = int32_t(s1 - s0);
      }
      consumed += piece;
    };

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
          set_piece_span(tick_amount.impl);
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

          set_piece_span(this_tick.impl);
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

        set_piece_span(this_tick);
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

  //! Where we must start to read / write in our physical buffers.
  //! The producer's word is taken when it gave one: reconstructing the sample
  //! from the flick-quantised offset can land one sample off the actual cut.
  [[nodiscard]] constexpr physical_time physical_start(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    if(start_sample >= 0)
      return start_sample;
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
  //! this is the amount of samples that we must write in the audio buffer.
  //! As with physical_start, the span the producer carried wins over the
  //! flick-rounded reconstruction.
  [[nodiscard]] constexpr physical_time
  physical_write_duration(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    if(length_sample >= 0)
      return length_sample;
    return sample_at(this->offset + abs(date - prev_date), ratio)
           - sample_at(this->offset, ratio);
  }

  //! This is an upper bound on what we can write to a buffer.
  [[nodiscard]] constexpr physical_time
  safe_physical_write_duration(double ratio, int bufferSize) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    return bufferSize - physical_start(ratio);
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
    // How far into the tick this date sits, counted forwards in both directions.
    const int64_t in_tick
        = speed < 0. ? (prev_date - global_time).impl : (global_time - prev_date).impl;

    // Place it inside the span this tick was actually handed, so an event and
    // the audio it belongs to cannot end up on different samples. Reconstructing
    // the position from the model dates can miss the span by one.
    if(start_sample >= 0 && length_sample >= 0)
    {
      const int64_t dt = abs(date - prev_date).impl;
      if(dt <= 0)
        return start_sample;
      int64_t s = start_sample
                  + constexpr_floor(double(in_tick) / double(dt) * length_sample);
      if(s < start_sample)
        s = start_sample;
      else if(s > int64_t(start_sample) + length_sample)
        s = int64_t(start_sample) + length_sample;
      return s;
    }

    assert(speed != 0.);
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


  //! Calls fn(bar_line, next_bar_line) for every bar segment the tick touches,
  //! in increasing musical order.
  //!
  //! Bar lines come from two places and both matter: the ones the signature
  //! implies, and the one the interval reported for the far end of the tick.
  //! A signature change puts a bar line where the arithmetic alone would not,
  //! so the grid restarts there.
  template <typename F>
  void for_each_bar_segment(double lo, double hi, F&& fn) const noexcept
  {
    const bool valid_sig = signature.upper > 0 && signature.lower > 0;
    const double quarters_in_bar
        = valid_sig ? 4. * signature.upper / signature.lower : 4.;
    if(!(quarters_in_bar > 0.))
      return;

    constexpr double eps = 1e-9;
    const bool rewinding = date < prev_date;
    const double near_bar = rewinding ? musical_end_last_bar : musical_start_last_bar;
    const double far_bar = rewinding ? musical_start_last_bar : musical_end_last_bar;

    // Up to the reported far bar, on the grid the near bar defines.
    double b = near_bar;
    for(int i = 0; i < 1024 && b < far_bar - eps; i++)
    {
      const double next = (b + quarters_in_bar < far_bar) ? b + quarters_in_bar : far_bar;
      if(next > lo + eps && b < hi + eps)
        fn(b, next);
      b += quarters_in_bar;
    }

    // From the far bar on, on the grid it defines.
    b = (far_bar > near_bar) ? far_bar : near_bar;
    for(int i = 0; i < 1024 && b < hi + eps; i++)
    {
      fn(b, b + quarters_in_bar);
      b += quarters_in_bar;
    }
  }

  //! Given a quantification rate (1 for bars, 2 for half, 4 for quarters...)
  //! return the next occurring quantification date, if such date is in the tick
  //! defined by this token_request.
  //!
  //! This is the first of get_quantification_dates(), not a second
  //! implementation of it: a node that takes one point and a node that takes
  //! them all have to agree about where the grid is.
  [[nodiscard]] std::optional<time_value>
  get_quantification_date(double rate) const noexcept
  {
    if(prev_date == date)
      return std::nullopt;

    // Quantized triggers are not interactive while rewinding.
    if(backward())
      return std::nullopt;

    const auto pts = get_quantification_dates(rate);
    if(pts.empty())
      return std::nullopt;
    return pts[0].date;
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
      // Scale the musical distance to this point by the tick, rather than
      // scaling the tick by a normalised position. The two associate
      // differently in floating point and the result is truncated to a whole
      // flick, so the other order lands a flick short on dates that come out
      // exact in this one.
      const double scale = double(tick_duration.impl) / musical_tick_duration;
      time_value d
          = prev_date + (musical_position - musical_start_position) * scale;

      // The tick owns [start; end[ in musical positions as well as in dates:
      // a point sitting musically on the far end belongs to the next tick,
      // even when truncating its date to a whole flick pulls it inside this
      // one. Without this, the point fires here at the last flick AND in the
      // next tick at its first one.
      if(rewinding)
      {
        if(musical_position <= musical_end_position)
          return false;
        if(d > prev_date)
          d = prev_date;
        if(d <= date)
          return false;
      }
      else
      {
        if(musical_position >= musical_end_position)
          return false;
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

    // Collect the segments first so a rewinding walk can take them in reverse:
    // the points inside a segment are monotone in k, and so are the segments.
    ossia::small_vector<std::pair<double, double>, 8> segments;
    for_each_bar_segment(lo, hi, [&](double bar_line, double next_bar) {
      if(segments.size() < 1024)
        segments.push_back({bar_line, next_bar});
    });

    const auto walk_segment = [&](double bar_line, double next_bar) {
      const int divs = int(std::ceil((next_bar - bar_line) / unit)) + 1;
      if(!rewinding)
      {
        for(int64_t k = 0; k <= divs; k++)
        {
          const double p = bar_line + k * unit;
          if(p >= next_bar - eps || p > hi + eps)
            return true;
          if(p < lo)
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
          if(p >= next_bar - eps || p > hi + eps)
            continue;
          if(p < lo)
            return true;
          if(!try_push(p, k))
            return false;
        }
      }
      return true;
    };

    if(!rewinding)
    {
      for(const auto& [b, n] : segments)
        if(!walk_segment(b, n))
          break;
    }
    else
    {
      for(auto it = segments.rbegin(); it != segments.rend(); ++it)
        if(!walk_segment(it->first, it->second))
          break;
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

    // The same bar segments the quantification grid uses, so a click and a
    // quantized event at the same bar line land on the same sample.
    double seg_lo[64]{};
    double seg_hi[64]{};
    int n_seg = 0;
    for_each_bar_segment(lo, hi, [&](double bar_line, double next_bar) {
      if(n_seg < 64)
      {
        seg_lo[n_seg] = bar_line;
        seg_hi[n_seg] = next_bar;
        n_seg++;
      }
    });

    const auto walk_segment = [&](double bar_line, double next_bar) {
      if(!rewinding)
      {
        emit(bar_line, true);
        for(double q = bar_line + 1.; q < next_bar - 1e-9; q += 1.)
          emit(q, false);
      }
      else
      {
        double last = bar_line;
        for(double q = bar_line + 1.; q < next_bar - 1e-9; q += 1.)
          last = q;
        for(double q = last; q > bar_line + 1e-9; q -= 1.)
          emit(q, false);
        emit(bar_line, true);
      }
    };

    if(!rewinding)
      for(int i = 0; i < n_seg; i++)
        walk_segment(seg_lo[i], seg_hi[i]);
    else
      for(int i = n_seg - 1; i >= 0; i--)
        walk_segment(seg_lo[i], seg_hi[i]);
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

  //! The fraction of the tick [prev_date; t] represents, in [0; 1], correct
  //! in both playback directions. Used to split the carried sample span the
  //! same way the model dates are split.
  [[nodiscard]] constexpr double tick_fraction_at(time_value t) const noexcept
  {
    const double total = double(date.impl - prev_date.impl);
    if(total == 0.)
      return 0.;
    double f = double(t.impl - prev_date.impl) / total;
    if(f < 0.)
      f = 0.;
    else if(f > 1.)
      f = 1.;
    return f;
  }

  constexpr void set_end_time(time_value t) noexcept
  // C++23: [[ expects: t <= this->date && t > this->prev_date ]]
  {
    if(length_sample > 0)
      length_sample = int32_t(length_sample * tick_fraction_at(t) + 0.5);

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
    if(length_sample > 0)
    {
      const auto skipped = int32_t(length_sample * tick_fraction_at(t) + 0.5);
      start_sample += skipped;
      length_sample -= skipped;
    }

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

  //! The span of the audio buffer this token covers, in samples: this token's
  //! node must write samples [start_sample; start_sample + length_sample[.
  //!
  //! Decided by whoever cut the tick (the root audio callback, or a scenario
  //! splitting it on an interval boundary) and carried verbatim, because it
  //! cannot be reconstructed: the model dates are quantised to whole flicks,
  //! so dividing them back by the speed lands next to the sample the cut was
  //! actually taken at, and the last sample of the buffer ends up written by
  //! nobody. -1 means the producer did not know the buffer (hand-made tokens,
  //! non-audio drivers); consumers then fall back to deriving the span from
  //! the model dates.
  int32_t start_sample{-1};
  int32_t length_sample{-1};

  bool start_discontinuous{};
  bool end_discontinuous{};

  ossia::quarter_note musical_start_last_signature{}; // Position of the last bar
                                                      // signature change in quarter
                                                      // notes (at prev_date)
  ossia::quarter_note musical_start_last_bar{}; // Position of the last bar start in
                                                // quarter notes (at prev_date)
  ossia::quarter_note musical_start_position{}; // Current position in quarter notes
  ossia::quarter_note musical_end_last_bar{};   // Position of the last bar start in
                                                // quarter notes (at date)
  ossia::quarter_note musical_end_position{};   // Current position in quarter notes
};

// Copied per node per tick on the audio thread: it has to stay a POD that
// memcpys, and it should not grow carelessly.
static_assert(std::is_trivially_copyable_v<token_request>);
static_assert(std::is_standard_layout_v<token_request>);
static_assert(sizeof(token_request) == 104);

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
