#pragma once
#include <ossia_export.h>

namespace ossia
{
class value;
/*! \details Char value */
struct OSSIA_EXPORT Char
{
  char value;

  /*! constructor
   \param char value */
  Char(char v = 0x00) : value(v)
  {
  }

  Char(const Char&) = default;
  Char(Char&&) = default;
  Char& operator=(const Char&) = default;
  Char& operator=(Char&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
