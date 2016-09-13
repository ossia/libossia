#pragma once
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

  constexpr Float(): value{} { }

  constexpr Float(float v) : value(v) { }
  constexpr Float(double v) : value(float(v)) { }
  constexpr Float(int32_t v) : value(float(v)) { }
  constexpr Float(int64_t v) : value(float(v)) { }
  constexpr Float(uint32_t v) : value(float(v)) { }
  constexpr Float(uint64_t v) : value(float(v)) { }

  constexpr Float(const Float&) = default;
  constexpr Float(Float&&) = default;
  constexpr Float& operator=(const Float&) = default;
  constexpr Float& operator=(Float&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
