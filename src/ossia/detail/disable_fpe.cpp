#include <ossia/detail/disable_fpe.hpp>

#include <boost/predef.h>

#include <cfenv>
#include <cfloat>
#include <cstdint>

#if !defined(__EMSCRIPTEN__)

#if BOOST_ARCH_X86
#if BOOST_COMP_MSVC
#include <float.h>
#include <intrin.h>
#else
#include <pmmintrin.h>
#include <xmmintrin.h>
#if BOOST_COMP_GNUC
#include <cpuid.h>
#endif
#endif

#elif BOOST_ARCH_ARM && (BOOST_ARCH_WORD_BITS == 64)
#if !BOOST_COMP_MSVC
#include <arm_neon.h>
#endif
#endif

#if BOOST_OS_MACOS || BOOST_OS_IOS
#include <TargetConditionals.h>
#endif

#endif

#pragma STDC FENV_ACCESS ON

namespace ossia
{

#if !defined(__EMSCRIPTEN__)
static inline void helper_x87_mask_exceptions()
{
#if BOOST_ARCH_X86 && !BOOST_COMP_MSVC
  uint16_t control;

  // 1. Store x87 Control Word
  __asm volatile("fnstcw %0" : "=m"(control));

  // 2. Set Exception Masks (Bits 0-5)
  // 0x3F = Masks: Invalid, Denormal, DivByZero, Overflow, Underflow, Precision
  control |= 0x3F;

  // 3. Load x87 Control Word back
  __asm volatile("fldcw %0" ::"m"(control));
#endif
}

static inline void set_fast_math_mode()
{
  std::fesetround(FE_TONEAREST);

// Delicate little flowers first
#if BOOST_COMP_MSVC
  unsigned int current_word = 0;
  _controlfp_s(&current_word, _DN_FLUSH, _MCW_DN);

#elif BOOST_OS_MACOS || BOOST_OS_IOS
#if BOOST_ARCH_ARM && (BOOST_ARCH_WORD_BITS == 64)
  uint64_t fpcr;
  asm volatile("mrs %0, fpcr" : "=r"(fpcr));
  fpcr |= (1ULL << 24); // FZ (Flush-to-Zero)
  fpcr |= (1ULL << 25); // DN (Default NaN)
  asm volatile("msr fpcr, %0" ::"r"(fpcr));
#elif BOOST_ARCH_X86
  _mm_setcsr(_mm_getcsr() | 0x8040);
#endif

// Then real systems
#elif BOOST_ARCH_X86
  _mm_setcsr(_mm_getcsr() | 0x8040);

#elif BOOST_ARCH_ARM && (BOOST_ARCH_WORD_BITS == 64)
#if defined(__clang__)
  uint64_t fpcr;
  asm volatile("mrs %0, fpcr" : "=r"(fpcr));
  fpcr |= (1ULL << 24); // FZ (Flush-to-Zero)
  fpcr |= (1ULL << 25); // DN (Default NaN)
  asm volatile("msr fpcr, %0" ::"r"(fpcr));
#else
  uint64_t fpcr = __builtin_aarch64_get_fpcr();
  fpcr |= (1ULL << 24); // FZ (Flush-to-Zero)
  fpcr |= (1ULL << 25); // DN (Default NaN)
  __builtin_aarch64_set_fpcr(fpcr);
#endif

#elif BOOST_ARCH_ARM && (BOOST_ARCH_WORD_BITS == 32)
  uint32_t fpscr;
  asm volatile("vmrs %0, fpscr" : "=r"(fpscr));
  fpscr |= (1 << 24); // FZ (Enable Flush-to-Zero)
  fpscr |= (1 << 25); // DN (Default NaN)
  asm volatile("vmsr fpscr, %0" ::"r"(fpscr));

#elif BOOST_ARCH_RISCV
  unsigned int fcsr;
  asm volatile("frcsr %0" : "=r"(fcsr));
  fcsr &= ~(0b111 << 5); // Force Round-to-Nearest
  asm volatile("fscsr %0" ::"r"(fcsr));
#endif
}

static inline void mask_fpu_exceptions()
{
  std::feclearexcept(FE_ALL_EXCEPT);

#if BOOST_OS_LINUX && defined(__GLIBC__)
  fedisableexcept(FE_ALL_EXCEPT);
#endif

#if BOOST_COMP_MSVC
  unsigned int current_word = 0;
  _controlfp_s(&current_word, _MCW_EM, _MCW_EM);

#elif BOOST_OS_MACOS
  fenv_t env;
  fegetenv(&env);
#if BOOST_ARCH_ARM
  env.__fpcr = env.__fpcr & ~(FE_ALL_EXCEPT << 8);
#else
  env.__control = env.__control | FE_ALL_EXCEPT;
  env.__mxcsr = env.__mxcsr | (FE_ALL_EXCEPT << 7);
#endif
  fesetenv(&env);

#elif BOOST_ARCH_X86
  unsigned int mxcsr = _mm_getcsr();
  mxcsr |= (0x3F << 7); // Mask bits 7-12
  _mm_setcsr(mxcsr);

  helper_x87_mask_exceptions();
#elif BOOST_ARCH_ARM
#if BOOST_ARCH_WORD_BITS == 64
#if defined(__clang__)
  uint64_t fpcr;
  asm volatile("mrs %0, fpcr" : "=r"(fpcr));
  fpcr &= ~(
      (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11)
      | (1ULL << 12));   // Standard Traps
  fpcr &= ~(1ULL << 15); // Input Denormal Trap (IDE)
  asm volatile("msr fpcr, %0" ::"r"(fpcr));
#else
  uint64_t fpcr = __builtin_aarch64_get_fpcr();
  fpcr &= ~(
      (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11)
      | (1ULL << 12));   // Standard Traps
  fpcr &= ~(1ULL << 15); // Input Denormal Trap (IDE)
  __builtin_aarch64_set_fpcr(fpcr);
#endif
#else
  uint32_t fpscr;
  asm volatile("vmrs %0, fpscr" : "=r"(fpscr));
  fpscr &= ~((1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12)); // Standard Traps
  fpscr &= ~(1 << 15); // Input Denormal Trap
  asm volatile("vmsr fpscr, %0" ::"r"(fpscr));
#endif
#endif
}

void disable_fpe()
{
  set_fast_math_mode();
  mask_fpu_exceptions();
}
#else
void disable_fpe() { }
#endif

void reset_default_fpu_state()
{
  std::fesetenv(FE_DFL_ENV);

#if BOOST_COMP_MSVC
  unsigned int current_word = 0;
  _controlfp_s(&current_word, _DN_SAVE, _MCW_DN); // Denormals: process normally
  _controlfp_s(&current_word, _MCW_EM, _MCW_EM);  // All exceptions masked

#elif BOOST_ARCH_X86
  // Default MXCSR: 0x1F80
  //   - FTZ (bit 15) = 0, DAZ (bit 6) = 0
  //   - Exception masks (bits 7-12) = all set
  //   - Rounding (bits 13-14) = 00 (round-to-nearest)
  _mm_setcsr(0x1F80);

  // FNINIT resets x87 to power-on state
  __asm volatile("fninit");

#elif BOOST_ARCH_ARM && (BOOST_ARCH_WORD_BITS == 64)
#if BOOST_OS_MACOS || BOOST_OS_IOS || defined(__clang__)
  asm volatile(
      "msr fpcr, xzr \n\t"
      "msr fpsr, xzr" ::
          : "memory");
#else
  __builtin_aarch64_set_fpcr(0); // Clear FZ, DN, rounding, trap enables
  __builtin_aarch64_set_fpsr(0); // Clear cumulative exception flags
#endif

#elif BOOST_ARCH_ARM && (BOOST_ARCH_WORD_BITS == 32)
  uint32_t fpscr;
  asm volatile("vmrs %0, fpscr" : "=r"(fpscr));
  fpscr &= ~(1 << 25);                 // Clear DN
  fpscr &= ~(1 << 24);                 // Clear FZ
  fpscr &= ~(0b11 << 22);              // Clear RMode → Round-to-Nearest
  fpscr &= ~((0x1F << 8) | (1 << 15)); // Clear trap enables (bits 8-12, 15)
  fpscr &= ~0x9F;                      // Clear exception flags (bits 0-4, 7)
  // Note: bits 28-31 (NZCV condition flags) are preserved
  asm volatile("vmsr fpscr, %0" ::"r"(fpscr));

#elif BOOST_ARCH_RISCV
  unsigned int fcsr = 0; // Round-to-nearest, all flags cleared
  asm volatile("fscsr %0" ::"r"(fcsr));
#endif
}
}
