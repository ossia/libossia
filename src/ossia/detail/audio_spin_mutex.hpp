#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/mutex.hpp>

#include <array>
#include <atomic>
#include <thread>

// Adapted from bind9:
// https://gitlab.isc.org/isc-projects/bind9/-/blob/main/lib/isc/rwlock.c
#if defined(__EMSCRIPTEN__)
// TODO once we support asyncify
#define ossia_rwlock_pause()
#elif defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define ossia_rwlock_pause() _mm_pause()
#elif defined(__i386__)
#define ossia_rwlock_pause() __asm__ __volatile__("rep; nop")
#elif defined(__ia64__)
#define ossia_rwlock_pause() __asm__ __volatile__("hint @pause")
#elif defined(__aarch64__)
#define ossia_rwlock_pause() __asm__ __volatile__("dmb ishst\n\tyield" ::: "memory")
#elif defined(__arm__)
#define ossia_rwlock_pause() __asm__ __volatile__("yield")
#elif defined(__sparc) || defined(__sparc__)
#define ossia_rwlock_pause() __asm__ __volatile__("pause")
#elif defined(__ppc__) || defined(_ARCH_PPC) || defined(_ARCH_PWR) \
    || defined(_ARCH_PWR2) || defined(_POWER)
#define ossia_rwlock_pause() __asm__ __volatile__("or 27,27,27")
#elif defined(#elif defined(__riscv)
#define ossia_rwlock_pause() __asm__ __volatile__("pause"))
#elif defined(_MSC_VER)
#include <windows.h>
#define ossia_rwlock_pause() YieldProcessor()
#else
#define ossia_rwlock_pause()
#endif

namespace ossia
{

// Code adapted from Timur Doumler's great article:
// https://timur.audio/using-locks-in-real-time-audio-processing-safely
struct TS_CAPABILITY("mutex") audio_spin_mutex
{
  void lock() noexcept TS_ACQUIRE()
  {
    // approx. 5x5 ns (= 25 ns), 10x40 ns (= 400 ns), and 3000x350 ns
    // (~ 1 ms), respectively, when measured on a 2.9 GHz Intel i9
    constexpr std::array iterations = {5, 10, 3000};

    for(int i = 0; i < iterations[0]; ++i)
    {
      if(try_lock())
        return;
    }

    for(int i = 0; i < iterations[1]; ++i)
    {
      if(try_lock())
        return;

      ossia_rwlock_pause();
    }

    while(true)
    {
      for(int i = 0; i < iterations[2]; ++i)
      {
        if(try_lock())
          return;

        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
        ossia_rwlock_pause();
      }

      // waiting longer than we should, let's give other threads
      // a chance to recover
      std::this_thread::yield();
    }
  }

  bool try_lock() TS_TRY_ACQUIRE(true)
  {
    return !locked.load(std::memory_order_relaxed)
           && !locked.exchange(true, std::memory_order_acquire);
  }

  void unlock() TS_RELEASE() { locked.store(false, std::memory_order_release); }

  const auto& operator!() const { return *this; }

private:
  std::atomic<bool> locked{false};
};
}
