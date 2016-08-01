#pragma once
#include <ossia_export.h>

namespace ossia
{
class value;

/**
 * @class Float float.hpp ossia/editor/value/value.hpp
 */
struct OSSIA_EXPORT Float
{
  float value;

  Float(float v = 0.) : value(v)
  {
  }

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
