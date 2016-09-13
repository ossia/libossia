#pragma once
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
  constexpr Char(char v = 0x00) : value(v)
  {
  }

  constexpr Char(const Char&) = default;
  constexpr Char(Char&&) = default;
  constexpr Char& operator=(const Char&) = default;
  constexpr Char& operator=(Char&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
