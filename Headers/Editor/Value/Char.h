#pragma once
#include <Editor/Value/ValueBase.h>

namespace OSSIA
{

/*! \details Char value */
struct OSSIA_EXPORT Char final : public Value
{
  char value;

  /*! constructor
   \param char value */
  Char(char v = 0x00) :
      Value{Type::CHAR},
      value(v)
  {
  }
  virtual ~Char();
  Char(const Char&) = default;
  Char(Char&&) = default;
  Char& operator=(const Char&) = default;
  Char& operator=(Char&&) = default;


  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

}
