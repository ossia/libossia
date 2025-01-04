#include <ossia/detail/disable_fpe.hpp>

#include <boost/predef.h>

#if BOOST_ARCH_X86
#include <xmmintrin.h>
#endif

#include <cfenv>
#include <cstdint>

namespace ossia
{
#if defined(__APPLE__)
static void disable_fpe_impl()
{
  fenv_t env;
  fegetenv(&env);

#if BOOST_ARCH_ARM
  env.__fpcr = env.__fpcr & ~(FE_ALL_EXCEPT << 8);
#else
  env.__control = env.__control | FE_ALL_EXCEPT;
  env.__mxcsr = env.__mxcsr | (FE_ALL_EXCEPT << 7);
#endif

  fesetenv(&env);
}
#elif BOOST_ARCH_X86
static void disable_fpe_impl()
{
#if defined(_MSC_VER)
#else
  uint32_t mxcsr;
  uint16_t control;
  __asm volatile("fnstcw %0" : "=m"(*(&control)));
  __asm volatile("stmxcsr %0" : "=m"(*(&mxcsr)));

  control |= FE_ALL_EXCEPT;
  __asm volatile("fldcw %0" : : "m"(control));

  mxcsr |= FE_ALL_EXCEPT << 7;
  __asm volatile("ldmxcsr %0" : : "m"(mxcsr));
#endif
}
#else
static void disable_fpe_impl() { }
#endif

void disable_fpe()
{
#if defined(__EMSCRIPTEN__)
  // do nothing
#elif defined(__APPLE__)
#pragma STDC FENV_ACCESS ON
#if BOOST_ARCH_ARM
  fesetenv(FE_DFL_DISABLE_DENORMS_ENV);
#elif BOOST_ARCH_X86
  // Apple Docs:
  // It is not possible to disable denormal stalls for calculations performed on the x87 FPU
  fesetenv(FE_DFL_DISABLE_SSE_DENORMS_ENV);
  _mm_setcsr(_MM_MASK_MASK | _MM_FLUSH_ZERO_ON | (1 << 6));
#endif
#else

#if BOOST_ARCH_X86
  // See https://en.wikipedia.org/wiki/Denormal_number
  // and
  // http://stackoverflow.com/questions/9314534/why-does-changing-0-1f-to-0-slow-down-performance-by-10x
  _mm_setcsr(_MM_MASK_MASK | _MM_FLUSH_ZERO_ON | (1 << 6));

#elif BOOST_ARCH_ARM
#if BOOST_ARCH_WORD_BITS_64
#if !defined(_MSC_VER)
  uint64_t fpcr;
  asm("mrs %0,   fpcr" : "=r"(fpcr));          //Load the FPCR register
  asm("msr fpcr, %0" ::"r"(fpcr | (1 << 24))); //Set the 24th bit (FTZ) to 1
#endif
#else
  int x;
  asm("vmrs %[result],FPSCR \r\n"
      "bic %[result],%[result],#16777216 \r\n"
      "vmsr FPSCR,%[result]"
      : [result] "=r"(x)
      :
      :);
#endif
#endif
#endif

  ::feclearexcept(FE_ALL_EXCEPT);
#if defined(__linux__)
  fedisableexcept(FE_ALL_EXCEPT);
#else
  disable_fpe_impl();
#endif
}
}
