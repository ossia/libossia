#pragma once
#include <ossia/detail/config.hpp>

#include <cmath>

#include <utility>
/**
 * \file math.hpp
 *
 * This file contains various mathematical constants and utilities
 */

namespace ossia
{
#if defined(__ANDROID_API__)
using ::asinh;
using ::exp2;
using ::fma;
using ::llround;
using ::log1p;
using ::remainder;
#else
using std::asinh;
using std::exp2;
using std::fma;
using std::llround;
using std::log1p;
using std::remainder;
#endif
const constexpr auto pi = 3.141592653589793238462643383279502884;
const constexpr auto two_pi = 6.283185307179586476925286766559005768;
const constexpr auto half_pi = 1.570796326794896619231321691639751442;
const constexpr auto rad_to_deg = 5.729577951308232087679815481410517033e+01;
const constexpr auto deg_to_rad = 1.745329251994329576923690768488612713e-02;
const constexpr auto ln_2 = 6.931471805599453094172321214581765680e-01;

template <typename T>
T ipow(T x, int N) noexcept
{
  T res = x;
  for (int i = 1; i < N; i++)
  {
    res *= x;
  }
  return res;
}

template <typename T>
auto norm(T x, T y) noexcept
{
  return std::sqrt(ipow(x, 2) + ipow(y, 2));
}

template <typename T>
auto norm(T x, T y, T z) noexcept
{
  return std::sqrt(ipow(x, 2) + ipow(y, 2) + ipow(z, 2));
}

inline constexpr size_t constexpr_log2(size_t n) noexcept
{
  return ((n < 2) ? 0 : 1 + constexpr_log2(n / 2));
}

/**
 * @brief clamp_min Returns the value bounded by a min
 */
template <class T>
OSSIA_INLINE constexpr T clamp_min(T d, const T min) noexcept
{
  return d < min ? min : d;
}

/**
 * @brief clamp_max Returns the value bounded by a max
 */
template <class T>
OSSIA_INLINE constexpr T clamp_max(T d, const T max) noexcept
{
  return d > max ? max : d;
}

/**
 * @brief clamp Returns the value bounded by a min and a max
 */
template <class T>
OSSIA_INLINE constexpr T clamp(T d, const T min, const T max) noexcept
{
  return clamp_min(clamp_max(d, max), min);
}

// Wrap & Fold code taken from Jamoma TTLimits.h.
// Credits : Nils Peters, Nov. 2008
template <class T>
OSSIA_INLINE constexpr T wrap(const T val, const T low, const T high) noexcept
{
  if ((val >= low) && (val < high))
    return val;
  else if (val >= low)
    return low + std::fmod(val - low, std::fabs(low - high));
  else
    return high - std::fmod(low - val, std::fabs(low - high));
}

template <class T>
OSSIA_INLINE constexpr T fold(const T val, const T low, const T high) noexcept
{
  if ((val >= low) && (val <= high))
    return val;
  else
  {
    return low
           + std::fabs(
                 ossia::remainder(val - low, 2. * std::fabs(low - high)));
  }
}

template <class T>
OSSIA_INLINE constexpr std::pair<T, T> div(T num, T denom) noexcept
{
  return {num / denom, num % denom};
}
}
