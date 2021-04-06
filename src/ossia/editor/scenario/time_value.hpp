#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/flicks.hpp>
#if defined(__APPLE__)
#include <mach/time_value.h>
#endif
#include <ossia_export.h>

#include <cinttypes>
#include <limits>
#include <cmath>
#include <cassert>

/**
 * \file time_value.hpp
 */
namespace ossia
{
using physical_time = int64_t;
/**
 * @brief The time_value class
 *
 * Represents an amount of time.
 * The unit is context-dependent.
 */
struct OSSIA_EXPORT time_value
{
  // infinity is everything beyond 2^60 as this is already a gigantic quantity (~50 years in flicks)
  // we set ~2^62 as the default "infinity" value to allow for some leeway and make sure we won't hit integer overflow in
  // any reasonable cases
  static const constexpr int64_t infinite_min = std::numeric_limits<int64_t>::max() / 8;
  static const constexpr int64_t infinity = std::numeric_limits<int64_t>::max() / 2;

  constexpr time_value& operator=(bool d) noexcept = delete;
  constexpr time_value& operator=(double d) noexcept = delete;
  constexpr time_value& operator=(float d) noexcept = delete;
  constexpr time_value& operator=(uint64_t d) noexcept = delete;

  constexpr time_value& operator=(int64_t d) noexcept
  {
    impl = d;
    return *this;
  }

  /*! self addition operator */
  constexpr time_value& operator+=(double d) noexcept = delete;
  constexpr time_value& operator+=(float d) noexcept = delete;

  constexpr time_value& operator+=(int64_t d) noexcept
  {
    if (infinite())
      impl = 0;
    else
      impl += d;

    return *this;
  }

  constexpr time_value& operator+=(ossia::time_value t) noexcept
  {
    if (infinite() || t.infinite())
      impl = infinity;
    else
      impl += t.impl;

    return *this;
  }

  /*! self substraction operator */
  constexpr time_value& operator-=(double d) noexcept = delete;
  constexpr time_value& operator-=(int64_t d) noexcept
  {
    if (infinite())
      impl = infinity;
    else
      impl -= d;

    return *this;
  }

  constexpr time_value& operator-=(ossia::time_value t) noexcept
  {
    if (infinite() || t.infinite())
      impl = infinity;
    else
      impl -= t.impl;

    return *this;
  }

  constexpr time_value& operator-() noexcept
  {
    if (!infinite())
      impl = -impl;

    return *this;
  }

  /*! addition operator */
  // not constexpr because of isnan
  /* constexpr */ time_value operator+(double d) const noexcept
  {
    assert(!std::isnan(d));
    assert(d < static_cast<double>(infinite_min));
    return *this + time_value{int64_t(d)};
  }
  constexpr time_value operator+(int64_t d) const noexcept
  {
    return *this + time_value{d};
  }
  constexpr time_value operator+(uint64_t d) const noexcept
  {
    assert(d < infinite_min);
    return *this + time_value{int64_t(d)};
  }
  constexpr time_value operator-(int64_t d) const noexcept
  {
    return *this + time_value{-d};
  }
  constexpr time_value operator-(uint64_t d) const noexcept
  {
    assert(d < infinite_min);
    return *this + time_value{-int64_t(d)};
  }

  static constexpr bool add_is_infinite(
      const ossia::time_value& lhs,
      const ossia::time_value& rhs) noexcept
  {
    if (lhs.infinite() || rhs.infinite())
    {
      return true;
    }
    else if (lhs.impl >= 0 && rhs.impl >= 0)
    {
      uint64_t l = lhs.impl;
      uint64_t r = rhs.impl;
      return l + r >= infinite_min;
    }
    else if(lhs.impl >= 0 && rhs.impl < 0)
    {
      uint64_t l = lhs.impl;
      return l + rhs.impl >= infinite_min;
    }
    else if(lhs.impl < 0 && rhs.impl >= 0)
    {
      uint64_t r = rhs.impl;
      return lhs.impl + r >= infinite_min;
    }
    else if(lhs.impl < 0 && rhs.impl < 0)
    {
      // TODO have a better underflow check
      uint64_t l = -lhs.impl;
      uint64_t r = -rhs.impl;
      return l + r >= infinite_min;
    }

    return false;
  }

