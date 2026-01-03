#pragma once

#include <ossia/detail/yield.hpp>

#if defined(_WIN32)
#include <Windows.h>
#include <timeapi.h>
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
  uint64_t deltaMach = static_cast<uint64_t>(ns) * detail::g_timebase_info.denom / detail::g_timebase_info.numer;
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

class frame_pacing_sleep
{
  static constexpr int64_t max_count = 500;
  static constexpr double min_margin_ns = 500'000.0;   // 0.5ms floor
  static constexpr double max_margin_ns = 5'000'000.0; // 5ms ceiling
  static constexpr double target_error_ns = -500'000.0; // Target waking 0.5ms early
  static constexpr double correction_rate = 0.1;

public:
  void sleep_until(uint64_t target_ns)
  {
    const int64_t to_sleep = static_cast<int64_t>(target_ns - now_ns()) - static_cast<int64_t>(m_margin_ns);

    if (to_sleep > 0)
      coarse_sleep(to_sleep);

    // Spin for final precision
    while (now_ns() < target_ns)
    {
      ossia_rwlock_pause();
    }

    // Measure error: negative = early (good), positive = late (bad)
    const int64_t error_ns = static_cast<int64_t>(now_ns() - target_ns);
    update(static_cast<double>(error_ns));
  }

private:
  void update(double error_ns)
  {
    if (m_count >= max_count)
    {
      m_count = max_count / 2;
      m_m2 /= 2.0;
    }

    ++m_count;
    const double delta = error_ns - m_mean_error_ns;
    m_mean_error_ns += delta / static_cast<double>(m_count);
    m_m2 += delta * (error_ns - m_mean_error_ns);

    // Adjust margin to push mean error toward target (slightly early)
    const double correction = (m_mean_error_ns - target_error_ns) * correction_rate;
    m_margin_ns = std::clamp(m_margin_ns + correction, min_margin_ns, max_margin_ns);
  }

  double m_margin_ns = 2'000'000.0;
  double m_mean_error_ns = 0.0;
  double m_m2 = 0.0;
  int64_t m_count = 0;
};

#if defined(_WIN32)
class windows_timer_sleep
{
  static constexpr int64_t max_count = 500;
  static constexpr double min_margin_ns = 100'000.0;    // 0.1ms floor (high-res timer is good)
  static constexpr double max_margin_ns = 2'000'000.0;  // 2ms ceiling
  static constexpr double target_error_ns = -100'000.0; // Target waking 0.1ms early
  static constexpr double correction_rate = 0.1;

public:
  windows_timer_sleep()
  {
    // Request 1ms system timer resolution
    timeBeginPeriod(1);

    // High-resolution waitable timer
    m_timer = CreateWaitableTimerExW(
        nullptr, nullptr,
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_ALL_ACCESS
        );

    if (m_timer)
    {
      m_high_resolution = true;
    }
    else
    {
      // Fallback to standard waitable timer
      m_timer = CreateWaitableTimerW(nullptr, TRUE, nullptr);
      m_high_resolution = false;
      m_margin_ns = 1'500'000.0; // Need more margin without high-res
    }
  }

  ~windows_timer_sleep()
  {
    if (m_timer)
      CloseHandle(m_timer);
    timeEndPeriod(1);
  }

  windows_timer_sleep(const windows_timer_sleep&) = delete;
  windows_timer_sleep& operator=(const windows_timer_sleep&) = delete;

  void sleep_until(uint64_t target_ns)
  {
    const uint64_t now = ossia::now_ns();
    if (target_ns <= now)
      return;

    const int64_t remaining_ns = static_cast<int64_t>(target_ns - now);
    const int64_t sleep_ns = remaining_ns - static_cast<int64_t>(m_margin_ns);

    if (sleep_ns > 0)
    {
      // Negative value = relative time, in 100ns units
      LARGE_INTEGER due_time;
      due_time.QuadPart = -(sleep_ns / 100);

      SetWaitableTimerEx(m_timer, &due_time, 0, nullptr, nullptr, nullptr, 0);
      WaitForSingleObject(m_timer, INFINITE);
    }

    // Spin for final precision
    while (ossia::now_ns() < target_ns)
    {
      YieldProcessor();
    }

    // Measure actual error and adapt
    const int64_t error_ns = static_cast<int64_t>(ossia::now_ns() - target_ns);
    update(static_cast<double>(error_ns));
  }

private:
  void update(double error_ns)
  {
    if (m_count >= max_count)
    {
      m_count = max_count / 2;
      m_m2 /= 2.0;
    }

    ++m_count;
    const double delta = error_ns - m_mean_error_ns;
    m_mean_error_ns += delta / static_cast<double>(m_count);
    m_m2 += delta * (error_ns - m_mean_error_ns);

    const double correction = (m_mean_error_ns - target_error_ns) * correction_rate;
    m_margin_ns = std::clamp(m_margin_ns + correction, min_margin_ns, max_margin_ns);
  }

  HANDLE m_timer = nullptr;
  bool m_high_resolution = false;
  double m_margin_ns = 500'000.0;
  double m_mean_error_ns = 0.0;
  double m_m2 = 0.0;
  int64_t m_count = 0;
};
#endif
}
