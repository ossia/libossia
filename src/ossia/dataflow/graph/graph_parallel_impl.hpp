#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/detail/disable_fpe.hpp>
#include <ossia/detail/fmt.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/detail/thread.hpp>

#include <boost/container/static_vector.hpp>

#include <blockingconcurrentqueue.h>
#include <concurrentqueue.h>
#include <smallfun.hpp>

#include <thread>
#include <vector>
#define DISABLE_DONE_TASKS
//#define CHECK_FOLLOWS
//#define CHECK_EXEC_COUNTS
//#define memory_order_relaxed memory_order_seq_cst
//#define memory_order_acquire memory_order_seq_cst
//#define memory_order_release memory_order_seq_cst
namespace ossia
{
using task_function = smallfun::function<void(ossia::graph_node&), sizeof(void*) * 4>;

class taskflow;
class executor;
class task
{
public:
  task() = default;
  task(const task&) = delete;
  task(task&& other) noexcept
      : m_taskId{other.m_taskId}
      , m_dependencies{other.m_dependencies}
      , m_remaining_dependencies{other.m_remaining_dependencies.load()}
      , m_node{other.m_node}
      , m_precedes{std::move(other.m_precedes)}
#if defined(CHECK_FOLLOWS)
      , m_follows{std::move(other.m_follows)}
#endif
  {
    other.m_precedes.clear();
#if defined(CHECK_FOLLOWS)
    other.m_follows.clear();
#endif
  }
  task& operator=(const task&) = delete;
  task& operator=(task&& other) noexcept
  {
    m_taskId = other.m_taskId;
    m_dependencies = other.m_dependencies;
    m_remaining_dependencies = other.m_remaining_dependencies.load();
    m_node = other.m_node;
    m_precedes = std::move(other.m_precedes);
    other.m_precedes.clear();
#if defined(CHECK_FOLLOWS)
    m_follows = std::move(other.m_follows);
    other.m_follows.clear();
#endif
    return *this;
  }

  task(ossia::graph_node& node)
      : m_node{&node}
  {
  }

  void precede(task& other)
  {
    m_precedes.push_back(other.m_taskId);
#if defined(CHECK_FOLLOWS)
    other.m_follows.push_back(m_taskId);
#endif
    other.m_dependencies++;
  }

private:
  friend class taskflow;
  friend class executor;

  int m_taskId{};
  int m_dependencies{0};
  std::atomic_int m_remaining_dependencies{};
  std::atomic_bool m_executed{};

  ossia::graph_node* m_node{};
  ossia::small_pod_vector<int, 4> m_precedes;
#if defined(CHECK_FOLLOWS)
  ossia::small_pod_vector<int, 4> m_follows;
#endif
};

class taskflow
{
public:
  void clear() { m_tasks.clear(); }

  void reserve(std::size_t sz) { m_tasks.reserve(sz); }

  task* emplace(ossia::graph_node& node)
  {
    const int taskId = m_tasks.size();
    auto& last = m_tasks.emplace_back(node);
    last.m_taskId = taskId;
    return &last;
  }

private:
  friend class executor;

  std::vector<task> m_tasks;
};

class executor
{
public:
  explicit executor(int nthreads)
  {
    m_running = true;
    m_threads.resize(nthreads);
    int k = 0;
    for(auto& t : m_threads)
    {
      t = std::thread{[this, k = k++] {
        while(!m_startFlag.test())
          std::this_thread::yield();

        ossia::set_thread_name(m_threads[k], "ossia exec " + std::to_string(k));
        ossia::set_thread_realtime(m_threads[k], 95);
        ossia::set_thread_pinned(ossia::thread_type::AudioTask, k);

        ossia::disable_fpe();
        while(m_running)
        {
          task* t{};
          if(m_tasks.wait_dequeue_timed(t, 100))
          {
            execute(*t);
          }
        }
      }};
    }

    m_startFlag.test_and_set();
  }

  ~executor()
  {
    m_running = false;
    for(auto& t : m_threads)
    {
      t.join();
    }
  }

  void set_task_executor(task_function f) { m_func = std::move(f); }

  void enqueue_task(task& task)
  {
    if(task.m_node->not_threadable())
    {
      [[unlikely]];
      m_not_threadsafe_tasks.enqueue(&task);
    }
    else
    {
      [[likely]];
      m_tasks.enqueue(&task);
    }
  }