  static constexpr bool sub_is_infinite(
      const ossia::time_value& lhs,
      const ossia::time_value& rhs) noexcept
  {
    if (lhs.infinite() || rhs.infinite())
    {
      return true;
    }
    else if (lhs.impl >= 0 && rhs.impl >= 0)
    {
      return false;
    }
    else if(lhs.impl >= 0 && rhs.impl < 0)
    {
      uint64_t l = lhs.impl;
      uint64_t r = -rhs.impl;
      return l + r >= infinite_min;
    }
    else if(lhs.impl < 0 && rhs.impl >= 0)
    {
      uint64_t l = -lhs.impl;
      uint64_t r = rhs.impl;
      return l + r >= infinite_min;
    }
    else if(lhs.impl < 0 && rhs.impl < 0)
    {
      return false;
    }

    return false;
  }

  constexpr time_value operator+(ossia::time_value t) const noexcept
  {
    if (add_is_infinite(*this, t))
      return time_value{infinity};

    return time_value{impl + t.impl};
  }

  /*! substraction operator */
  // not constexpr because of isnan
  /* constexpr */ time_value operator-(double d) const noexcept
  {
    assert(!std::isnan(d));
    assert(d < static_cast<double>(infinite_min));
    return *this - time_value{int64_t(d)};
  }

  constexpr time_value operator-(ossia::time_value t) const noexcept
  {
    if (sub_is_infinite(*this, t))
      return time_value{infinity};

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

  friend constexpr double operator/(time_value lhs, time_value rhs) noexcept
  {
    return double(lhs.impl) / double(rhs.impl);
  }

  /*! is the time value infinite ?
   \return bool infinite */
  constexpr bool infinite() const noexcept
  {
    return impl >= infinite_min;
  }
  constexpr time_value operator%(time_value d) const noexcept
  {
    return time_value{impl % d.impl};
  }
  constexpr bool operator==(ossia::time_value rhs) const noexcept
  {
    return (infinite() && rhs.infinite()) || (impl == rhs.impl);
  }
  constexpr bool operator!=(ossia::time_value rhs) const noexcept
  {
    return (infinite() != rhs.infinite()) || (impl != rhs.impl);
  }
  constexpr bool operator<(ossia::time_value rhs) const noexcept
  {
    return !(infinite() && rhs.infinite()) && (impl < rhs.impl);
  }
  constexpr bool operator>(ossia::time_value rhs) const noexcept
  {
    return !(infinite() && rhs.infinite()) && (impl > rhs.impl);
  }
  constexpr bool operator<=(ossia::time_value rhs) const noexcept
  {
    return !(infinite() && rhs.infinite()) && (impl <= rhs.impl);
  }
  constexpr bool operator>=(ossia::time_value rhs) const noexcept
  {
    return !(infinite() && rhs.infinite()) && (impl >= rhs.impl);
  }

  int64_t impl;
};

OSSIA_EXPORT constexpr inline time_value operator"" _tv(long double v)noexcept
{
  return time_value{int64_t(v)};
}

OSSIA_EXPORT constexpr inline time_value operator"" _tv(unsigned long long v) noexcept
{
  return time_value{(int64_t)v};
}

const constexpr time_value Infinite{time_value::infinity};
const constexpr time_value Zero{0};
const constexpr time_value One{1};

OSSIA_EXPORT constexpr inline time_value abs(time_value t) noexcept
{
  return time_value{t.impl >= 0 ? t.impl : -t.impl};
}

OSSIA_EXPORT constexpr inline time_value norm(time_value t1, time_value t2) noexcept
{
  if (t1.infinite() || t2.infinite())
    return Infinite;
  return time_value{t1.impl > t2.impl ? t1.impl - t2.impl : t2.impl - t1.impl};
}

inline constexpr int64_t to_sample(ossia::time_value t, double rate) noexcept
{
  const double samples_per_flicks = rate / ossia::flicks_per_second<double>;
  return (rate > 0 && !t.infinite()) ? std::round(t.impl * samples_per_flicks) : 0;
}

}

// static_assert(std::is_pod<ossia::time_value>::value, "bug introduced
// somewhere");
