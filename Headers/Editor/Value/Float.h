#pragma once
#include <ossia_export.h>

namespace OSSIA
{
class Value;
/*! \details Float value */
struct OSSIA_EXPORT Float
{
  float value;

  /*! constructor
   \param float value */
  Float(float v = 0.):
      value(v)
  {
  }

  virtual ~Float();
  Float(const Float&) = default;
  Float(Float&&) = default;
  Float& operator=(const Float&) = default;
  Float& operator=(Float&&) = default;

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
