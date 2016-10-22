#pragma once
#include <ossia/detail/config.hpp>
#include <cinttypes>
#include <ossia_export.h>

namespace ossia
{
class value;
using Int = int32_t;
/**
 * @class Int int.hpp ossia/editor/value/value.hpp
 */
//struct OSSIA_EXPORT Int
//{
//  int value;

//  operator const int&() const { return value; }
//  operator int&() { return value; }

//  constexpr Int(int v = 0) : value(v)
//  {
//  }

//  OSSIA_DECL_RELAXED_CONSTEXPR Int(double v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(double v) { value = v; return *this;  }
//  OSSIA_DECL_RELAXED_CONSTEXPR Int(float v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(float v) { value = v; return *this;  }
//  OSSIA_DECL_RELAXED_CONSTEXPR Int(char v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(char v) { value = v; return *this;  }
//  OSSIA_DECL_RELAXED_CONSTEXPR Int(bool v) : value(v) { }
//  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(bool v) { value = v; return *this;  }

//  template <typename T>
//  OSSIA_DECL_RELAXED_CONSTEXPR Int(T*) = delete;

//  OSSIA_DECL_RELAXED_CONSTEXPR Int(const Int&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Int(Int&&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(const Int&) = default;
//  OSSIA_DECL_RELAXED_CONSTEXPR Int& operator=(Int&&) = default;

//  bool operator==(const ossia::value&) const;
//  bool operator!=(const ossia::value&) const;
//  bool operator>(const ossia::value&) const;
//  bool operator>=(const ossia::value&) const;
//  bool operator<(const ossia::value&) const;
//  bool operator<=(const ossia::value&) const;
//};
}
