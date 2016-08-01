#pragma once
#include <ossia_export.h>

namespace ossia
{
class value;

/**
 * @class Bool bool.hpp ossia/editor/value/value.hpp
 */
struct OSSIA_EXPORT Bool
{
  bool value;

  /*! constructor */
  Bool(bool v = false) : value(v)
  {
  }

  Bool(const Bool&) = default;
  Bool(Bool&&) = default;
  Bool& operator=(const Bool&) = default;
  Bool& operator=(Bool&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
