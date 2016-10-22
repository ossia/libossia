#pragma once
#include <ossia/detail/config.hpp>
#include <ossia_export.h>
#include <cinttypes>

namespace ossia
{
class value;
using Float = float;
///**
// * @class Float float.hpp ossia/editor/value/value.hpp
// */
//struct OSSIA_EXPORT Float
//{
//  float value;

//  operator const float&() const { return value; }
//  operator float&() { return value; }

//  OSSIA_DECL_RELAXED_CONSTEXPR Float& operator=(int v) { value = v; return *this;  }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(char v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float& operator=(char v) { value = v; return *this;  }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(bool v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float& operator=(bool v) { value = v; return *this;  }

//  OSSIA_DECL_RELAXED_CONSTEXPR Float(): value{} { }

//  OSSIA_DECL_RELAXED_CONSTEXPR Float(float v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(double v) : value(float(v)) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(int32_t v) : value(float(v)) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(int64_t v) : value(float(v)) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(uint32_t v) : value(float(v)) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(uint64_t v) : value(float(v)) { }

//  OSSIA_DECL_RELAXED_CONSTEXPR Float(const Float&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Float(Float&&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Float& operator=(const Float&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Float& operator=(Float&&) = default;

//  bool operator==(const ossia::value&) const;
//  bool operator!=(const ossia::value&) const;
//  bool operator>(const ossia::value&) const;
//  bool operator>=(const ossia::value&) const;
//  bool operator<(const ossia::value&) const;
//  bool operator<=(const ossia::value&) const;
//};
}
