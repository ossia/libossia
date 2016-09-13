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

  constexpr Int(int v = 0) : value(v)
  {
  }

  template <typename T>
  constexpr Int(T*) = delete;

  constexpr Int(const Int&) = default;
  constexpr Int(Int&&) = default;
  constexpr Int& operator=(const Int&) = default;
  constexpr Int& operator=(Int&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