  void run(taskflow& tf)
  {
    m_tf = &tf;
    if(tf.m_tasks.empty())
    {
      return;
    }

    m_toDoTasks = tf.m_tasks.size();
    m_doneTasks.store(0, std::memory_order_relaxed);

    for(auto& task : tf.m_tasks)
    {
      task.m_remaining_dependencies.store(
          task.m_dependencies, std::memory_order_relaxed);
      task.m_executed.store(false, std::memory_order_relaxed);
#if defined(CHECK_EXEC_COUNTS)
      m_checkVec[task.m_taskId] = 0;
#endif
    }

    std::atomic_thread_fence(std::memory_order_seq_cst);
#if defined(DISABLE_DONE_TASKS)
    thread_local ossia::small_pod_vector<ossia::task*, 8> toCleanup;
#endif
    for(auto& task : tf.m_tasks)
    {
      if(task.m_dependencies == 0)
      {
#if defined(DISABLE_DONE_TASKS)
        if(task.m_node->enabled())
#endif
        {
#if defined(CHECK_EXEC_COUNTS)
          m_checkVec[task.m_taskId]++;
          if(m_checkVec[task.m_taskId] != 1)
          {
            fmt::print(
                stderr, "!!! task {} enqueued {}\n", task.m_taskId,
                m_checkVec[task.m_taskId]);
            std::abort();
          }
          assert(task.m_dependencies == 0);
#endif
          std::atomic_thread_fence(std::memory_order_release);
          enqueue_task(task);
        }
#if defined(DISABLE_DONE_TASKS)
        else
        {
          toCleanup.push_back(&task);
        }
#endif
      }
    }

#if defined(DISABLE_DONE_TASKS)
    for(auto& task : toCleanup)
    {
      process_done(*task);
    }
    toCleanup.clear();
#endif

    while(m_doneTasks.load(std::memory_order_relaxed) != m_toDoTasks)
    {
      task* t{};
      if(m_not_threadsafe_tasks.wait_dequeue_timed(t, 1))
      {
        execute(*t);
      }

      if(m_tasks.wait_dequeue_timed(t, 1))
      {
        execute(*t);
      }
    }

    std::atomic_thread_fence(std::memory_order_seq_cst);
  }

private:
  void process_done(ossia::task& task)
  {
    if(task.m_executed.exchange(true))
      return;
    assert(this->m_doneTasks != m_tf->m_tasks.size());

#if defined(DISABLE_DONE_TASKS)
    ossia::small_pod_vector<ossia::task*, 8> toCleanup;
#endif
    for(int taskId : task.m_precedes)
    {
      auto& nextTask = m_tf->m_tasks[taskId];
      assert(!nextTask.m_executed);

      std::atomic_int& remaining = nextTask.m_remaining_dependencies;
      assert(remaining > 0);
      const int rem = remaining.fetch_sub(1, std::memory_order_relaxed) - 1;
      assert(rem >= 0);
      if(rem == 0)
      {
#if defined(DISABLE_DONE_TASKS)
        if(nextTask.m_node->enabled())
#endif
        {
#if defined(CHECK_EXEC_COUNTS)
          m_checkVec[nextTask.m_taskId]++;
          if(m_checkVec[nextTask.m_taskId] != 1)
          {
            fmt::print(
                stderr, "!!! task {} enqueued {}\n", nextTask.m_taskId,
                m_checkVec[nextTask.m_taskId]);
            std::abort();
          }
#endif
          std::atomic_thread_fence(std::memory_order_release);
          enqueue_task(nextTask);
        }
#if defined(DISABLE_DONE_TASKS)
        else
        {
          toCleanup.push_back(&nextTask);
        }
#endif
      }
    }

#if defined(DISABLE_DONE_TASKS)
    for(auto& clean : toCleanup)
    {
      process_done(*clean);
    }
#endif

    this->m_doneTasks.fetch_add(1, std::memory_order_relaxed);
  }

