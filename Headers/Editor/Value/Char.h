#pragma once
#include <ossia_export.h>

namespace OSSIA
{
class Value;
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
