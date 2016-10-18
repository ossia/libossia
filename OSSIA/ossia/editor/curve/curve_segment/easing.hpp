#pragma once
#include <ossia/detail/math.hpp>
#include <cmath>
#include <ossia_export.h>

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

template <typename T>
struct ease
{
  constexpr T operator()(T a, T b, T t) const
  {
    return a + (b - a) * t;
  }
};

template <typename T>
struct linear
{
  constexpr T operator()(T t) const
  {
    return t;
  }
};

template <typename T>
struct quadraticIn
{
  constexpr T operator()(T t) const
  {
    return std::pow(t, 2);
  }
};

template <typename T>
struct quadraticOut
{
  constexpr T operator()(T t) const
  {
    return -(t * (t - 2.));
  }
};

template <typename T>
struct quadraticInOut
{
  constexpr T operator()(T t) const
  {
    return (t < 0.5) ? 2. * t * t : (-2. * t * t) + (4. * t) - 1.;
  }
};

template <typename T>
struct cubicIn
{
  constexpr T operator()(T t) const
  {
    return std::pow(t, 3);
  }
};

template <typename T>
struct cubicOut
{
  constexpr T operator()(T t) const
  {
    return std::pow(t - 1., 3) + 1.;
  }
};

template <typename T>
struct cubicInOut
{
  constexpr T operator()(T t) const
  {
    return (t < T(0.5))
        ?  4. * std::pow(t, 3)
        : 0.5 * std::pow((2. * t) - 2, 3) + 1.;
  }
};

template <typename T>
struct quarticIn
{
  constexpr T operator()(T t) const
  {
    return std::pow(t, 4);
  }
};

template <typename T>
struct quarticOut
{
  constexpr T operator()(T t) const
  {
    return std::pow(t - 1., 3) * (1. - t) + 1.;
  }
};

template <typename T>
struct quarticInOut
{
  constexpr T operator()(T t) const
  {
    return (t < 0.5)
      ?  8. * std::pow(t, 4)
      : -8. * std::pow(t - 1., 4) + 1.;
  }
};

template <typename T>
struct quinticIn
{
  constexpr T operator()(T t) const
  {
    return std::pow(t, 5);
  }
};

template <typename T>
struct quinticOut
{
  constexpr T operator()(T t) const
  {
    return std::pow(t - 1., 5) + 1.;
  }
};

template <typename T>
struct quinticInOut
{
  constexpr T operator()(T t) const
  {
    return (t < 0.5)
        ? 16. * std::pow(t, 5)
        : 0.5 * std::pow((2. * t) - 2., 5) + 1.;
  }
};

template <typename T>
struct sineIn
{
  constexpr T operator()(T t) const
  {
    return std::sin((t - 1.) * half_pi) + 1.;
  }
};

template <typename T>
struct sineOut
{
  constexpr T operator()(T t) const
  {
    return std::sin(t * half_pi);
  }
};

template <typename T>
struct sineInOut
{
  constexpr T operator()(T t) const
  {
    return 0.5 * (1. - std::cos(t * pi));
  }
};

template <typename T>
struct circularIn
{
  constexpr T operator()(T t) const
  {
    return 1. - std::sqrt(1. - (t * t));
  }
};

template <typename T>
struct circularOut
{
  constexpr T operator()(T t) const
  {
    return std::sqrt((2. - t) * t);
  }
};

template <typename T>
struct circularInOut
{
  constexpr T operator()(T t) const
  {
    return (t < 0.5)
       ? 0.5 * (1 - std::sqrt(1 - 4. * (t * t)))
       : 0.5 * (std::sqrt(-((2. * t) - 3.) * ((2. * t) - 1.)) + 1.);
  }
};

template <typename T>
struct exponentialIn
{
  constexpr T operator()(T t) const
  {
    return (t <= 0) ? t : std::exp2(10. * (t - 1.));
  }
};

template <typename T>
struct exponentialOut
{
  constexpr T operator()(T t) const
  {
    return (t >= 1.) ? t : 1. - std::exp2(-10. * t);
  }
};

template <typename T>
struct exponentialInOut
{
  constexpr T operator()(T t) const
  {
    return (t <= 0. || t >= 1.)
        ? t
        : (t < 0.5)
           ?  0.5 * std::exp2(( 20. * t) - 10.)
           : -0.5 * std::exp2((-20. * t) + 10.) + 1.;
  }
};

template <typename T>
struct elasticIn
{
  constexpr T operator()(T t) const
  {
    return std::sin(13. * half_pi * t) * std::exp2(10. * (t - 1.));
  }
};

template <typename T>
struct elasticOut
{
  constexpr T operator()(T t) const
  {
    return sin(-13. * half_pi * (t + 1.)) * std::exp2(-10. * t) + 1.;
  }
};

template <typename T>
struct elasticInOut
{
  constexpr T operator()(T t) const
  {
    return (t < 0.5)
        ? 0.5 * std::sin(13. * half_pi * (2. * t))
              * std::exp2(10. * ((2. * t) - 1.))
        : 0.5 * (std::sin(-13. * half_pi * ((2. * t - 1) + 1))
                 * std::exp2(-10. * (2. * t - 1.))
                 + 2.);
  }
};

template <typename T>
struct backIn
{
  constexpr T operator()(T t) const
  {
    return std::pow(t, 3) - t * std::sin(t * pi);
  }
};

template <typename T>
struct backOut
{
  constexpr T operator()(T t) const
  {
    return 1. - (std::pow(1. - t, 3) - (1. - t) * std::sin((1. - t) * pi));
  }
};

template <typename T>
struct backInOut
{
  constexpr T operator()(T t) const
  {
    return (t < 0.5)
        ? 0.5 * (std::pow(2. * t, 3) - (2. * t) * std::sin((2. * t) * pi))
        : 0.5 * (1. - (std::pow(1. - (2. * t - 1.), 3) - (1. - (2. * t - 1.)) * std::sin((1. - (2. * t - 1.)) * pi))) + 0.5;
  }
};

template <typename T>
struct bounceOut
{
  constexpr T operator()(T t) const
  {
    return t < 4. / 11.
        ? (121. * t * t) / 16.
        : (t < 8. / 11.)
          ? (363. / 40. * t * t) - (99 / 10. * t) + 17 / 5.
          : (t < 9. / 10.)
            ? (4356. / 361. * t * t) - (35442. / 1805. * t) + 16061. / 1805.
            : (54. / 5. * t * t) - (513. / 25. * t) + 268. / 25.;
  }
};

template <typename T>
struct bounceIn
{
  constexpr T operator()(T t) const
  {
    return 1. - bounceOut<T>{}(1. - t);
  }
};

template <typename T>
struct bounceInOut
{
  constexpr T operator()(T t) const
  {
    return t < 0.5
        ? 0.5 * bounceIn<T>{}(t * 2.)
        : 0.5 * bounceOut<T>{}(t * 2. - 1.) + 0.5;
  }
};

template <typename T>
struct perlinInOut
{
  constexpr T operator()(T t) const
  {
    return 6. * std::pow(t, 5) - 15. * std::pow(t, 4) + 10. * std::pow(t, 3);
  }
};
}

template <typename Y, typename Easing>
struct curve_segment_ease
{
  Y operator()(double ratio, Y start, Y end) const
  {
    return start + Easing{}(ratio) * (end - start);
  }
};
}
