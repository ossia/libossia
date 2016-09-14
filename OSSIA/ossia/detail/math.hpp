#pragma once
#include <cmath>
#include <boost/math/constants/constants.hpp>

#if defined(_MSC_VER)
#define OSSIA_DECL_RELAXED_CONSTEXPR
#else
#define OSSIA_DECL_RELAXED_CONSTEXPR constexpr
#endif
/**
 * \file math.hpp
 *
 * This file contains various mathematical constants and utilities
 */

namespace ossia
{
const constexpr auto pi = boost::math::constants::pi<double>();
const constexpr auto two_pi = boost::math::constants::two_pi<double>();
const constexpr auto half_pi = boost::math::constants::half_pi<double>();
const constexpr auto rad_to_deg = boost::math::constants::radian<double>();
const constexpr auto deg_to_rad = boost::math::constants::degree<double>();

// http://stackoverflow.com/a/16659263/1495627
template <class T>
OSSIA_DECL_RELAXED_CONSTEXPR T clamp(T d, T min, T max)
{
  const T t = d < min ? min : d;
  return t > max ? max : t;
}

template <class T>
constexpr T clamp_min(T d, T min)
{
  return d < min ? min : d;
}

template <class T>
constexpr T clamp_max(T d, T max)
{
  return d > max ? max : d;
}

// Wrap & Fold code taken from Jamoma TTLimits.h.
// Credits : Nils Peters, Nov. 2008
template <class T>
OSSIA_DECL_RELAXED_CONSTEXPR T wrap(T val, const T low, const T high)
{
  if ((val >= low) && (val < high))
    return val;
  else if (val - low >= 0)
    return std::fmod(val - low, std::fabs(low - high)) + low;
  else
    return -1.0 * std::fmod(-1.0 * (val - low), std::fabs(low - high)) + high;
}

template <class T>
OSSIA_DECL_RELAXED_CONSTEXPR T fold(T val, const T low, const T high)
{
  if ((val >= low) && (val <= high))
    return val;
  else
  {
    return std::fabs(std::remainder(val - low, 2. * std::fabs(low - high)))
           + low;
  }
}
}
