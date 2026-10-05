// ossia::thread: threads get the main thread's 16 MB of stack, not the
// default of the platform for secondary threads (512 kB on macOS).
#include <ossia/detail/thread.hpp>

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <memory>
#include <string>

#if defined(OSSIA_THREAD_PTHREAD)
#include <pthread.h>

static std::size_t current_stack_size()
{
#if defined(__APPLE__)
  return pthread_get_stacksize_np(pthread_self());
#else
  pthread_attr_t attr;
  std::size_t size{};
  if(pthread_getattr_np(pthread_self(), &attr) == 0)
  {
    pthread_attr_getstacksize(&attr, &size);
    pthread_attr_destroy(&attr);
  }
  return size;
#endif
}

TEST_CASE("ossia::thread has a 16 MB stack", "[thread]")
{
  std::size_t size{};
  ossia::thread t{[&] { size = current_stack_size(); }};
  t.join();
  CHECK(size >= ossia::thread::stack_size);
}

TEST_CASE("ossia::thread uses that stack", "[thread]")
{
  // Far past 512 kB; a write per page so that every page is touched.
  std::atomic_int sum{-1};
  ossia::thread t{[&] {
    volatile char buf[4 * 1024 * 1024];
    for(std::size_t i = 0; i < sizeof(buf); i += 4096)
      buf[i] = char(i >> 12);
    sum = buf[4096 * 3];
  }};
  t.join();
  CHECK(sum == 3);
}
#endif

TEST_CASE("ossia::thread moves, joins, detaches like std::thread", "[thread]")
{
  std::atomic_int runs{};
  ossia::thread a{[&] { runs++; }};
  CHECK(a.joinable());
  ossia::thread b = std::move(a);
  CHECK(!a.joinable());
  CHECK(b.joinable());
  b.join();
  CHECK(!b.joinable());

  // A move-only callable.
  auto owned = std::make_unique<int>(3);
  ossia::thread c{[p = std::move(owned), &runs] { runs += *p; }};
  c.join();
  CHECK(runs == 4);

  ossia::thread d{[] {}};
  d.detach();
  CHECK(!d.joinable());
}
