#pragma once
#include <ossia/editor/scenario/time_signature.hpp>

#include <ossia-qt/time_value.hpp>

#include <verdigris>

namespace ossia
{
using musical_sync = double;
struct bar_time
{
  int32_t bars{};
  int16_t quarters{};
  int8_t semiquavers{};
  int8_t cents{};

  friend bool operator==(const bar_time& lhs, const bar_time& rhs) noexcept
  {
    return lhs.bars == rhs.bars && lhs.quarters == rhs.quarters
           && lhs.semiquavers == rhs.semiquavers && lhs.cents == rhs.cents;
  }
  friend bool operator!=(const bar_time& lhs, const bar_time& rhs) noexcept
  {
    return !(lhs == rhs);
  }
};
}

inline QDataStream& operator<<(QDataStream& i, const ossia::time_signature& sel)
{
  return i << sel.upper << sel.lower;
}

inline QDataStream& operator>>(QDataStream& i, ossia::time_signature& sel)
{
  return i >> sel.upper >> sel.lower;
}

inline QDataStream& operator<<(QDataStream& i, const ossia::bar_time& sel)
{
  return i << sel.bars << sel.quarters << sel.semiquavers << sel.cents;
}

inline QDataStream& operator>>(QDataStream& i, ossia::bar_time& sel)
{
  return i >> sel.bars >> sel.quarters >> sel.semiquavers >> sel.cents;
}

Q_DECLARE_METATYPE(ossia::musical_sync)
W_REGISTER_ARGTYPE(ossia::musical_sync)

Q_DECLARE_METATYPE(ossia::bar_time)
W_REGISTER_ARGTYPE(ossia::bar_time)

Q_DECLARE_METATYPE(ossia::time_signature)
W_REGISTER_ARGTYPE(ossia::time_signature)
