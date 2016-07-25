#pragma once
#include <ossia_export.h>

namespace OSSIA
{
class Value;

/*! \details Impulse value */
struct OSSIA_EXPORT Impulse
{
  /*! constructor */
  Impulse() = default;
  Impulse(const Impulse&) = default;
  Impulse(Impulse&&) = default;
  Impulse& operator=(const Impulse&) = default;
  Impulse& operator=(Impulse&&) = default;

  bool operator== (const Value&) const { return true; }
  bool operator!= (const Value&) const { return false; }
  bool operator>  (const Value&) const { return false; }
  bool operator>= (const Value&) const { return true; }
  bool operator<  (const Value&) const { return false; }
  bool operator<= (const Value&) const { return true; }
};

}
