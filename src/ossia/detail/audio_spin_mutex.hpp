#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/mutex.hpp>
#include <ossia/detail/yield.hpp>

#include <array>
#include <atomic>
#include <thread>

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

private:
  std::atomic<bool> locked{false};
};
}
