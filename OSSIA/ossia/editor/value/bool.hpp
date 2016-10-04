#pragma once
#include <ossia/detail/config.hpp>
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
  OSSIA_DECL_RELAXED_CONSTEXPR Bool(bool v = false) : value(v)
  {
  }

  OSSIA_DECL_RELAXED_CONSTEXPR Bool(const Bool&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Bool(Bool&&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(const Bool&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(Bool&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
