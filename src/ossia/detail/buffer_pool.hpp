#pragma once
#include <concurrentqueue.h>
#include <ossia/detail/pod_vector.hpp>
namespace ossia
{
struct buffer_pool
{
  template<typename T, size_t MAX_BLOCK_SIZE = 64>
  using mpmc_queue = moodycamel::ConcurrentQueue<T>;
  using buffer = ossia::pod_vector<char>;

  mpmc_queue<buffer> buffers;

  buffer acquire(std::size_t req_size = 1024)
  {
    buffer b;

    buffers.try_dequeue(b);
    b.resize(req_size);
    return b;
  }

  void release(buffer b)
  {
    b.clear();
    buffers.enqueue(std::move(b));
  }

  static buffer_pool& instance()
  {
    static buffer_pool p;
    return p;
  }
};

}
