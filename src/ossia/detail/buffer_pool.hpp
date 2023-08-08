#pragma once
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/lockfree_queue.hpp>

namespace ossia
{
template <typename Obj_T>
struct object_pool
{
  mpmc_queue<Obj_T> buffers;

  Obj_T acquire()
  {
    Obj_T b;
    buffers.try_dequeue(b);
    return b;
  }

  void release(Obj_T b) { buffers.enqueue(std::move(b)); }
};

// TODO categorized_object_pool with more generic user-defined categorization ?

// given
// std::integer_sequence<int, 10, 100, 1000>;
// queues[0]: size € [0 , 9]
// queues[1]: size € [10, 99]
// queues[2]: size € [100, 999]
// queues[3]: size € [1000, +oo[

template <typename Obj_T, typename Alloc, std::size_t... sz>
struct sized_object_pool
{
  using SizeSpec = std::integer_sequence<std::size_t, sz...>;

  static const constexpr auto buckets = SizeSpec::size() + 1;
  std::array<mpmc_queue<Obj_T>, buckets> queues;

  template <std::size_t Sz, std::size_t... Szs>
  static constexpr int find_bucket_impl(std::size_t size, std::size_t k) noexcept
  {
    if(size < Sz)
      return k;
    else if constexpr(sizeof...(Szs) > 0)
      return find_bucket_impl<Szs...>(size, k + 1);
    else
      return k + 1;
  }

  static constexpr int find_bucket(std::size_t size) noexcept
  {
    return find_bucket_impl<sz...>(size, 0);
  }

  Obj_T acquire_sized(std::size_t req_size = 1024) noexcept
  {
    Obj_T b;
    if(!queues[find_bucket(req_size)].try_dequeue(b))
    {
      Alloc::resize(b, req_size);
    }
    return b;
  }

  Obj_T acquire_reserved(std::size_t req_size = 1024) noexcept
  {
    Obj_T b;
    if(!queues[find_bucket(req_size)].try_dequeue(b))
    {
      Alloc::reserve(b, req_size);
    }
    return b;
  }

  void release(Obj_T b) noexcept
  {
    queues[find_bucket(Alloc::size(b))].enqueue(std::move(b));
  }

  static sized_object_pool& instance() noexcept
  {
    static sized_object_pool p;
    return p;
  }
};

template <typename T>
struct container_memory_manager
{
  static constexpr void resize(T& t, std::size_t sz) noexcept { t.resize(sz); }
  static constexpr void reserve(T& t, std::size_t sz) noexcept { t.reserve(sz); }
  static constexpr std::size_t size(const T& t) noexcept { return t.size(); }
};

struct buffer_pool
{
  using buffer = ossia::pod_vector<char>;

  mpmc_queue<buffer> buffers;

  buffer acquire(std::size_t req_size = 1024)
  {
    buffer b;

    buffers.try_dequeue(b);
    b.resize(req_size, boost::container::default_init);
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
