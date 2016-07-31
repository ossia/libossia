#pragma once
#include <cmath>
namespace ossia
{
const constexpr double pi = 3.1415926535897932384626;
const constexpr double two_pi = 2. * pi;
const constexpr double half_pi = pi / 2.;

// http://stackoverflow.com/a/16659263/1495627
template <class T>
T clamp(T d, T min, T max)
{
  const T t = d < min ? min : d;
  return t > max ? max : t;
}

// Wrap & Fold code taken from Jamoma TTLimits.h.
// Credits : Nils Peters, Nov. 2008
template <class T>
T wrap(T val, const T low, const T high)
{
  if ((val >= low) && (val < high))
    return val;
  else if (val - low >= 0)
    return std::fmod(val - low, std::fabs(low - high)) + low;
  else
    return -1.0 * std::fmod(-1.0 * (val - low), std::fabs(low - high)) + high;
}

template <class T>
T fold(T val, const T low, const T high)
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
