#pragma once
#include <ossia_export.h>

namespace ossia
{
class value;
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

  bool operator== (const ossia::value&) const;
  bool operator!= (const ossia::value&) const;
  bool operator>  (const ossia::value&) const;
  bool operator>= (const ossia::value&) const;
  bool operator<  (const ossia::value&) const;
  bool operator<= (const ossia::value&) const;
};

}
