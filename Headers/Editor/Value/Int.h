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
  virtual ~Int();
  Int(const Int&) = default;
  Int(Int&&) = default;
  Int& operator=(const Int&) = default;
  Int& operator=(Int&&) = default;

  /*! equal operator */
  bool operator== (const Value&) const;

  /*! different operator */
  bool operator!= (const Value&) const;

  /*! greater than operator */
  bool operator> (const Value&) const;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const;

  /*! less than operator */
  bool operator< (const Value&) const;

  /*! less than and equal operator */
  bool operator<= (const Value&) const;
};

}
