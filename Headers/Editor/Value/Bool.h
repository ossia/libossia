#pragma once
#include <ossia_export.h>

namespace OSSIA
{
class Value;
/*! \details Bool value */
struct OSSIA_EXPORT Bool
{
  bool value;

  /*! constructor */
  Bool(bool v = false) :
      value(v)
  {
  }

  virtual ~Bool();
  Bool(const Bool&) = default;
  Bool(Bool&&) = default;
  Bool& operator=(const Bool&) = default;
  Bool& operator=(Bool&&) = default;

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

const Bool False{false};
const Bool True{true};

}
