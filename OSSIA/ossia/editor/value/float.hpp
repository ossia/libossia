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

  Float(const Float&) = default;
  Float(Float&&) = default;
  Float& operator=(const Float&) = default;
  Float& operator=(Float&&) = default;

  bool operator== (const Value&) const;
  bool operator!= (const Value&) const;
  bool operator>  (const Value&) const;
  bool operator>= (const Value&) const;
  bool operator<  (const Value&) const;
  bool operator<= (const Value&) const;
};

}
