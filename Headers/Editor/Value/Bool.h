#pragma once
#include <ossia_export.h>

namespace OSSIA
{
class SafeValue;
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

const Bool False{false};
const Bool True{true};

}
