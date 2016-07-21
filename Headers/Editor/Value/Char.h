#pragma once
#include <ossia_export.h>

namespace OSSIA
{
class SafeValue;
/*! \details Char value */
struct OSSIA_EXPORT Char
{
  char value;

  /*! constructor
   \param char value */
  Char(char v = 0x00) :
      value(v)
  {
  }
  virtual ~Char();
  Char(const Char&) = default;
  Char(Char&&) = default;
  Char& operator=(const Char&) = default;
  Char& operator=(Char&&) = default;


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
