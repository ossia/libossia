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

  Impulse(const Impulse&) = default;
  Impulse(Impulse&&) = default;
  Impulse& operator=(const Impulse&) = default;
  Impulse& operator=(Impulse&&) = default;

  virtual ~Impulse();

  /*! equal operator */
  bool operator== (const SafeValue&) const override
  { return true; }

  /*! different operator */
  bool operator!= (const SafeValue&) const override
  { return false; }

  /*! greater than operator */
  bool operator> (const SafeValue&) const override
  { return false; }

  /*! greater than and equal operator */
  bool operator>= (const SafeValue&) const override
  { return true; }

  /*! less than operator */
  bool operator< (const SafeValue&) const override
  { return false; }

  /*! less than and equal operator */
  bool operator<= (const SafeValue&) const override
  { return true; }
};

}
