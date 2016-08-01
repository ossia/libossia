#pragma once
#include <ossia_export.h>

namespace ossia
{
class value;

/**
 * @class Int int.hpp ossia/editor/value/value.hpp
 */
struct OSSIA_EXPORT Int
{
  int value;

  Int(int v = 0) : value(v)
  {
  }

  template <typename T>
  Int(T*) = delete;

  Int(const Int&) = default;
  Int(Int&&) = default;
  Int& operator=(const Int&) = default;
  Int& operator=(Int&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
