#pragma once
#include <ossia/detail/config.hpp>

// Adapted from bind9:
// https://gitlab.isc.org/isc-projects/bind9/-/blob/main/lib/isc/rwlock.c
#if defined(__EMSCRIPTEN__)
#include <thread>
// TODO once we support asyncify
#define ossia_rwlock_pause() std::this_thread::yield()
#elif defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define ossia_rwlock_pause() _mm_pause()
#elif defined(__i386__)
#define ossia_rwlock_pause() __asm__ __volatile__("rep; nop" ::: "memory")
#elif defined(__ia64__)
#define ossia_rwlock_pause() __asm__ __volatile__("hint @pause" ::: "memory")
#elif defined(__aarch64__)
#define ossia_rwlock_pause() __asm__ __volatile__("dmb ishst\n\tyield" ::: "memory")
#elif defined(__arm__)
#define ossia_rwlock_pause() __asm__ __volatile__("yield" ::: "memory")
#elif defined(__sparc) || defined(__sparc__)
#define ossia_rwlock_pause() __asm__ __volatile__("pause" ::: "memory")
#elif defined(__ppc__) || defined(_ARCH_PPC) || defined(_ARCH_PWR) \
|| defined(_ARCH_PWR2) || defined(_POWER)
#define ossia_rwlock_pause() __asm__ __volatile__("or 27,27,27" ::: "memory")
#elif defined(#elif defined(__riscv)
#define ossia_rwlock_pause() __asm__ __volatile__("pause" ::: "memory")
#elif defined(_MSC_VER)
#include <windows.h>
#define ossia_rwlock_pause() YieldProcessor()
#else
#include <thread>
#define ossia_rwlock_pause() std::this_thread::yield()
#endif
