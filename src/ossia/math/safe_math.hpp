#pragma once
#include <ossia/detail/config.hpp>

#include <cmath>

namespace ossia
{

OSSIA_INLINE bool safe_isnan(double val) noexcept
{
#if __FINITE_MATH_ONLY__
#if defined(_MSC_VER)
  return std::isnan(val);
#elif defined(__APPLE__)
  return __isnand(val);
#elif defined(__EMSCRIPTEN__)
  return __fpclassifyl(val) == FP_NAN;
#else
  // On gcc / clang, with -ffast-math, std::isnan always returns 0
  // There's __isnan but it's not always available.
  union
  {
    double fp;
    uint64_t bits;
  } num{.fp = val};

  return ((unsigned)(num.bits >> 32) & 0x7fffffff) + ((unsigned)num.bits != 0)
         > 0x7ff00000;
#endif
#else
  return std::isnan(val);
#endif
}

OSSIA_INLINE bool safe_isinf(double val) noexcept
{
#if __FINITE_MATH_ONLY__
#if defined(_MSC_VER)
  return std::isinf(val);
#elif defined(__APPLE__)
  return __isinfd(val);
#elif defined(__EMSCRIPTEN__)
  return __fpclassifyl(val) == FP_INFINITE;
#else
  // On gcc / clang, with -ffast-math, std::isinf always returns 0
  // There's __isinf but it's not always available.
  union
  {
    double fp;
    uint64_t bits;
  } num{.fp = val};

  return ((unsigned)(num.bits >> 32) & 0x7fffffff) == 0x7ff00000
         && (unsigned)num.bits == 0;
#endif
#else
  return std::isinf(val);
#endif
}

}