  void execute(task& task)
  {
    std::atomic_thread_fence(std::memory_order_acquire);
    try
    {
      assert(!task.m_executed);
#if defined(CHECK_FOLLOWS)
      for(auto& prev : task.m_follows)
      {
        auto& t = m_tf->m_tasks[prev];
        assert(t.m_executed);
      }
#endif

#if defined(CHECK_EXEC_COUNTS)
      assert(m_checkVec[task.m_taskId] == 1);
#endif
      m_func(*task.m_node);

#if defined(CHECK_EXEC_COUNTS)
      assert(m_checkVec[task.m_taskId] == 1);
#endif
    }
    catch(...)
    {
      fmt::print(stderr, "error !\n");
    }
    std::atomic_thread_fence(std::memory_order_release);

    process_done(task);
#if defined(CHECK_EXEC_COUNTS)
    assert(m_checkVec[task.m_taskId] == 1);
#endif

    std::atomic_thread_fence(std::memory_order_release);
  }

  task_function m_func;

  std::atomic_bool m_running{};

  ossia::small_vector<std::thread, 8> m_threads;
  std::atomic_flag m_startFlag = ATOMIC_FLAG_INIT;

  taskflow* m_tf{};
  std::atomic_size_t m_doneTasks = 0;
  std::size_t m_toDoTasks = 0;

  moodycamel::BlockingConcurrentQueue<task*> m_tasks;
  moodycamel::BlockingConcurrentQueue<task*> m_not_threadsafe_tasks;

#if defined(CHECK_EXEC_COUNTS)
  std::array<std::atomic_int, 5000> m_checkVec;
#endif
};
}

#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/detail/hash_map.hpp>
namespace ossia
{
struct custom_parallel_exec;
template <typename Impl>
struct custom_parallel_update
{
public:
  std::shared_ptr<ossia::logger_type> logger;
  std::shared_ptr<bench_map> perf_map;

  template <typename Graph_T>
  custom_parallel_update(Graph_T& g, const ossia::graph_setup_options& opt)
      : impl{g, opt}
      , executor{opt.parallel_threads}
  {
  }

  void update_graph(
      ossia::node_map& nodes, const std::vector<graph_node*>& topo_order,
      ossia::graph_t& graph)
  {
    flow_nodes.clear();
    flow_graph.clear();

    flow_graph.reserve(nodes.size());

    if(logger)
    {
      if(perf_map)
      {
        executor.set_task_executor(
            node_exec_logger_bench{cur_state, *perf_map, *logger});
        for(auto node : topo_order)
        {
          (*perf_map)[node] = std::nullopt;
          flow_nodes[node] = flow_graph.emplace(*node);
        }
      }
      else
      {
        executor.set_task_executor(node_exec_logger{cur_state, *logger});
        for(auto node : topo_order)
        {
          flow_nodes[node] = flow_graph.emplace(*node);
        }
      }
    }
    else
    {
      executor.set_task_executor(node_exec{cur_state});
      for(auto node : topo_order)
      {
        flow_nodes[node] = flow_graph.emplace(*node);
      }
    }

    for(auto [ei, ei_end] = boost::edges(graph); ei != ei_end; ++ei)
    {
      auto edge = *ei;
      auto& n1 = graph[edge.m_source];
      auto& n2 = graph[edge.m_target];

      auto& sender = flow_nodes[n2.get()];
      auto& receiver = flow_nodes[n1.get()];
      sender->precede(*receiver);
    }
  }

  template <typename Graph_T, typename DevicesT>
  void operator()(Graph_T& g, const DevicesT& devices)
  {
    impl(g, devices);
    update_graph(g.m_nodes, g.m_all_nodes, impl.m_sub_graph);
  }

private:
  friend struct custom_parallel_exec;

  Impl impl;
  execution_state* cur_state{};

  ossia::taskflow flow_graph;
  ossia::executor executor;
  ossia::hash_map<graph_node*, ossia::task*> flow_nodes;
};

struct custom_parallel_exec
{
  template <typename Graph_T>
  custom_parallel_exec(Graph_T&)
  {
  }

  template <typename Graph_T, typename Impl>
  void operator()(
      Graph_T& g, custom_parallel_update<Impl>& self, ossia::execution_state& e,
      const std::vector<ossia::graph_node*>&)
  {
    self.cur_state = &e;
    self.executor.run(self.flow_graph);
  }
};

using custom_parallel_tc_graph
    = graph_static<custom_parallel_update<tc_update<fast_tc>>, custom_parallel_exec>;
}

//#undef memory_order_relaxed
//#undef memory_order_acquire
//#undef memory_order_release
