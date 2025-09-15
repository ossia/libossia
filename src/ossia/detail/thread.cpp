// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/thread.hpp>

#include <boost/predef.h>

#include <optional>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>

namespace ossia
{
void set_thread_realtime(std::thread& t, int prio, bool algo_fifo)
{
  auto hdl = reinterpret_cast<HANDLE>(t.native_handle());

  SetPriorityClass(hdl, REALTIME_PRIORITY_CLASS);
  SetThreadPriority(hdl, THREAD_PRIORITY_TIME_CRITICAL);
}

void set_thread_name(std::thread& t, std::string_view name) { }
void set_thread_name(std::string_view name) { }
void set_thread_pinned(std::thread& t, int cpu) { }
void set_thread_pinned(int cpu) { }

int get_pid()
{
  return GetCurrentProcessId();
}
}
#else

#include <pthread.h>

// BSD
#if __has_include(<pthread_np.h>)
#include <pthread_np.h>
#endif

#include <unistd.h>
namespace ossia
{
void set_thread_realtime(std::thread& t, int prio, bool algo_fifo)
{
#if !defined(__EMSCRIPTEN__) && !defined(OSSIA_FREESTANDING)              \
    && (BOOST_OS_UNIX || BOOST_OS_LINUX || BOOST_OS_BSD || BOOST_OS_MACOS \
        || BOOST_LIB_C_GNU)
  sched_param sch_params;
  sch_params.sched_priority = 99;
  pthread_setschedparam(
      t.native_handle(), algo_fifo ? SCHED_FIFO : SCHED_RR, &sch_params);

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
#endif
}

void set_thread_name(std::thread& t, std::string_view name)
{
#if BOOST_OS_MACOS
  pthread_setname_np(name.data());
#elif (BOOST_OS_UNIX || BOOST_OS_LINUX || BOOST_OS_BSD || BOOST_LIB_C_GNU) \
    && !defined(__EMSCRIPTEN__) && !defined(OSSIA_FREESTANDING)
  pthread_setname_np(t.native_handle(), name.data());
#endif
}

void set_thread_name(std::string_view name)
{
#if BOOST_OS_MACOS
  pthread_setname_np(name.data());
#elif (BOOST_OS_UNIX || BOOST_OS_LINUX || BOOST_OS_BSD || BOOST_LIB_C_GNU) \
    && !defined(__EMSCRIPTEN__) && !defined(OSSIA_FREESTANDING)
  pthread_setname_np(pthread_self(), name.data());
#endif
}

void set_thread_pinned(int cpu)
{
#if !defined(__EMSCRIPTEN__) && !defined(OSSIA_FREESTANDING) && (!BOOST_OS_APPLE) \
    && (BOOST_OS_UNIX || BOOST_OS_LINUX || BOOST_OS_BSD || BOOST_LIB_C_GNU)

#if(BOOST_LIB_C_GNU)
  cpu_set_t cpus{};
#else // FreeBSD, NetBSD
  cpuset_t cpus{};
#endif

  CPU_ZERO(&cpus);
  CPU_SET(cpu, &cpus);

  pthread_setaffinity_np(pthread_self(), sizeof(cpus), &cpus);
#endif
}

int get_pid()
{
#if defined(OSSIA_FREESTANDING)
  return 1; // hehe PID 1 is ossia now
#else
  return getpid();
#endif
}

}

#endif

#if defined(__EMSCRIPTEN__) || defined(OSSIA_FREESTANDING)                   \
    || !(                                                                    \
        BOOST_OS_LINUX || BOOST_OS_BSD || BOOST_OS_MACOS || BOOST_OS_WINDOWS \
        || BOOST_OS_CYGWIN)
namespace ossia
{
std::string get_exe_path()
{
  return "";
}
std::string get_exe_folder()
{
  return "";
}
std::string get_module_path()
{
  return "";
}
}
#elif __has_include(<ossia/../../3rdparty/whereami/src/whereami.c>)
#include <ossia/../../3rdparty/whereami/src/whereami.h>

