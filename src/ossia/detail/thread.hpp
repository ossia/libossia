#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/flat_map.hpp>

#include <exception>
#include <functional>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__) && !defined(OSSIA_FREESTANDING)
#define OSSIA_THREAD_PTHREAD 1
#include <pthread.h>
#endif

namespace ossia
{
/**
 * A std::thread whose stack is 16 MB, as the main thread's, rather than the
 * default for secondary threads: 512 kB on macOS, where an unoptimized build
 * of a large avendish node overflows it on the audio thread.
 *
 * Same use as std::thread (start in the constructor, join or detach before
 * destruction). Windows and WebAssembly keep std::thread and their own stack
 * sizes.
 */
class OSSIA_EXPORT thread
{
public:
  static constexpr std::size_t stack_size = 16 * 1024 * 1024;

#if defined(OSSIA_THREAD_PTHREAD)
  using native_handle_type = pthread_t;
#else
  using native_handle_type = std::thread::native_handle_type;
#endif

  thread() noexcept = default;

  //! As std::thread: f and args are copied (or moved) to the new thread.
  template <typename F, typename... Args>
    requires(!std::is_same_v<std::remove_cvref_t<F>, thread>)
  explicit thread(F&& f, Args&&... args)
  {
    using call_t = std::tuple<std::decay_t<F>, std::decay_t<Args>...>;
    start(
        [](void* p) {
      auto* call = static_cast<call_t*>(p);
      std::apply(
          [](auto& fun, auto&... a) { std::invoke(std::move(fun), std::move(a)...); },
          *call);
      delete call;
    },
        [](void* p) { delete static_cast<call_t*>(p); },
        new call_t(std::forward<F>(f), std::forward<Args>(args)...));
  }

  thread(thread&& other) noexcept;
  thread& operator=(thread&& other) noexcept;
  thread(const thread&) = delete;
  thread& operator=(const thread&) = delete;
  ~thread();

  bool joinable() const noexcept;
  void join();
  void detach();
  native_handle_type native_handle() noexcept;

private:
  //! Runs fun(arg) on the new thread; destroy(arg) if it cannot be created.
  void start(void (*fun)(void*), void (*destroy)(void*), void* arg);

#if defined(OSSIA_THREAD_PTHREAD)
  pthread_t m_handle{};
  bool m_joinable{};
#else
  std::thread m_impl;
#endif
};

OSSIA_EXPORT
void set_thread_realtime(std::thread& t, int prio = 99, bool algo_fifo = true);
OSSIA_EXPORT
void set_thread_realtime(ossia::thread& t, int prio = 99, bool algo_fifo = true);
OSSIA_EXPORT
void set_thread_name(std::thread& t, std::string_view name);
OSSIA_EXPORT
void set_thread_name(ossia::thread& t, std::string_view name);
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
  UiTask = 'u',
  Render = 'R',
  RenderTask = 'r',
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

// Allows e.g. Audio and AudioTask
OSSIA_EXPORT
void ensure_current_thread_kind(thread_type kind);

// Request explicitly Audio
OSSIA_EXPORT
void ensure_current_thread(thread_type kind);

#if defined(NDEBUG)
#define OSSIA_ENSURE_CURRENT_THREAD_KIND(a) \
  do                                        \
  {                                         \
  } while(0)
#define OSSIA_ENSURE_CURRENT_THREAD(a) \
  do                                   \
  {                                    \
  } while(0)
#else
#define OSSIA_ENSURE_CURRENT_THREAD_KIND(a) ::ossia::ensure_current_thread_kind(a)
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
