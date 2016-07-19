#pragma once
#include <Editor/Value/ValueBase.h>

namespace OSSIA
{

/*! \details Impulse value */
struct OSSIA_EXPORT Impulse final : public Value
{
  /*! constructor */
    Impulse():
        Value{Type::IMPULSE}
    {
    }

  virtual ~Impulse();
  Impulse(const Impulse&) = delete;

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override { return true; }

  /*! different operator */
  bool operator!= (const Value&) const override { return false; }

  /*! greater than operator */
  bool operator> (const Value&) const override { return false; }

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override { return true; }

  /*! less than operator */
  bool operator< (const Value&) const override { return false; }

  /*! less than and equal operator */
  bool operator<= (const Value&) const override { return true; }
};

}
