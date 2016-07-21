#pragma once
#include <ossia_export.h>
#include <Editor/CurveSegment/MathUtil.h>
#include <cmath>
#include <Editor/CurveSegment.h>

/*
  Easing functions
  by Kristoffer Gronlund, 2014
  Public domain
  This work is a spiritual descendent (not to say derivative work) of works done by the following individuals:
  Warren Moore (https://github.com/warrenm)
  Robert Penner (http://www.robertpenner.com/easing/)
  George McGinley Smith (http://gsgd.co.uk/sandbox/jquery/easing/)
  James Padolsey (http://james.padolsey.com/demos/jquery/easing/)
  Authors of jQuery (http://plugins.jquery.com/project/Easing)
  Matt Gallagher (http://cocoawithlove.com/2008/09/parametric-acceleration-curves-in-core.html)
  Jesse Crossen (http://stackoverflow.com/questions/5161465/how-to-create-custom-easing-function-with-core-animation)
*/

namespace OSSIA
{
namespace easing {

template <typename T>
struct ease {
  constexpr T operator()(T a, T b, T t) {
    return a + (b - a) * t;
  } };

template <typename T>
struct linear {
  constexpr T operator()(T t) {
    return t;
  } };

template <typename T>
struct quadraticIn {
  constexpr T operator()(T t) {
    return t * t;
  } };

template <typename T>
struct quadraticOut {
  constexpr T operator()(T t) {
    return -(t * (t - 2.));
  } };

template <typename T>
struct quadraticInOut {
  constexpr T operator()(T t) {
    return (t < 0.5) ? 2. * t * t : (-2. * t * t) + (4. * t) - 1.;
  } };

template <typename T>
struct cubicIn {
  constexpr T operator()(T t) {
    return t * t * t;
  } };

template <typename T>
struct cubicOut {
  constexpr T operator()(T t) {
    const T f = t - 1.; return f * f * f + 1.;
  } };

template <typename T>
struct cubicInOut {
  constexpr T operator()(T t) {
  if (t < T(0.5)) {
    return 4. * t * t * t;
  } else {
    const T f = (2. * t) - 2.;
    return 0.5 * f * f * f + 1.;
  }
} };

template <typename T>
struct quarticIn {
  constexpr T operator()(T t) {
  return t * t * t * t;
} };

template <typename T>
struct quarticOut {
  constexpr T operator()(T t) {
  const T f = t - 1.;
  return f * f * f * (1. - t) + 1.;
} };

template <typename T>
struct quarticInOut {
  constexpr T operator()(T t) {
  if(t < 0.5) {
    return 8. * t * t * t * t;
  } else {
    T f = (t - 1.);
    return -8. * f * f * f * f + 1.;
  }
} };

template <typename T>
struct quinticIn {
  constexpr T operator()(T t) {
  return t * t * t * t * t;
} };

template <typename T>
struct quinticOut {
  constexpr T operator()(T t) {
  T f = (t - 1.);
  return f * f * f * f * f + 1.;
} };

template <typename T>
struct quinticInOut {
  constexpr T operator()(T t) {
  if (t < 0.5) {
    return 16. * t * t * t * t * t;
  } else {
    T f = ((2. * t) - 2.);
    return  0.5 * f * f * f * f * f + 1.;
  }
} };

template <typename T>
struct sineIn {
  constexpr T operator()(T t) {
  return sin((t - 1.) * half_pi) + 1.;
} };

template <typename T>
struct sineOut {
  constexpr T operator()(T t) {
  return sin(t * half_pi);
} };

template <typename T>
struct sineInOut {
  constexpr T operator()(T t) {
  return 0.5 * (1. - std::cos(t * pi));
} };

template <typename T>
struct circularIn {
  constexpr T operator()(T t) {
  return 1. - std::sqrt(1. - (t * t));
} };

template <typename T>
struct circularOut {
  constexpr T operator()(T t) {
  return std::sqrt((2. - t) * t);
} };

template <typename T>
struct circularInOut {
  constexpr T operator()(T t) {
  if (t < 0.5) {
    return 0.5 * (1 - std::sqrt(1 - 4. * (t * t)));
  } else {
    return 0.5 * (std::sqrt(-((2. * t) - 3.) * ((2. * t) - 1.)) + 1.);
  }
} };

template <typename T>
struct exponentialIn {
  constexpr T operator()(T t) {
  return (t <= 0) ? t : std::pow(2., 10. * (t - 1.));
} };

template <typename T>
struct exponentialOut {
  constexpr T operator()(T t) {
  return (t >= 1.) ? t : 1. - std::pow(2., -10. * t);
} };

template <typename T>
struct exponentialInOut {
  constexpr T operator()(T t) {
  if (t <= 0. || t >= 1.)
    return t;

  if (t < 0.5) {
    return 0.5 * std::pow(2., (20. * t) - 10.);
  } else {
    return -0.5 * std::pow(2., (-20. * t) + 10.) + 1.;
  }
} };

template <typename T>
struct elasticIn {
  constexpr T operator()(T t) {
  return sin(13. * half_pi * t) * std::pow(2., 10. * (t - 1.));
} };

template <typename T>
struct elasticOut {
  constexpr T operator()(T t) {
  return sin(-13. * half_pi * (t + 1.)) * std::pow(2., -10. * t) + 1.;
} };

template <typename T>
struct elasticInOut {
  constexpr T operator()(T t) {
  if (t < 0.5) {
    return 0.5 * std::sin(13. * half_pi * (2. * t)) * std::pow(2., 10. * ((2. * t) - 1.));
  } else {
    return 0.5 * (std::sin(-13. * half_pi * ((2. * t - 1) + 1)) * std::pow(2., -10. * (2. * t - 1.)) + 2.);
  }
} };

template <typename T>
struct backIn {
  constexpr T operator()(T t) {
  return t * t * t - t * std::sin(t * pi);
} };

template <typename T>
struct backOut {
  constexpr T operator()(T t) {
  const T f = 1. - t;
  return 1. - (f * f * f - f * std::sin(f * pi));
} };

template <typename T>
struct backInOut {
  constexpr T operator()(T t) {
  if (t < 0.5) {
    const T f = 2. * t;
    return 0.5 * (f * f * f - f * std::sin(f * pi));
  } else {
    const T f = (1. - (2.*t - 1.));
    return 0.5 * (1. - (f * f * f - f * std::sin(f * pi))) + 0.5;
  }
} };

template <typename T>
struct bounceOut {
  constexpr T operator()(T t) {
  if (t < 4. / 11.) {
    return (121. * t * t) / 16.;
  } else if (t < 8. / 11.) {
    return (363. / 40. * t * t) - (99 / 10. * t) + 17 / 5.;
  } else if (t < 9. / 10.) {
    return (4356. / 361. * t * t) - (35442. / 1805. * t) + 16061. / 1805.;
  } else {
    return (54. / 5. * t * t) - (513. / 25. * t) + 268. / 25.;
  }
} };

template <typename T>
struct bounceIn {
  constexpr T operator()(T t) {
  return 1. - bounceOut<T>{}(1. - t);
} };

template <typename T>
struct bounceInOut {
  constexpr T operator()(T t) {
  if (t < 0.5) {
    return 0.5 * bounceIn<T>{}(t * 2.);
  } else {
    return 0.5 * bounceOut<T>{}(t * 2. - 1.) + 0.5;
  }
} };

template <typename T>
struct perlinInOut {
  constexpr T operator()(T t) {
  T t3 = t * t * t;
  T t4 = t3 * t;
  T t5 = t4 * t;
  return 6. * t5 - 15. * t4 + 10. * t3;
} };

}
/*
template <typename Y, typename Easing>
struct OSSIA_EXPORT CurveSegmentEase final : public CurveSegment<Y>
{
  Y valueAt(double ratio, Y start, Y end) const override
  {
    return start + Easing{}(ratio) * (end - start);
  }

};
*/
}
