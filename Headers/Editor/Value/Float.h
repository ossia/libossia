#pragma once
#include <Editor/Value/ValueBase.h>

namespace OSSIA
{

/*! \details Float value */
struct OSSIA_EXPORT Float final : public Value
{
  float value;

  /*! constructor
   \param float value */
  Float(float v = 0.) :
      Value{Type::FLOAT},
      value(v)
  {
  }

  virtual ~Float();
  Float(const Float&) = default;
  Float(Float&&) = default;
  Float& operator=(const Float&) = default;
  Float& operator=(Float&&) = default;


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
