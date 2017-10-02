#pragma once
#include <limits>
#include <ossia/editor/scenario/time_value.hpp>
namespace ossia
{
namespace qt
{
inline int64_t infinite() { return std::numeric_limits<int64_t>::lowest(); }

inline bool isInfinite(int64_t t) { return t == infinite(); }

inline auto defaultTime(int64_t t) -> ossia::time_value {
  return t == infinite()
      ? ossia::Infinite
      : ossia::time_value{t * 1000.};
}

inline auto reverseTime(time_value t) -> int64_t {
  return t.infinite()
      ? infinite()
      : double(t) / 1000.;
}
}
}
