#pragma once
#include <ossia/detail/config.hpp>
#include <ossia_export.h>

namespace ossia
{
class value;

/**
 * @class Char char.hpp ossia/editor/value/value.hpp
 */
struct OSSIA_EXPORT Char
{
  char value;

  /*! constructor
   \param char value */
  OSSIA_DECL_RELAXED_CONSTEXPR Char(char v = 0x00) : value(v)
  {
  }

  OSSIA_DECL_RELAXED_CONSTEXPR Char(const Char&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Char(Char&&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Char& operator=(const Char&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Char& operator=(Char&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
