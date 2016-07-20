#pragma once
#include <Editor/Value/ValueBase.h>

namespace OSSIA
{

/*! \details Int value */
struct OSSIA_EXPORT Int final : public Value
{
  int value;

  /*! constructor
   \param int value */
  Int(int v = 0) :
      Value{Type::INT},
      value(v)
  {
  }
  virtual ~Int();
  Int(const Int&) = default;
  Int(Int&&) = default;
  Int& operator=(const Int&) = default;
  Int& operator=(Int&&) = default;

  /*! equal operator */
  bool operator== (const SafeValue&) const override;

  /*! different operator */
  bool operator!= (const SafeValue&) const override;

  /*! greater than operator */
  bool operator> (const SafeValue&) const override;

  /*! greater than and equal operator */
  bool operator>= (const SafeValue&) const override;

  /*! less than operator */
  bool operator< (const SafeValue&) const override;

  /*! less than and equal operator */
  bool operator<= (const SafeValue&) const override;
};

}
