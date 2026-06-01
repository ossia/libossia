#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/disable_fpe.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/thread.hpp>

#include <boost/container/static_vector.hpp>

#include <concurrentqueue.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <string>
#include <thread>

namespace ossia
{
struct pool_task
{
  void (*execute)(pool_task*) noexcept = nullptr;
};

struct task_batch
{
  std::atomic<int> remaining;
};

class task_pool
{
public:
  static task_pool& instance()
  {
    static task_pool pool;
    return pool;
  }

  int worker_count() const noexcept { return static_cast<int>(m_workers.size()); }

  void submit(pool_task* t) noexcept
  {
    if(!m_queue.try_enqueue(t))
    {
      t->execute(t);
      return;
    }
    wake();
  }

  void submit_owner_only(pool_task* t) noexcept
  {
    while(!m_owner_queue.try_enqueue(t))
      std::this_thread::yield();
    wake();
  }

  void finish(task_batch& b) noexcept
  {
    if(b.remaining.fetch_sub(1, std::memory_order_acq_rel) == 1)
      wake();
  }

  void corun_until(task_batch& b) noexcept
  {
    const bool owner = !t_is_worker;
    ++t_depth;
    assert(t_depth < 1024);
    while(b.remaining.load(std::memory_order_acquire) != 0)
    {
      if(try_run_one(owner))
        continue;

      const std::uint32_t e = m_epoch.load(std::memory_order_acquire);
      if(b.remaining.load(std::memory_order_acquire) == 0)
        break;
      if(try_run_one(owner))
        continue;
      m_epoch.wait(e, std::memory_order_acquire);
    }
    --t_depth;
  }

  template <typename F>
  void fork_n(int n, F&& f) noexcept
  {
    if(n <= 0)
      return;

    int nslices = std::min(n, worker_count() + 1);
    if(nslices > MAX_SLICES)
      nslices = MAX_SLICES;

    struct slice : pool_task
    {
      std::remove_reference_t<F>* fn = nullptr;
      int lo = 0, hi = 0;
      task_batch* batch = nullptr;
      task_pool* pool = nullptr;
    };

    boost::container::static_vector<slice, MAX_SLICES> slices(nslices);
    task_batch batch;
    batch.remaining.store(nslices, std::memory_order_relaxed);

    constexpr auto run_slice = [](pool_task* pt) noexcept {
      auto* s = static_cast<slice*>(pt);
      for(int i = s->lo; i < s->hi; ++i)
      {
        try
        {
          (*s->fn)(i);
        }
        catch(...)
        {
        }
      }
      s->pool->finish(*s->batch);
    };

    const int base = n / nslices;
    const int rem = n % nslices;
    int pos = 0;
    for(int s = 0; s < nslices; ++s)
    {
      const int count = base + (s < rem ? 1 : 0);
      slice& sl = slices[s];
      sl.execute = +run_slice;
      sl.fn = &f;
      sl.lo = pos;
      sl.hi = pos + count;
      sl.batch = &batch;
      sl.pool = this;
      pos += count;
    }
    assert(pos == n);

    for(int s = 0; s < nslices; ++s)
      submit(&slices[s]);

    corun_until(batch);
  }

  task_pool(const task_pool&) = delete;
  task_pool& operator=(const task_pool&) = delete;

private:
  static constexpr int MAX_SLICES = 64;

  task_pool()
  {
    int W = default_worker_count();

    m_running.store(true, std::memory_order_relaxed);
    m_workers.reserve(W);
    for(int k = 0; k < W; ++k)
      m_workers.emplace_back([this, k] { worker_run(k); });

    for(auto& t : m_workers)
      ossia::set_thread_realtime(t, 95);

    m_start.test_and_set();
  }

  ~task_pool()
  {
    m_running.store(false, std::memory_order_release);
    wake();
    for(auto& t : m_workers)
      if(t.joinable())
        t.join();
  }

  static int default_worker_count() noexcept
  {
    const auto& specs = ossia::get_thread_specs();
    if(auto it = specs.find(ossia::thread_type::AudioTask);
       it != specs.end() && it->second.num_threads > 1)
      return it->second.num_threads;

    const unsigned hw = std::thread::hardware_concurrency();
    return static_cast<int>(hw > 2 ? hw - 1 : 1);
  }

  bool try_run_one(bool owner) noexcept
  {
    pool_task* t = nullptr;
    if(owner && m_owner_queue.try_dequeue(t))
    {
      t->execute(t);
      return true;
    }
    if(m_queue.try_dequeue(t))
    {
      t->execute(t);
      return true;
    }
    return false;
  }

  void wake() noexcept
  {
    m_epoch.fetch_add(1, std::memory_order_release);
    m_epoch.notify_all();
  }

  void worker_run(int k) noexcept
  {
    while(!m_start.test())
      std::this_thread::yield();

    t_is_worker = true;
    ossia::set_thread_name("ossia exec " + std::to_string(k));
    ossia::set_thread_pinned(ossia::thread_type::AudioTask, k);
    ossia::disable_fpe();

    while(m_running.load(std::memory_order_acquire))
    {
      if(try_run_one(false))
        continue;

      const std::uint32_t e = m_epoch.load(std::memory_order_acquire);
      if(!m_running.load(std::memory_order_acquire))
        break;
      if(try_run_one(false))
        continue;
      m_epoch.wait(e, std::memory_order_acquire);
    }
  }

  moodycamel::ConcurrentQueue<pool_task*> m_queue{4096};
  moodycamel::ConcurrentQueue<pool_task*> m_owner_queue{1024};

  std::atomic<std::uint32_t> m_epoch{0};
  std::atomic<bool> m_running{false};
  std::atomic_flag m_start = ATOMIC_FLAG_INIT;

  ossia::small_vector<std::thread, 16> m_workers;

  static inline thread_local bool t_is_worker = false;
  static inline thread_local int t_depth = 0;
};
}
