#pragma once

// TSAN seems to have some trouble with ReaderWriterQueue...
#if !defined(OSSIA_FREESTANDING)

#include <blockingconcurrentqueue.h>
#include <concurrentqueue.h>

// SPSC
#if defined(__has_feature)
#if __has_feature(thread_sanitizer)
namespace ossia
{
template <typename T, size_t MAX_BLOCK_SIZE = 512>
using spsc_queue = moodycamel::ConcurrentQueue<T>;
}
#else
#include <readerwriterqueue.h>
namespace ossia
{
template <typename T, size_t MAX_BLOCK_SIZE = 512>
using spsc_queue = moodycamel::ReaderWriterQueue<T, MAX_BLOCK_SIZE>;
}
#endif
#else
#include <readerwriterqueue.h>
namespace ossia
{
template <typename T, size_t MAX_BLOCK_SIZE = 512>
using spsc_queue = moodycamel::ReaderWriterQueue<T, MAX_BLOCK_SIZE>;
}
#endif

// MPMC
namespace ossia
{
template <typename T>
using mpmc_queue = moodycamel::ConcurrentQueue<T>;
template <typename T>
using blocking_mpmc_queue = moodycamel::BlockingConcurrentQueue<T>;
}

#else
// Will only be used on one thread anyways
#include <vector>
namespace ossia
{
template <typename T>
struct minimal_queue
{
public:
  bool try_dequeue(T& t)
  {
    if(impl.empty())
      return false;
    t = std::move(impl.back());
    impl.pop_back();
    return true;
  }

  void enqueue(T&& t) { impl.insert(impl.begin(), std::move(t)); }
  int size_approx() const noexcept { return impl.size(); }

private:
  std::vector<T> impl;
};
template <typename T>
using spsc_queue = ossia::minimal_queue<T>;
template <typename T>
using mpmc_queue = ossia::minimal_queue<T>;
template <typename T>
using blocking_mpmc_queue = ossia::minimal_queue<T>;
}
#endif
