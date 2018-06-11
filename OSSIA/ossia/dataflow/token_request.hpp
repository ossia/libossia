#pragma once
#include <ossia/editor/scenario/time_value.hpp>

namespace ossia
{

struct token_request
{
    token_request() noexcept = default;
    token_request(const token_request&) noexcept = default;
    token_request(token_request&&) noexcept = default;
    token_request& operator=(const token_request&) noexcept = default;
    token_request& operator=(token_request&&) noexcept = default;

    token_request(
        ossia::time_value prev_d
        , ossia::time_value d
        , double pos
        , ossia::time_value off
        , double s) noexcept
      : prev_date{prev_d}
      , date{d}
      , position{pos}
      , offset{off}
      , speed{s}
    {
      if(offset.impl < 0)
      {
        offset.impl = 0;
      }
    }

    token_request(
        ossia::time_value prev_d
        , ossia::time_value d
        , double pos
        , ossia::time_value off) noexcept
      : token_request{prev_d, d, pos, off, 1.}
    {

    }

    token_request(ossia::time_value prev_d, ossia::time_value d, double pos) noexcept
      : token_request{prev_d, d, pos, time_value{}, 1.}
    {

    }

    token_request(ossia::time_value prev_d, ossia::time_value d) noexcept
      : token_request{prev_d, d, 0., time_value{}, 1.}
    {

    }

    ossia::time_value prev_date{};
    ossia::time_value date{};
    double position{};
    ossia::time_value offset{};
    double speed{1.};
    bool start_discontinuous{};
    bool end_discontinuous{};
};

inline bool operator==(const token_request& lhs, const token_request& rhs) {
  return lhs.date == rhs.date && lhs.position == rhs.position
      && lhs.offset == rhs.offset && lhs.speed == rhs.speed && lhs.start_discontinuous == rhs.start_discontinuous
      && lhs.end_discontinuous == rhs.end_discontinuous;
}
inline bool operator!=(const token_request& lhs, const token_request& rhs) {
  return !(lhs == rhs);
}

}
