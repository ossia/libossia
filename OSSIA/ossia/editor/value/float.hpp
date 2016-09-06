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

  Float(): value{} { }

  Float(float v) : value(v) { }
  Float(double v) : value(float(v)) { }
  Float(int32_t v) : value(float(v)) { }
  Float(int64_t v) : value(float(v)) { }
  Float(uint32_t v) : value(float(v)) { }
  Float(uint64_t v) : value(float(v)) { }

  Float(const Float&) = default;
  Float(Float&&) = default;
  Float& operator=(const Float&) = default;
  Float& operator=(Float&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
