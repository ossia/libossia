#include <ossia/detail/config.hpp>

#include <ossia/dataflow/exec_pool.hpp>

#include "../catch/include_catch.hpp"

#include <atomic>
#include <numeric>
#include <vector>

using namespace ossia;

TEST_CASE("fork_n runs each index exactly once", "[exec_pool]")
{
  auto& pool = task_pool::instance();

  for(int n : {0, 1, 2, 3, pool.worker_count(), pool.worker_count() + 1, 1000,
               10000})
  {
    std::vector<std::atomic_int> hits(std::max(1, n));
    pool.fork_n(n, [&](int i) { hits[i].fetch_add(1, std::memory_order_relaxed); });

    for(int i = 0; i < n; ++i)
      REQUIRE(hits[i].load() == 1);
  }
}

TEST_CASE("fork_n sums correctly under contention", "[exec_pool]")
{
  auto& pool = task_pool::instance();
  constexpr int N = 100000;

  std::atomic<int64_t> sum{0};
  pool.fork_n(N, [&](int i) { sum.fetch_add(i, std::memory_order_relaxed); });

  REQUIRE(sum.load() == int64_t(N - 1) * N / 2);
}

TEST_CASE("nested fork_n does not deadlock", "[exec_pool]")
{
  auto& pool = task_pool::instance();
  std::atomic_int total{0};

  // Outer tasks each launch an inner fork_n on the same pool. The work-
  // conserving join must keep the pool live even when workers block here.
  pool.fork_n(8, [&](int) {
    pool.fork_n(8, [&](int) { total.fetch_add(1, std::memory_order_relaxed); });
  });

  REQUIRE(total.load() == 64);
}

TEST_CASE("throwing task body neither hangs nor terminates", "[exec_pool]")
{
  auto& pool = task_pool::instance();
  std::atomic_int ran{0};

  pool.fork_n(16, [&](int i) {
    ran.fetch_add(1, std::memory_order_relaxed);
    if(i % 2 == 0)
      throw std::runtime_error("boom");
  });

  // All 16 slices completed (the join returned) despite half throwing.
  REQUIRE(ran.load() == 16);
}

TEST_CASE("repeated fork_n is stable", "[exec_pool]")
{
  auto& pool = task_pool::instance();
  for(int rep = 0; rep < 200; ++rep)
  {
    std::atomic_int c{0};
    pool.fork_n(64, [&](int) { c.fetch_add(1, std::memory_order_relaxed); });
    REQUIRE(c.load() == 64);
  }
}

int main(int argc, char** argv)
{
  return Catch::Session().run(argc, argv);
}
