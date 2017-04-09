#pragma once
#include <cmath>
#include <cinttypes>
#include <ossia_export.h>

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
  explicit constexpr time_value(double d) noexcept :
    impl{d}
  {
  }

  explicit constexpr time_value() noexcept :
    impl{0.}
  {
  }

  time_value& operator=(double d) noexcept
  {
    impl = d;
    return *this;
  }

  /*! self addition operator */
  time_value& operator+=(double d) noexcept
  {
    if (isInfinite())
      impl = 0.;
    else
      impl += d;

    return *this;
  }

  time_value& operator+=(time_value t) noexcept
  {
    if (isInfinite() || t.isInfinite())
      impl = 0.;
    else
      impl += t.impl;

    return *this;
  }

  /*! self substraction operator */
  time_value& operator-=(double d) noexcept
  {
    if (isInfinite())
      impl = 0.;
    else
      impl -= d;

    return *this;
  }

  time_value& operator-=(time_value t) noexcept
  {
    if (isInfinite() || t.isInfinite())
      impl = 0.;
    else
      impl -= t.impl;

    return *this;
  }

  /*! addition operator */
  constexpr time_value operator+(double d) const noexcept
  {
    return time_value(impl + d);
  }

  time_value operator+(time_value t) const noexcept
  {
    if (isInfinite() || t.isInfinite())
    {
      return time_value(INFINITY);
    }

    return time_value(impl + t.impl);
  }

  /*! substraction operator */
  constexpr time_value operator-(double d) const noexcept
  {
    return time_value(impl - d);
  }

  time_value operator-(time_value t) const noexcept
  {
    if (isInfinite() || t.isInfinite())
    {
      return time_value(INFINITY);
    }

    return time_value(impl - t.impl);
  }

  /*! multiplication operator */
  constexpr time_value operator*(float d) const noexcept
  { return time_value(impl * d); }

  constexpr time_value operator*(double d) const noexcept
  { return time_value(impl * d); }

  constexpr time_value operator*(int32_t d) const noexcept
  { return time_value(impl * d); }

  constexpr time_value operator*(int64_t d) const noexcept
  { return time_value(impl * d); }

  constexpr time_value operator*(uint32_t d) const noexcept
  { return time_value(impl * d); }

  constexpr time_value operator*(uint64_t d) const noexcept
  { return time_value(impl * d); }


  /*! double casting operator */
  constexpr operator double() const noexcept
  {
    return impl;
  }

  /*! is the time value infinite ?
   \return bool infinite */
  bool isInfinite() const noexcept
  {
    return std::isinf(impl);
  }

  bool operator==(time_value rhs) const noexcept { return impl == rhs.impl; }
  bool operator!=(time_value rhs) const noexcept { return impl != rhs.impl; }

  double impl;
};

OSSIA_EXPORT inline time_value operator "" _tv(long double v)
{
    return time_value(v);
}

OSSIA_EXPORT inline time_value operator "" _tv(unsigned long long v)
{
    return time_value(v);
}

const constexpr time_value Infinite{INFINITY};
const constexpr time_value Zero{0.};
const constexpr time_value One{1.};

}
