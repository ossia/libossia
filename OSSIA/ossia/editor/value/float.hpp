#pragma once
#include <ossia_export.h>

namespace ossia
{
class value;
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

  bool operator== (const ossia::value&) const;
  bool operator!= (const ossia::value&) const;
  bool operator>  (const ossia::value&) const;
  bool operator>= (const ossia::value&) const;
  bool operator<  (const ossia::value&) const;
  bool operator<= (const ossia::value&) const;
};

}
