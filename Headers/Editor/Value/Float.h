#pragma once
#include <Editor/Value/ValueBase.h>

namespace OSSIA
{

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
  bool operator== (const SafeValue&) const;

  /*! different operator */
  bool operator!= (const SafeValue&) const;

  /*! greater than operator */
  bool operator> (const SafeValue&) const;

  /*! greater than and equal operator */
  bool operator>= (const SafeValue&) const;

  /*! less than operator */
  bool operator< (const SafeValue&) const;

  /*! less than and equal operator */
  bool operator<= (const SafeValue&) const;
};

}
