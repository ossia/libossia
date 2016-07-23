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

  virtual ~Impulse();

  /*! equal operator */
  bool operator== (const Value&) const
  { return true; }

  /*! different operator */
  bool operator!= (const Value&) const
  { return false; }

  /*! greater than operator */
  bool operator> (const Value&) const
  { return false; }

  /*! greater than and equal operator */
  bool operator>= (const Value&) const
  { return true; }

  /*! less than operator */
  bool operator< (const Value&) const
  { return false; }

  /*! less than and equal operator */
  bool operator<= (const Value&) const
  { return true; }
};

}
