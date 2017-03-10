#include <ossia/detail/thread.hpp>
#if defined(_MSC_VER)

#include <windows.h>

namespace ossia
{
void set_thread_realtime(std::thread& t)
{
  auto hdl = t.native_handle();

  SetPriorityClass(hdl, REALTIME_PRIORITY_CLASS);
  SetThreadPriority(hdl, THREAD_PRIORITY_TIME_CRITICAL);
}
}
#else

#include <pthread.h>
namespace ossia
{
void set_thread_realtime(std::thread& t)
{
  sched_param sch_params;
  sch_params.sched_priority = 99;
  pthread_setschedparam(t.native_handle(), SCHED_FIFO, &sch_params);
}
}

#endif