#include <ossia/../../3rdparty/whereami/src/whereami.c>
namespace ossia
{
std::string get_exe_path()
{
  static const std::string path = [] {
    std::string path;
    int length{};
    int dirname_length{};

    length = wai_getExecutablePath(NULL, 0, &dirname_length);
    if(length > 0)
    {
      path.resize(length);
      wai_getExecutablePath(&path[0], length, &dirname_length);
    }

    return path;
  }();
  return path;
}

std::string get_exe_folder()
{
  static const std::string path = []() -> std::string {
    std::string path = get_exe_path();
    auto last_slash =
#if defined(_WIN32)
        path.find_last_of('\\');
#else
        path.find_last_of('/');
#endif
    if(last_slash == std::string::npos)
      return "";

    path = path.substr(0, last_slash);
    return path;
  }();
  return path;
}

std::string get_module_path()
{
  static const std::string path = [] {
    std::string path;
    int length{};
    int dirname_length{};

    length = wai_getModulePath(NULL, 0, &dirname_length);
    if(length > 0)
    {
      path.resize(length);
      wai_getModulePath(&path[0], length, &dirname_length);
    }

    return path;
  }();
  return path;
}
}
#else
namespace ossia
{
std::string get_exe_path()
{
  std::string path;
  return path;
}

std::string get_module_path()
{
  std::string path;
  return path;
}
}
#endif

#include <boost/unordered/concurrent_flat_map.hpp>

