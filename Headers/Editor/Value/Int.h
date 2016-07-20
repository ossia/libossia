#pragma once
#include <Editor/Value/ValueBase.h>
#include <ossia_export.h>

namespace OSSIA
{

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
