#pragma once
#include <cmath>
#include <boost/math/constants/constants.hpp>
#include <ossia/detail/config.hpp>
/**
 * \file math.hpp
 *
 * This file contains various mathematical constants and utilities
 */

namespace ossia
{
const OSSIA_DECL_RELAXED_CONSTEXPR auto pi = boost::math::constants::pi<double>();
const OSSIA_DECL_RELAXED_CONSTEXPR auto two_pi = boost::math::constants::two_pi<double>();
const OSSIA_DECL_RELAXED_CONSTEXPR auto half_pi = boost::math::constants::half_pi<double>();
const OSSIA_DECL_RELAXED_CONSTEXPR auto rad_to_deg = boost::math::constants::radian<double>();
const OSSIA_DECL_RELAXED_CONSTEXPR auto deg_to_rad = boost::math::constants::degree<double>();
const OSSIA_DECL_RELAXED_CONSTEXPR auto ln_2 = boost::math::constants::ln_two<double>();

/**
 * @brief clamp_min Returns the value bounded by a min
 */
template <class T>
OSSIA_DECL_RELAXED_CONSTEXPR T clamp_min(T d, const T min)
{
  return d < min ? min : d;
}

/**
 * @brief clamp_max Returns the value bounded by a max
 */
template <class T>
OSSIA_DECL_RELAXED_CONSTEXPR T clamp_max(T d, const T max)
{
  return d > max ? max : d;
}

/**
 * @brief clamp Returns the value bounded by a min and a max
 */
template <class T>
OSSIA_DECL_RELAXED_CONSTEXPR T clamp(T d, const T min, const T max)
{
  return clamp_min(clamp_max(d, max), min);
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
