#pragma once
#include <ossia/detail/config.hpp>
#include <ossia_export.h>
#include <cinttypes>

namespace ossia
{
class value;

/**
 * @class Float float.hpp ossia/editor/value/value.hpp
 */
struct OSSIA_EXPORT Float
{
  float value;

  OSSIA_DECL_RELAXED_CONSTEXPR Float(): value{} { }

  OSSIA_DECL_RELAXED_CONSTEXPR Float(float v) : value(v) { }
  OSSIA_DECL_RELAXED_CONSTEXPR Float(double v) : value(float(v)) { }
  OSSIA_DECL_RELAXED_CONSTEXPR Float(int32_t v) : value(float(v)) { }
  OSSIA_DECL_RELAXED_CONSTEXPR Float(int64_t v) : value(float(v)) { }
  OSSIA_DECL_RELAXED_CONSTEXPR Float(uint32_t v) : value(float(v)) { }
  OSSIA_DECL_RELAXED_CONSTEXPR Float(uint64_t v) : value(float(v)) { }

  OSSIA_DECL_RELAXED_CONSTEXPR Float(const Float&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Float(Float&&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Float& operator=(const Float&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Float& operator=(Float&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
