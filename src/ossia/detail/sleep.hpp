#pragma once

#include <ossia/detail/yield.hpp>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/thread_policy.h>
#include <pthread.h>
#else
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#endif

#include <cinttypes>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace ossia
{

namespace detail
{
#if defined(_WIN32)
static const LARGE_INTEGER g_performance_frequency = []() {
  LARGE_INTEGER f;
  QueryPerformanceFrequency(&f);
  return f;
}();
#elif defined(__APPLE__)
static const mach_timebase_info_data_t g_timebase_info = []() {
  mach_timebase_info_data_t tb;
  mach_timebase_info(&tb);
  return tb;
}();
#endif
}

// Returns current time in nanoseconds
inline uint64_t now_ns()
{
#if defined(_WIN32)
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return static_cast<uint64_t>(now.QuadPart * 1'000'000'000ULL / detail::g_performance_frequency.QuadPart);
#elif defined(__APPLE__)
  return mach_absolute_time() * detail::g_timebase_info.numer / detail::g_timebase_info.denom;
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return static_cast<uint64_t>(ts.tv_sec) * 1'000'000'000ULL + ts.tv_nsec;
#endif
}

static void coarse_sleep(int64_t ns)
{
#if defined(_WIN32)
  Sleep(static_cast<DWORD>(ns / 1'000'000));
#elif defined(__APPLE__)
  uint64_t now = mach_absolute_time();
  uint64_t deltaMach = static_cast<uint64_t>(ns) * g_timebase_info.denom / g_timebase_info.numer;
  mach_wait_until(now + deltaMach);
#else
  struct timespec ts;
  ts.tv_sec = ns / 1'000'000'000LL;
  ts.tv_nsec = ns % 1'000'000'000LL;
  clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
#endif
}

class adaptive_sleep
{
  static constexpr int64_t max_count = 1000;
  static constexpr int64_t min_sleep_ns = 1'000'000; // Don't bother sleeping < 1ms

public:
  void sleep_until(uint64_t target_ns)
  {
    const uint64_t current = now_ns();
    if (target_ns <= current)
      return;

    const int64_t remaining = static_cast<int64_t>(target_ns - current);
    const int64_t to_sleep = remaining - static_cast<int64_t>(m_margin_ns);

    if (to_sleep > min_sleep_ns)
    {
      const uint64_t start_ns = now_ns();
      coarse_sleep(to_sleep);
      const uint64_t end_ns = now_ns();

      // Measure overshoot: how much longer than requested?
      const double overshoot = static_cast<double>(static_cast<int64_t>(end_ns - start_ns) - to_sleep);
      update(overshoot);
    }

    // Spin for final precision
    while (now_ns() < target_ns)
    {
      ossia_rwlock_pause();
    }
  }

private:
  void update(double overshoot)
  {
    if (m_count >= max_count)
    {
      m_count = max_count / 2;
      m_m2 /= 2.0;
    }

    ++m_count;
    const double delta = overshoot - m_mean_overshoot_ns;
    m_mean_overshoot_ns += delta / static_cast<double>(m_count);
    m_m2 += delta * (overshoot - m_mean_overshoot_ns);

    const double stddev = (m_count > 1) ? std::sqrt(m_m2 / static_cast<double>(m_count - 1)) : 0.0;

    // Margin = mean overshoot + 2 stddev (to wake up early, not late)
    m_margin_ns = std::max(1'000'000.0, m_mean_overshoot_ns + 2.0 * stddev);
  }

  double m_margin_ns = 2'000'000.0; // 2ms estimate
  double m_mean_overshoot_ns = 0.0;
  double m_m2 = 0.0;
  int64_t m_count = 1;
};

}
