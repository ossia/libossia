#pragma once
#include <ossia_export.h>

namespace ossia
{
class value;

/*! \details Impulse value */
struct OSSIA_EXPORT Impulse
{
  /*! constructor */
  Impulse() = default;
  Impulse(const Impulse&) = default;
  Impulse(Impulse&&) = default;
  Impulse& operator=(const Impulse&) = default;
  Impulse& operator=(Impulse&&) = default;

  bool operator==(const value&) const
  {
    return true;
  }
  bool operator!=(const value&) const
  {
    return false;
  }
  bool operator>(const value&) const
  {
    return false;
  }
  bool operator>=(const value&) const
  {
    return true;
  }
  bool operator<(const value&) const
  {
    return false;
  }
  bool operator<=(const value&) const
  {
    return true;
  }
};
}
