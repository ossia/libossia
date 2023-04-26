#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/math.hpp>

#include <cmath>

#include <string_view>

/**
  \file easing.hpp
  this file contains the implementation of various easing functions, to
  use in \ref automation or \ref mapper.

  Easing functions
  by Kristoffer Gronlund, 2014
  Public domain
  This work is a spiritual descendent (not to say derivative work) of works
  done
  by the following individuals:
  Warren Moore (https://github.com/warrenm)
  Robert Penner (http://www.robertpenner.com/easing/)
  George McGinley Smith (http://gsgd.co.uk/sandbox/jquery/easing/)
  James Padolsey (http://james.padolsey.com/demos/jquery/easing/)
  Authors of jQuery (http://plugins.jquery.com/project/Easing)
  Matt Gallagher
  (http://cocoawithlove.com/2008/09/parametric-acceleration-curves-in-core.html)
  Jesse Crossen
  (http://stackoverflow.com/questions/5161465/how-to-create-custom-easing-function-with-core-animation)
*/

namespace ossia
{
namespace easing
{

struct ease
{
  template <typename T, typename U, typename V>
  constexpr T operator()(T a, U b, V t) const noexcept
  {
#if defined(FP_FAST_FMA)
    return ossia::fma(t, b, ossia::fma(-t, a, a));
#else
    return (static_cast<T>(1) - t) * a + t * b;
#endif
  }
};

struct linear
{
  static consteval std::string_view name() noexcept { return "linear"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return t;
  }
};

template <typename V>
struct power
{
  static consteval std::string_view name() noexcept { return "power"; }

