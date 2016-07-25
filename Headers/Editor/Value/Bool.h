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

  Bool(const Bool&) = default;
  Bool(Bool&&) = default;
  Bool& operator=(const Bool&) = default;
  Bool& operator=(Bool&&) = default;

  bool operator== (const Value&) const;
  bool operator!= (const Value&) const;
  bool operator>  (const Value&) const;
  bool operator>= (const Value&) const;
  bool operator<  (const Value&) const;
  bool operator<= (const Value&) const;
};

const Bool False{false};
const Bool True{true};

}
