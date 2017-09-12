// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/thread.hpp>
#include <string>
#include <ossia/../../3rdparty/whereami/src/whereami.h>
#include <ossia/../../3rdparty/whereami/src/whereami.c>

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

int get_pid()
{
  return GetCurrentProcessId();
}

}
#else

#include <pthread.h>
#include <unistd.h>
namespace ossia
{
void set_thread_realtime(std::thread& t)
{
  sched_param sch_params;
  sch_params.sched_priority = 99;
  pthread_setschedparam(t.native_handle(), SCHED_FIFO, &sch_params);
}

int get_pid()
{
  return getpid();
}

}

#endif


namespace ossia
{
std::string get_exe_path()
{
  std::string path;
  int length{};
  int dirname_length{};

  length = wai_getExecutablePath(NULL, 0, &dirname_length);
  if (length > 0)
  {
    path.resize(length);
    wai_getExecutablePath(&path[0], length, &dirname_length);
  }

  return path;
}

std::string get_module_path()
{
  std::string path;
  int length{};
  int dirname_length{};

  length = wai_getModulePath(NULL, 0, &dirname_length);
  if (length > 0)
  {
    path.resize(length);
    wai_getModulePath(&path[0], length, &dirname_length);
  }

  return path;
}

}