  static constexpr V linear_gamma = 1.0;
  V gamma = linear_gamma;

  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return std::pow(t, gamma);
  }
};

struct quadraticIn
{
  static consteval std::string_view name() noexcept { return "quadraticIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t, 2);
  }
};

struct quadraticOut
{
  static consteval std::string_view name() noexcept { return "quadraticOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return -(t * (t - 2.));
  }
};

struct quadraticInOut
{
  static consteval std::string_view name() noexcept { return "quadraticInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t < 0.5) ? 2. * t * t : (-2. * t * t) + (4. * t) - 1.;
  }
};

struct cubicIn
{
  static consteval std::string_view name() noexcept { return "cubicIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t, 3);
  }
};

struct cubicOut
{
  static consteval std::string_view name() noexcept { return "cubicOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t - 1., 3) + 1.;
  }
};

struct cubicInOut
{
  static consteval std::string_view name() noexcept { return "cubicInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t < T(0.5)) ? 4. * ipow(t, 3) : 0.5 * ipow((2. * t) - 2, 3) + 1.;
  }
};

struct quarticIn
{
  static consteval std::string_view name() noexcept { return "quarticIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t, 4);
  }
};

struct quarticOut
{
  static consteval std::string_view name() noexcept { return "quarticOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t - 1., 3) * (1. - t) + 1.;
  }
};

struct quarticInOut
{
  static consteval std::string_view name() noexcept { return "quarticInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t < 0.5) ? 8. * ipow(t, 4) : -8. * ipow(t - 1., 4) + 1.;
  }
};

struct quinticIn
{
  static consteval std::string_view name() noexcept { return "quinticIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t, 5);
  }
};

struct quinticOut
{
  static consteval std::string_view name() noexcept { return "quinticOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t - 1., 5) + 1.;
  }
};

struct quinticInOut
{
  static consteval std::string_view name() noexcept { return "quinticInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t < 0.5) ? 16. * ipow(t, 5) : 0.5 * ipow((2. * t) - 2., 5) + 1.;
  }
};

struct sineIn
{
  static consteval std::string_view name() noexcept { return "sineIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return std::sin((t - 1.) * half_pi) + 1.;
  }
};

struct sineOut
{
  static consteval std::string_view name() noexcept { return "sineOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return std::sin(t * half_pi);
  }
};

struct sineInOut
{
  static consteval std::string_view name() noexcept { return "sineInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return 0.5 * (1. - std::cos(t * pi));
  }
};

struct circularIn
{
  static consteval std::string_view name() noexcept { return "circularIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return 1. - std::sqrt(1. - (t * t));
  }
};

struct circularOut
{
  static consteval std::string_view name() noexcept { return "circularOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return std::sqrt((2. - t) * t);
  }
};

struct circularInOut
{
  static consteval std::string_view name() noexcept { return "circularInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t < 0.5) ? 0.5 * (1 - std::sqrt(1 - 4. * (t * t)))
                     : 0.5 * (std::sqrt(-((2. * t) - 3.) * ((2. * t) - 1.)) + 1.);
  }
};

struct exponentialIn
{
  static consteval std::string_view name() noexcept { return "exponentialIn"; }
  template <typename T>
  constexpr T operator()(T t) const
  {
    return (t <= 0) ? t : ossia::exp2(10. * (t - 1.));
  }
};

struct exponentialOut
{
  static consteval std::string_view name() noexcept { return "exponentialOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t >= 1.) ? t : 1. - ossia::exp2(-10. * t);
  }
};

struct exponentialInOut
{
  static consteval std::string_view name() noexcept { return "exponentialInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t <= 0. || t >= 1.) ? t
           : (t < 0.5)          ? 0.5 * ossia::exp2((20. * t) - 10.)
                                : -0.5 * ossia::exp2((-20. * t) + 10.) + 1.;
  }
};

struct elasticIn
{
  static consteval std::string_view name() noexcept { return "elasticIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return std::sin(13. * half_pi * t) * ossia::exp2(10. * (t - 1.));
  }
};

struct elasticOut
{
  static consteval std::string_view name() noexcept { return "elasticOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return sin(-13. * half_pi * (t + 1.)) * ossia::exp2(-10. * t) + 1.;
  }
};

struct elasticInOut
{
  static consteval std::string_view name() noexcept { return "elasticInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t < 0.5) ? 0.5 * std::sin(13. * half_pi * (2. * t))
                           * ossia::exp2(10. * ((2. * t) - 1.))
                     : 0.5
                           * (std::sin(-13. * half_pi * ((2. * t - 1) + 1))
                                  * ossia::exp2(-10. * (2. * t - 1.))
                              + 2.);
  }
};

struct backIn
{
  static consteval std::string_view name() noexcept { return "backIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return ipow(t, 3) - t * std::sin(t * pi);
  }
};

struct backOut
{
  static consteval std::string_view name() noexcept { return "backOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return 1. - (ipow(1. - t, 3) - (1. - t) * std::sin((1. - t) * pi));
  }
};

struct backInOut
{
  static consteval std::string_view name() noexcept { return "backInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return (t < 0.5) ? 0.5 * (ipow(2. * t, 3) - (2. * t) * std::sin((2. * t) * pi))
                     : 0.5
                               * (1.
                                  - (ipow(1. - (2. * t - 1.), 3)
                                     - (1. - (2. * t - 1.))
                                           * std::sin((1. - (2. * t - 1.)) * pi)))
                           + 0.5;
  }
};

struct bounceOut
{
  static consteval std::string_view name() noexcept { return "bounceOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return t < 4. / 11.     ? (121. * t * t) / 16.
           : (t < 8. / 11.) ? (363. / 40. * t * t) - (99 / 10. * t) + 17 / 5.
           : (t < 9. / 10.)
               ? (4356. / 361. * t * t) - (35442. / 1805. * t) + 16061. / 1805.
               : (54. / 5. * t * t) - (513. / 25. * t) + 268. / 25.;
  }
};

struct bounceIn
{
  static consteval std::string_view name() noexcept { return "bounceIn"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return 1. - bounceOut{}(1. - t);
  }
};

struct bounceInOut
{
  static consteval std::string_view name() noexcept { return "bounceInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return t < 0.5 ? 0.5 * bounceIn{}(t * 2.) : 0.5 * bounceOut{}(t * 2. - 1.) + 0.5;
  }
};

struct perlinInOut
{
  static consteval std::string_view name() noexcept { return "perlinInOut"; }
  template <typename T>
  constexpr T operator()(T t) const noexcept
  {
    return 6. * ipow(t, 5) - 15. * ipow(t, 4) + 10. * ipow(t, 3);
  }
};
}

template <typename Y, typename Easing>
struct curve_segment_ease
{
  constexpr Y operator()(double ratio, Y start, Y end) const noexcept
  {
    return easing::ease{}(start, end, Easing{}(ratio));
  }
};

}
