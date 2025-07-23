#pragma once
#include <ossia/editor/scenario/time_value.hpp>

// clang-format off
// QtCore/qdatastream.h:313:34: error: no member named 'distance' in namespace 'std'
//                              const qint64 last = std::distance(rangeStart, it) - 1;
#include <iterator>
#include <QDataStream>
// clang-format on

#include <verdigris>

inline QDataStream& operator<<(QDataStream& i, const ossia::time_value& sel)
{
  return i << qint64(sel.impl);
}

inline QDataStream& operator>>(QDataStream& i, ossia::time_value& sel)
{
  qint64 r;
  i >> r;
  sel.impl = r;
  return i;
}

Q_DECLARE_METATYPE(ossia::time_value)
W_REGISTER_ARGTYPE(ossia::time_value)
