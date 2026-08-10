#pragma once

#include <ossia/detail/sleep.hpp>
#if defined(_WIN32)
#include <mmsystem.h>
#include <avrt.h>
#endif

namespace ossia
{

#if defined(_WIN32)
namespace detail
{
struct mmcss_api
{
  using set_characteristics_t = HANDLE(WINAPI*)(LPCWSTR, LPDWORD);
  using set_priority_t = BOOL(WINAPI*)(HANDLE, AVRT_PRIORITY);
  using revert_t = BOOL(WINAPI*)(HANDLE);

  set_characteristics_t set_characteristics{};
  set_priority_t set_priority{};
  revert_t revert{};

  mmcss_api() noexcept
  {
    if(HMODULE lib = LoadLibraryW(L"avrt.dll"))
    {
      set_characteristics = reinterpret_cast<set_characteristics_t>(
          reinterpret_cast<void*>(GetProcAddress(lib, "AvSetMmThreadCharacteristicsW")));
      set_priority = reinterpret_cast<set_priority_t>(
          reinterpret_cast<void*>(GetProcAddress(lib, "AvSetMmThreadPriority")));
      revert = reinterpret_cast<revert_t>(
          reinterpret_cast<void*>(GetProcAddress(lib, "AvRevertMmThreadCharacteristics")));
    }
  }
};

inline const mmcss_api& mmcss() noexcept
{
  static const mmcss_api api;
  return api;
}
}
#endif

struct priority_boost_handle
{
public:
  explicit priority_boost_handle(double frequencyHz)
  {
#if defined(_WIN32)
    // Timer resolution already increased in main()

    // MMCSS priority boost
    DWORD taskIndex = 0;
    const auto& mmcss = detail::mmcss();
    if (mmcss.set_characteristics)
      m_mmcss = mmcss.set_characteristics(L"Pro Audio", &taskIndex);
    if (m_mmcss && mmcss.set_priority)
    {
      mmcss.set_priority(m_mmcss, AVRT_PRIORITY_CRITICAL);
    }
#elif defined(__APPLE__)
    // Set real-time thread policy with time constraint
    // This tells the scheduler we need periodic execution
    mach_port_t threadPort = pthread_mach_thread_np(pthread_self());

    // Convert frequency to period in mach absolute time units
    double periodNs = 1'000'000'000.0 / frequencyHz;
    uint32_t periodMach = static_cast<uint32_t>(periodNs * detail::g_timebase_info.denom / detail::g_timebase_info.numer);

    thread_time_constraint_policy_data_t policy;
    policy.period = periodMach;                          // Nominal period
    policy.computation = periodMach / 10;                // Max computation time (~10% of period)
    policy.constraint = periodMach / 2;                  // Must complete within half period
    policy.preemptible = 1;                              // Can be preempted

    kern_return_t kr = thread_policy_set(
        threadPort,
        THREAD_TIME_CONSTRAINT_POLICY,
        reinterpret_cast<thread_policy_t>(&policy),
        THREAD_TIME_CONSTRAINT_POLICY_COUNT
        );

    m_policy_set = (kr == KERN_SUCCESS);

    if (!m_policy_set)
    {
      // Fallback: at least set high QoS
      pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
    }
#else
    // Save original scheduling policy
    pthread_getschedparam(pthread_self(), &m_original_policy, &m_original_param);

    // Try to set SCHED_FIFO
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);

    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) == 0)
    {
      m_elevated = true;
    }
    else
    {
      // Fallback: set highest nice value we can
      setpriority(PRIO_PROCESS, 0, -20);
    }
#endif
  }

  ~priority_boost_handle()
  {
#if defined(_WIN32)
    if (m_mmcss)
    {
      if (const auto& mmcss = detail::mmcss(); mmcss.revert)
        mmcss.revert(m_mmcss);
      m_mmcss = nullptr;
    }
#elif defined(__APPLE__)
    if (m_policy_set)
    {
      // Revert to default policy
      mach_port_t threadPort = pthread_mach_thread_np(pthread_self());
      thread_standard_policy_data_t policy;
      thread_policy_set(
          threadPort,
          THREAD_STANDARD_POLICY,
          reinterpret_cast<thread_policy_t>(&policy),
          THREAD_STANDARD_POLICY_COUNT
          );
      m_policy_set = false;
    }
#else
    if (m_elevated)
    {
      pthread_setschedparam(pthread_self(), m_original_policy, &m_original_param);
      m_elevated = false;
    }
#endif
  }

private:
#if defined(_WIN32)
  HANDLE m_mmcss = nullptr;
  UINT m_timer_resolution = 1;
#elif defined(__APPLE__)
  bool m_policy_set = false;
#else
  int m_original_policy = SCHED_OTHER;
  struct sched_param m_original_param = {};
  bool m_elevated = false;
#endif
};

}
