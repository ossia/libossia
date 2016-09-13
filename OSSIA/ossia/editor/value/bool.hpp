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
  constexpr Bool(bool v = false) : value(v)
  {
  }

  constexpr Bool(const Bool&) = default;
  constexpr Bool(Bool&&) = default;
  constexpr Bool& operator=(const Bool&) = default;
  constexpr Bool& operator=(Bool&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
