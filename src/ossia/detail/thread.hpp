#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/flat_map.hpp>

#include <string>
#include <thread>

namespace ossia
{
OSSIA_EXPORT
void set_thread_realtime(std::thread& t, int prio = 99, bool algo_fifo = true);
OSSIA_EXPORT
void set_thread_name(std::thread& t, std::string_view name);
OSSIA_EXPORT
void set_thread_name(std::string_view name);
OSSIA_EXPORT
void set_thread_pinned(int cpu);

enum class thread_type : unsigned char
{
  Net = 'N',
  Midi = 'M',
  Gpu = 'G',
  GpuTask = 'g',
  Audio = 'A',
  AudioTask = 'a',
  Ui = 'U',
  UiTask = 'u'
};

struct thread_spec
{
  int num_threads{};
  int spin_interval{};
};

using thread_specs = ossia::flat_map<thread_type, thread_spec>;

OSSIA_EXPORT
const thread_specs& get_thread_specs() noexcept;

OSSIA_EXPORT
thread_type get_current_thread_type();

// Schedule e.g. the third net thread on the appropriate CPU
OSSIA_EXPORT
void set_thread_pinned(thread_type kind, int thread_index);

OSSIA_EXPORT
void ensure_current_thread(thread_type kind);

#if defined(NDEBUG)
#define OSSIA_ENSURE_CURRENT_THREAD(a) \
  do                                   \
  {                                    \
  } while(0)
#else
#define OSSIA_ENSURE_CURRENT_THREAD(a) ::ossia::ensure_current_thread(a)
#endif

OSSIA_EXPORT
std::string get_exe_path();

OSSIA_EXPORT
std::string get_exe_folder();

OSSIA_EXPORT
std::string get_module_path();

OSSIA_EXPORT
int get_pid();
}
