#pragma once
#include <ossia/detail/config.hpp>
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
  OSSIA_DECL_RELAXED_CONSTEXPR Int(T*) = delete;

  OSSIA_DECL_RELAXED_CONSTEXPR Int(const Int&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Int(Int&&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(const Int&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(Int&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
