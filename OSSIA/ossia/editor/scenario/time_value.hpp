#pragma once
#include <ossia/detail/config.hpp>
#if defined(__APPLE__)
#include <mach/time_value.h>
#endif
#include <cinttypes>
#include <cmath>
#include <limits>
#include <ossia_export.h>

/**
 * \file time_value.hpp
 */
namespace ossia
{
/**
 * @brief The time_value class
 *
 * Represents an amount of time.
 * The unit is context-dependent.
 */
struct OSSIA_EXPORT time_value
{
  static const constexpr auto infinity = std::numeric_limits<int64_t>::max();

  time_value& operator=(double d) noexcept
  {
    impl = d;
    return *this;
  }

  /*! self addition operator */
  time_value& operator+=(double d) noexcept
  {
    if (infinite())
      impl = 0;
    else
      impl += d;

    return *this;
  }

  time_value& operator+=(ossia::time_value t) noexcept
  {
    if (infinite() || t.infinite())
      impl = 0;
    else
      impl += t.impl;

    return *this;
  }

  /*! self substraction operator */
  time_value& operator-=(double d) noexcept
  {
    if (infinite())
      impl = 0;
    else
      impl -= d;

    return *this;
  }

  time_value& operator-=(ossia::time_value t) noexcept
  {
    if (infinite() || t.infinite())
      impl = 0;
    else
      impl -= t.impl;

    return *this;
  }

  /*! addition operator */
  constexpr time_value operator+(double d) const noexcept
  {
    return time_value{int64_t(impl + d)};
  }
  constexpr time_value operator+(int64_t d) const noexcept
  {
    return time_value{impl + d};
  }
  constexpr time_value operator+(uint64_t d) const noexcept
  {
    return time_value{int64_t(impl + d)};
  }
  constexpr time_value operator-(int64_t d) const noexcept
  {
    return time_value{impl - d};
  }
  constexpr time_value operator-(uint64_t d) const noexcept
  {
    return time_value{int64_t(impl - d)};
  }

  time_value operator+(ossia::time_value t) const noexcept
  {
    if (infinite() || t.infinite())
    {
      return time_value{infinity};
    }

    return time_value{impl + t.impl};
  }

  /*! substraction operator */
  constexpr time_value operator-(double d) const noexcept
  {
    return time_value{int64_t(impl - d)};
  }

  time_value operator-(ossia::time_value t) const noexcept
  {
    if (infinite() || t.infinite())
    {
      return time_value{infinity};
    }

    return time_value{impl - t.impl};
  }

  /*! multiplication operator */
  constexpr time_value operator*(float d) const noexcept
  {
    return time_value{int64_t(impl * d)};
  }

  constexpr time_value operator*(double d) const noexcept
  {
    return time_value{int64_t(impl * d)};
  }

  constexpr time_value operator*(int32_t d) const noexcept
  {
    return time_value{impl * d};
  }

  constexpr time_value operator*(int64_t d) const noexcept
  {
    return time_value{impl * d};
  }

  constexpr time_value operator*(uint32_t d) const noexcept
  {
    return time_value{impl * d};
  }

  constexpr time_value operator*(uint64_t d) const noexcept
  {
    return time_value{int64_t(impl * d)};
  }

  friend double operator/(time_value lhs, time_value rhs) {
    return double(lhs.impl) / double(rhs.impl);
  }
  constexpr operator int64_t() const noexcept
  {
    return impl;
  }

  /*! is the time value infinite ?
   \return bool infinite */
  bool infinite() const noexcept
  {
    //! \todo this prevents ever compiling with -Ofast
    return std::numeric_limits<int64_t>::max() == impl;
  }

  bool operator==(ossia::time_value rhs) const noexcept
  {
    return impl == rhs.impl;
  }
  bool operator!=(ossia::time_value rhs) const noexcept
  {
    return impl != rhs.impl;
  }

  int64_t impl;
};

OSSIA_EXPORT inline time_value operator"" _tv(long double v)
{
  return time_value{int64_t(v)};
}

OSSIA_EXPORT inline time_value operator"" _tv(unsigned long long v)
{
  return time_value{(int64_t)v};
}

const constexpr time_value Infinite{time_value::infinity};
const constexpr time_value Zero{0};
const constexpr time_value One{1};

OSSIA_EXPORT inline time_value abs(time_value t) {
  return time_value{t.impl >= 0 ? t.impl : -t.impl};
}

OSSIA_EXPORT inline time_value norm(time_value t1, time_value t2) {
  if(t1.infinite() || t2.infinite())
    return Infinite;
  return time_value{t1.impl > t2.impl ? t1.impl - t2.impl : t2.impl - t1.impl};
}
}

static_assert(std::is_pod<ossia::time_value>::value, "bug introduced somewhere");
