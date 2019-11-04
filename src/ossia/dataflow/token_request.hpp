#pragma once
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/scenario/time_signature.hpp>

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
    : prev_date{prev_d}, date{d}, parent_duration{parent_duration},
      offset{off}, speed{s}
      , tempo{tempo}, signature{sig}
  {
    if (offset.impl < 0)
    {
      offset.impl = 0;
    }
  }

  token_request add_offset(ossia::time_value t) const noexcept
  {
    token_request other = *this;
    other.prev_date += t;
    other.date += t;
    return other;
  }

  template<typename F>
  void loop(ossia::time_value start_offset, ossia::time_value loop_duration, F f) const noexcept
  {
    ossia::token_request other = *this;
    ossia::time_value orig_from = other.prev_date;
    ossia::time_value tick_amount = other.date - other.prev_date;

    while (tick_amount > 0)
    {
      const auto cur_from = orig_from.impl % loop_duration.impl;
      if (cur_from + tick_amount < loop_duration)
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
        other.offset += this_tick;
      }
    }
  }

/*
  constexpr token_request(
      ossia::time_value prev_d, ossia::time_value d, double pos,
      ossia::time_value off) noexcept
      : token_request{prev_d, d, pos, off, 1.}
  {
  }

  constexpr token_request(
      ossia::time_value prev_d, ossia::time_value d, double pos) noexcept
      : token_request{prev_d, d, pos, time_value{}, 1.}
  {
  }

  constexpr token_request(
      ossia::time_value prev_d, ossia::time_value d) noexcept
      : token_request{prev_d, d, 0., time_value{}, 1.}
  {
  }
*/
  constexpr bool in_range(ossia::time_value global_time) const noexcept
  {
    return global_time.impl >= prev_date.impl && global_time.impl < date.impl;
  }

  constexpr ossia::time_value tick_start() const noexcept
  {
    return offset;
  }

  constexpr ossia::time_value to_tick_time(ossia::time_value global_time) const noexcept
  {
    return global_time - prev_date + offset;
  }

  constexpr ossia::time_value to_tick_time(int64_t global_time) const noexcept
  {
    return to_tick_time(ossia::time_value{global_time});
  }

  constexpr double position() const noexcept
  {
    return parent_duration.impl > 0 ? date.impl / double(parent_duration.impl) : 0.;
  }

  ossia::time_value prev_date{}; // Sample we are at
  ossia::time_value date{}; // Sample we are finishing at
  ossia::time_value parent_duration{}; // Duration of the parent item of the one being ticked
  ossia::time_value offset{}; // Position at which to write in the output buffer
  double speed{1.};
  double tempo{120.};
  time_signature signature{}; // Time signature at start

  ossia::quarter_note musical_start_last_bar{}; // Position of the last bar start in quarter notes (at prev_date)
  ossia::quarter_note musical_start_position{}; // Current position in quarter notes
  ossia::quarter_note musical_end_last_bar{}; // Position of the last bar start in quarter notes (at date)
  ossia::quarter_note musical_end_position{}; // Current position in quarter notes
  bool start_discontinuous{};
  bool end_discontinuous{};
};

inline bool operator==(const token_request& lhs, const token_request& rhs)
{
  return    lhs.prev_date == rhs.prev_date
         && lhs.date == rhs.date
         && lhs.parent_duration == rhs.parent_duration
         && lhs.offset == rhs.offset
         && lhs.speed == rhs.speed
         && lhs.tempo == rhs.tempo
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
struct simple_token_request : token_request
{
  using token_request::token_request;
  simple_token_request(ossia::time_value prev_d, ossia::time_value d):
    simple_token_request{prev_d, d, 0_tv}
  {
  }
  simple_token_request(ossia::time_value prev_d, ossia::time_value d, ossia::time_value offset):
    ossia::token_request{prev_d, d, 0_tv, offset, 1.0, {4,4}, 120.}
  {
  }

  friend bool operator==(const token_request& lhs, const simple_token_request& self)
  {
    return
        lhs.prev_date == self.prev_date &&
        lhs.date == self.date &&
        lhs.offset == self.offset;
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
