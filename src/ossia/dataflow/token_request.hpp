#pragma once
#include <ossia/detail/flicks.hpp>
#include <ossia/detail/math.hpp>
#include <ossia/editor/scenario/time_signature.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <cassert>
#include <optional>

#if defined(_LIBCPP_CONSTEXPR_SINCE_CXX23) || defined(_GLIBCXX23_CONSTEXPR)
#define ossia_constexpr_msvc_workaround constexpr
#else
#define ossia_constexpr_msvc_workaround
#endif
namespace ossia
{
using quarter_note = double;

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
    ossia::token_request other = *this;
    ossia::time_value orig_from = other.prev_date;
    ossia::time_value tick_amount = other.date - other.prev_date;

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

  //! How much we read from our data model
  [[nodiscard]] constexpr time_value model_read_duration() const noexcept
  {
    return date - prev_date;
  }

  //! The date of the first sample in the context of the parent.
  //! e.g. if we're at the start of our third buffer of 256 samples for
  //! a given time_interval, this will give 768.
  [[nodiscard]] constexpr physical_time
  start_date_to_physical(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    assert(speed != 0.);
    return this->prev_date.impl * ratio / speed;
  }

  //! Where we must start to read / write in our physical buffers
  [[nodiscard]] constexpr physical_time physical_start(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    assert(speed != 0.);
    return this->offset.impl * ratio / speed;
  }

  //! Given a sound file at 44100 and a system rate at 44100,
  //! this is the amount of samples that we must process from the sound file
  //! during this tick
  [[nodiscard]] constexpr physical_time
  physical_read_duration(double ratio) const noexcept
  {
    return constexpr_ceil(abs(date - prev_date).impl * ratio);
  }

  //! Given a sound file at 44100 and a system rate at 44100,
  //! this is the amount of samples that we must write in the audio buffer
  [[nodiscard]] constexpr physical_time
  physical_write_duration(double ratio) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    assert(speed != 0.);
    return constexpr_ceil(abs(date - prev_date).impl * ratio / speed);
  }

  //! This is an upper bound on what we can write to a buffer.
  [[nodiscard]] constexpr physical_time
  safe_physical_write_duration(double ratio, int bufferSize) const noexcept
  // C++23: [[ expects: speed != 0. ]]
  {
    assert(speed != 0.);
    return constexpr_floor(bufferSize - offset.impl * ratio / speed);
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
    return (global_time - prev_date + offset).impl * ratio / speed;
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
    return time_value{
        constexpr_floor(phys_time * (speed / ratio) + prev_date.impl - offset.impl)};
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
    if((musical_end_last_bar != musical_start_last_bar) || musical_start_position == 0.)
    {
      // There is a bar change in this tick, start the up tick
      const double musical_tick_duration = musical_end_position - musical_start_position;
      if(musical_tick_duration != 0)
      {
        const double musical_bar_start = musical_end_last_bar - musical_start_position;
        const int64_t samples_tick_duration
            = physical_write_duration(modelToSamplesRatio);
        if(samples_tick_duration > 0)
        {
          const double ratio = musical_bar_start / musical_tick_duration;
          const int64_t hi_start_sample = samples_tick_duration * ratio;
          tick(hi_start_sample);
        }
      }
    }
    else
    {
      const int64_t start_quarter
          = std::floor(musical_start_position - musical_start_last_bar);
      const int64_t end_quarter
          = std::floor(musical_end_position - musical_start_last_bar);
      if(start_quarter != end_quarter)
      {
        // There is a quarter change in this tick, start the down tick
        // start_position is prev_date
        // end_position is date
        const double musical_tick_duration
            = musical_end_position - musical_start_position;
        if(musical_tick_duration != 0)
        {
          const double musical_bar_start
              = (end_quarter + musical_start_last_bar) - musical_start_position;
          const int64_t samples_tick_duration
              = physical_write_duration(modelToSamplesRatio);
          if(samples_tick_duration > 0)
          {
            const double ratio = musical_bar_start / musical_tick_duration;
            const int64_t lo_start_sample = samples_tick_duration * ratio;
            tock(lo_start_sample);
          }
        }
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
