#pragma once
#include <ossia/editor/scenario/time_value.hpp>
#include <verdigris>

#include <QDataStream>

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
