#pragma once
#include <ossia/detail/config.hpp>
#include <ossia_export.h>

namespace ossia
{
class value;
using Bool = bool;
///**
// * @class Bool bool.hpp ossia/editor/value/value.hpp
// */
//struct OSSIA_EXPORT Bool
//{
//  bool value;

//  /*! constructor */
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool(bool v = false) : value(v)
//  {
//  }

//  OSSIA_DECL_RELAXED_CONSTEXPR Bool(double v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(double v) { value = v; return *this; }
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool(int v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(int v) { value = v; return *this;  }
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool(char v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(char v) { value = v; return *this;  }
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool(float v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(float v) { value = v; return *this;  }

//  operator const bool&() const { return value; }
//  operator bool&() { return value; }

//  OSSIA_DECL_RELAXED_CONSTEXPR Bool(const Bool&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool(Bool&&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(const Bool&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Bool& operator=(Bool&&) = default;

//  bool operator==(const ossia::value&) const;
//  bool operator!=(const ossia::value&) const;
//  bool operator>(const ossia::value&) const;
//  bool operator>=(const ossia::value&) const;
//  bool operator<(const ossia::value&) const;
//  bool operator<=(const ossia::value&) const;
//};
}