#include <bitset>
namespace ossia
{
struct cpu_pin
{
  uint8_t Net : 1 = 0;       //'N',
  uint8_t Midi : 1 = 0;      //'M',
  uint8_t Gpu : 1 = 0;       //'G',
  uint8_t GpuTask : 1 = 0;   // 'g',
  uint8_t Audio : 1 = 0;     // 'A',
  uint8_t AudioTask : 1 = 0; // = 'a',
  uint8_t Ui : 1 = 0;        // = 'U',
  uint8_t UiTask : 1 = 0;    // = 'u'
};

static const ossia::small_vector<cpu_pin, 128>& parse_pins()
{
  static const auto p = [] {
    ossia::small_vector<cpu_pin, 128> vec;
    auto pins = getenv("SCORE_THREAD_PINS");
    if(!pins)
      return vec;

    cpu_pin core{};
    for(unsigned char c : std::string_view(pins))
    {
      switch(c)
      {
        case ',':
        case ';':
          vec.push_back(core);
          core = {};
          break;
        case 'N':
          core.Net = 1;
          break;
        case 'M':
          core.Midi = 1;
          break;
        case 'G':
          core.Gpu = 1;
          break;
        case 'g':
          core.GpuTask = 1;
          break;
        case 'A':
          core.Audio = 1;
          break;
        case 'a':
          core.AudioTask = 1;
          break;
        case 'U':
          core.Ui = 1;
          break;
        case 'u':
          core.UiTask = 1;
          break;
      }
    }
    vec.push_back(core);

    return vec;
  }();
  return p;
}

static thread_local thread_type g_current_thread_type{255};

thread_type get_current_thread_type()
{
  return g_current_thread_type;
}

boost::unordered::concurrent_flat_map<
    std::thread::id, thread_type, std::hash<std::thread::id>>
    g_thread_types;

void ensure_current_thread(thread_type kind)
{
  assert((unsigned char)g_current_thread_type != 255);
  if((unsigned char)kind != (unsigned char)g_current_thread_type)
  {
    if((kind == thread_type::Audio || kind == thread_type::AudioTask)
       && g_current_thread_type != thread_type::Audio
       && g_current_thread_type != thread_type::AudioTask)
      if((kind == thread_type::Gpu || kind == thread_type::GpuTask)
         && g_current_thread_type != thread_type::Gpu
         && g_current_thread_type != thread_type::GpuTask)
        if((kind == thread_type::Ui || kind == thread_type::UiTask)
           && g_current_thread_type != thread_type::Ui
           && g_current_thread_type != thread_type::UiTask)
          fprintf(
              stderr, "!! Expected thread type %c, got %c\n",
              (char)g_current_thread_type, (char)kind);
    std::terminate();
  }
}

void set_thread_pinned(thread_type spec, int thread_index)
{
  static const auto& pins = parse_pins();
  // A string such as "N,M,A,Uu,uN,uN,,a"

  g_thread_types.emplace(std::this_thread::get_id(), spec);
  g_current_thread_type = spec;

  int k = 0;
  for(std::size_t c = 0; c < pins.size(); c++)
  {
#define try_pin(F)                                                           \
  if(F)                                                                      \
  {                                                                          \
    if(k == thread_index)                                                    \
    {                                                                        \
      printf("Pinning %c %d on core %d\n", (int)spec, thread_index, (int)c); \
      return set_thread_pinned(c);                                           \
    }                                                                        \
    else                                                                     \
    {                                                                        \
      k++;                                                                   \
    }                                                                        \
  }

    auto& core = pins[c];
    switch((unsigned char)spec)
    {
      case 'N':
        try_pin(core.Net);
        break;
      case 'M':
        try_pin(core.Midi);
        break;
      case 'G':
        try_pin(core.Gpu);
        break;
      case 'g':
        try_pin(core.GpuTask);
        break;
      case 'A':
        try_pin(core.Audio);
        break;
      case 'a':
        try_pin(core.AudioTask);
        break;
      case 'U':
        try_pin(core.Ui);
        break;
      case 'u':
        try_pin(core.UiTask);
        break;
    }
#undef try_pin
  }
}

const thread_specs& get_thread_specs() noexcept
{
  static const thread_specs specs = [] {
    auto pins = getenv("SCORE_THREAD_PINS");

    ossia::flat_map<thread_type, thread_spec> map;
    const auto check = [&map] {
      if(map[thread_type::Net].num_threads == 0)
        map[thread_type::Net].num_threads = 1;
      if(map[thread_type::Midi].num_threads == 0)
        map[thread_type::Midi].num_threads = 1;
      if(map[thread_type::Gpu].num_threads == 0)
        map[thread_type::Gpu].num_threads = 1;

      if(map[thread_type::GpuTask].num_threads == 0)
        map[thread_type::GpuTask].num_threads = 1;

      if(map[thread_type::Audio].num_threads == 0)
        map[thread_type::Audio].num_threads = 1;
      if(map[thread_type::AudioTask].num_threads == 0)
        map[thread_type::AudioTask].num_threads = 1;

      if(map[thread_type::Ui].num_threads == 0)
        map[thread_type::Ui].num_threads = 1;
      if(map[thread_type::UiTask].num_threads == 0)
        map[thread_type::UiTask].num_threads = 1;
    };
    if(!pins)
    {
      check();
      return map;
    }
    auto userspecs = getenv("SCORE_THREAD_SPECS");

    for(unsigned char c : std::string_view(pins))
    {
      switch(c)
      {
        case ',':
        case ';':
          break;
        case 'N':
          map[thread_type::Net].num_threads++;
          break;
        case 'M':
          map[thread_type::Midi].num_threads++;
          break;
        case 'G':
          map[thread_type::Gpu].num_threads++;
          break;
        case 'g':
          map[thread_type::GpuTask].num_threads++;
          break;
        case 'A':
          map[thread_type::Audio].num_threads++;
          break;
        case 'a':
          map[thread_type::AudioTask].num_threads++;
          break;
        case 'U':
          map[thread_type::Ui].num_threads++;
          break;
        case 'u':
          map[thread_type::UiTask].num_threads++;
          break;
      }
    }

    // A:100,u:1000
    if(userspecs)
    {
      std::optional<thread_type> cur{};
      std::string cur_num;
      for(unsigned char c : std::string_view(userspecs))
      {
        switch(c)
        {
          case ',':
          case ';':
            if(auto i = std::stoi(cur_num); cur && i >= 0 && i < 1000000)
              map[*cur].spin_interval = i;
            cur.reset();
            cur_num.clear();
            break;
          case ':':
            cur_num.clear();
            break;

          case 'N':
            cur = thread_type::Net;
            break;
          case 'M':
            cur = thread_type::Midi;
            break;
          case 'G':
            cur = thread_type::Gpu;
            break;
          case 'g':
            cur = thread_type::GpuTask;
            break;
          case 'A':
            cur = thread_type::Audio;
            break;
          case 'a':
            cur = thread_type::AudioTask;
            break;
          case 'U':
            cur = thread_type::Ui;
            break;
          case 'u':
            cur = thread_type::UiTask;
            break;
          default:
            cur_num += c;
            break;
        }
      }
    }

    check();

    return map;
  }();
  return specs;
}
}
