#pragma once
#include <Editor/Value/ValueBase.h>

namespace OSSIA
{

/*! \details Bool value */
struct OSSIA_EXPORT Bool final : public Value
{
  bool value;

  /*! constructor */
  Bool(bool v = false) :
      Value{Type::BOOL},
      value(v)
  {
  }

  virtual ~Bool();
  Bool(const Bool&) = default;
  Bool(Bool&&) = default;
  Bool& operator=(const Bool&) = default;
  Bool& operator=(Bool&&) = default;

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

const Bool False{false};
const Bool True{true};

}
