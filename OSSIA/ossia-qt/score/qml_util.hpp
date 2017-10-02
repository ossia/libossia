#pragma once
#include <limits>
#include <ossia/editor/scenario/time_value.hpp>
#include <QtGlobal>
namespace ossia
{
namespace qt
{
inline qint32 infinite() { return std::numeric_limits<qint32>::lowest(); }

inline bool isInfinite(qint32 t) { return t == infinite(); }

inline auto defaultTime(qint32 t) -> ossia::time_value {
  return t == infinite()
      ? ossia::Infinite
      : ossia::time_value{t * 1000.};
}

inline auto reverseTime(time_value t) -> qint32 {
  return t.infinite()
      ? infinite()
      : double(t) / 1000.;
}
}
}
