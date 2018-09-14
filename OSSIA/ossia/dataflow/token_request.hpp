#pragma once
#include <ossia/editor/scenario/time_value.hpp>

namespace ossia
{

struct token_request
{
  constexpr token_request() noexcept = default;
  constexpr token_request(const token_request&) noexcept = default;
  constexpr token_request(token_request&&) noexcept = default;
  constexpr token_request& operator=(const token_request&) noexcept = default;
  constexpr token_request& operator=(token_request&&) noexcept = default;

  constexpr token_request(
      ossia::time_value prev_d, ossia::time_value d, double pos,
      ossia::time_value off, double s) noexcept
      : prev_date{prev_d}, date{d}, position{pos}, offset{off}, speed{s}
  {
    if (offset.impl < 0)
    {
      offset.impl = 0;
    }
  }

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

  constexpr bool in_range(ossia::time_value global_time) const
  {
    return global_time.impl >= prev_date.impl && global_time.impl < date.impl;
  }

  constexpr ossia::time_value tick_start() const
  {
    return offset;
  }

  constexpr ossia::time_value to_tick_time(ossia::time_value global_time) const
  {
    return global_time - prev_date + offset;
  }

  constexpr ossia::time_value to_tick_time(int64_t global_time) const
  {
    return to_tick_time(ossia::time_value{global_time});
  }

  ossia::time_value prev_date{};
  ossia::time_value date{};
  double position{};
  ossia::time_value offset{};
  double speed{1.};
  bool start_discontinuous{};
  bool end_discontinuous{};
};

inline bool operator==(const token_request& lhs, const token_request& rhs)
{
  return lhs.date == rhs.date && lhs.position == rhs.position
         && lhs.offset == rhs.offset && lhs.speed == rhs.speed
         && lhs.start_discontinuous == rhs.start_discontinuous
         && lhs.end_discontinuous == rhs.end_discontinuous;
}
inline bool operator!=(const token_request& lhs, const token_request& rhs)
{
  return !(lhs == rhs);
}
}
