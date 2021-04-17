#pragma once
#include <ossia/dataflow/token_request.hpp>

#include <ossia-qt/time_value.hpp>

#include <verdigris>

namespace ossia
{
struct bar_time {
  int32_t bars{};
  int16_t quarters{};
  int8_t semiquavers{};
  int8_t cents{};

  friend bool operator==(const bar_time& lhs, const bar_time& rhs) noexcept
  {
    return lhs.bars == rhs.bars
        && lhs.quarters == rhs.quarters
        && lhs.semiquavers == rhs.semiquavers
        && lhs.cents == rhs.cents
    ;
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

inline QDataStream& operator<<(QDataStream& i, const ossia::token_request& sel)
{
  return
  i << sel.prev_date
    << sel.date
    << sel.parent_duration
    << sel.offset
    << sel.speed
    << sel.tempo
    << sel.signature
    << sel.musical_start_last_signature
    << sel.musical_start_last_bar
    << sel.musical_start_position
    << sel.musical_end_last_bar
    << sel.musical_end_position
    << sel.start_discontinuous
    << sel.end_discontinuous
  ;
}

inline QDataStream& operator>>(QDataStream& i, ossia::token_request& sel)
{
  return
  i >> sel.prev_date
    >> sel.date
    >> sel.parent_duration
    >> sel.offset
    >> sel.speed
    >> sel.tempo
    >> sel.signature
    >> sel.musical_start_last_signature
    >> sel.musical_start_last_bar
    >> sel.musical_start_position
    >> sel.musical_end_last_bar
    >> sel.musical_end_position
    >> sel.start_discontinuous
    >> sel.end_discontinuous
  ;
}

Q_DECLARE_METATYPE(ossia::bar_time)
W_REGISTER_ARGTYPE(ossia::bar_time)

Q_DECLARE_METATYPE(ossia::token_request)
W_REGISTER_ARGTYPE(ossia::token_request)
