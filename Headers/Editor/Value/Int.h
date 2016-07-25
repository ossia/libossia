#pragma once
#include <ossia_export.h>

namespace OSSIA
{
class Value;
/*! \details Int value */
struct OSSIA_EXPORT Int
{
  int value;

  /*! constructor
   \param int value */
  Int(int v = 0):
      value(v)
  {
  }

  Int(const Int&) = default;
  Int(Int&&) = default;
  Int& operator=(const Int&) = default;
  Int& operator=(Int&&) = default;

  bool operator== (const Value&) const;
  bool operator!= (const Value&) const;
  bool operator>  (const Value&) const;
  bool operator>= (const Value&) const;
  bool operator<  (const Value&) const;
  bool operator<= (const Value&) const;
};

}
