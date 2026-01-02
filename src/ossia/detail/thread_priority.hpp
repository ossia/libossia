#pragma once

#include <ossia/detail/sleep.hpp>

namespace ossia
{

struct priority_boost_handle
{
public:
  explicit priority_boost_handle(double frequencyHz)
  {
#ifdef HRT_PLATFORM_WINDOWS
    // Timer resolution already increased in main()

    // MMCSS priority boost
    DWORD taskIndex = 0;
    m_mmcss = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
    if (m_mmcss)
    {
      AvSetMmThreadPriority(m_mmcss, AVRT_PRIORITY_CRITICAL);
    }
#endif

#ifdef HRT_PLATFORM_MACOS
    // Set real-time thread policy with time constraint
    // This tells the scheduler we need periodic execution
    mach_port_t threadPort = pthread_mach_thread_np(pthread_self());

    // Convert frequency to period in mach absolute time units
    double periodNs = 1'000'000'000.0 / frequencyHz;
    uint32_t periodMach = static_cast<uint32_t>(periodNs * g_timebase_info.denom / g_timebase_info.numer);

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
#endif

#ifdef HRT_PLATFORM_LINUX
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
#ifdef HRT_PLATFORM_WINDOWS
    if (m_mmcss)
    {
      AvRevertMmThreadCharacteristics(m_mmcss);
      m_mmcss = nullptr;
    }
#endif

#ifdef HRT_PLATFORM_MACOS
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
#endif

#ifdef HRT_PLATFORM_LINUX
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
