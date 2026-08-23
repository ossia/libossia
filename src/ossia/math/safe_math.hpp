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

OSSIA_INLINE bool safe_isfinite(double val) noexcept
{
#if __FINITE_MATH_ONLY__
#if defined(_MSC_VER)
  return std::isfinite(val);
#elif defined(__APPLE__)
  return __isfinited(val);
#elif defined(__EMSCRIPTEN__)
  const auto cls = __fpclassifyl(val);
  return cls != FP_NAN && cls != FP_INFINITE;
#else
  // On gcc / clang, with -ffast-math, std::isfinite always returns 1.
  // NaN and infinity are exactly the values whose exponent field is all ones
  // and nothing else is, so unlike the two predicates above this needs only
  // one test and does not have to look at the mantissa at all.
  union
  {
    double fp;
    uint64_t bits;
  } num{.fp = val};

  return ((unsigned)(num.bits >> 32) & 0x7ff00000) != 0x7ff00000;
#endif
#else
  return std::isfinite(val);
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
