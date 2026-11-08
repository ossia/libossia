#pragma once
#include <ossia/detail/config.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>

namespace ossia
{
#if __FINITE_MATH_ONLY__ && !defined(_MSC_VER) && !defined(__EMSCRIPTEN__)
namespace detail
{
//! The bits of a double, out of the optimizer's sight. Under -ffast-math,
//! gcc and clang take every double to be finite, and clang also folds tests
//! of its bits read through a cast or a union. An empty asm statement hides
//! where the bits come from.
OSSIA_INLINE std::uint64_t opaque_bits(double val) noexcept
{
  std::uint64_t bits;
  std::memcpy(&bits, &val, sizeof(bits));
  asm("" : "+r"(bits));
  return bits;
}
}
#endif

OSSIA_INLINE bool safe_isnan(double val) noexcept
{
#if __FINITE_MATH_ONLY__
#if defined(_MSC_VER)
  return std::isnan(val);
#elif defined(__EMSCRIPTEN__)
  return __fpclassifyl(val) == FP_NAN;
#else
  // All exponent bits set, and a mantissa
  return (detail::opaque_bits(val) & 0x7fffffffffffffffull) > 0x7ff0000000000000ull;
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
#elif defined(__EMSCRIPTEN__)
  const auto cls = __fpclassifyl(val);
  return cls != FP_NAN && cls != FP_INFINITE;
#else
  // NaN and infinity are exactly the values whose exponent bits are all set
  return (detail::opaque_bits(val) & 0x7ff0000000000000ull) != 0x7ff0000000000000ull;
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
#elif defined(__EMSCRIPTEN__)
  return __fpclassifyl(val) == FP_INFINITE;
#else
  // All exponent bits set, and no mantissa
  return (detail::opaque_bits(val) & 0x7fffffffffffffffull) == 0x7ff0000000000000ull;
#endif
#else
  return std::isinf(val);
#endif
}

}
