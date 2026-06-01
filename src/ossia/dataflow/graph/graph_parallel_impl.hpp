#pragma once
#include <ossia/dataflow/exec_pool.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/detail/fmt.hpp>

#include <smallfun.hpp>

#include <atomic>
#include <vector>

namespace ossia
{
using task_function = smallfun::function<void(ossia::graph_node&), sizeof(void*) * 4>;

class taskflow;
class executor;

// A graph node wrapped as a DAG task. Readiness is tracked by
// m_remaining_dependencies: when it reaches zero the task is submitted to the
// shared pool (or run inline). Memory ordering is carried entirely by the
// acq_rel RMW chain on that counter, so no explicit fences are needed: the
// thread observing the 1->0 transition happens-after every predecessor's
// decrement, hence sees all predecessor buffer writes.
class task : public ossia::pool_task
{
public:
  task() = default;
  task(const task&) = delete;
  task(task&& other) noexcept
      : pool_task{other}
      , m_taskId{other.m_taskId}
      , m_dependencies{other.m_dependencies}
      , m_remaining_dependencies{other.m_remaining_dependencies.load()}
      , m_node{other.m_node}
      , m_exec{other.m_exec}
      , m_precedes{std::move(other.m_precedes)}
  {
    other.m_precedes.clear();
  }
  task& operator=(const task&) = delete;
  task& operator=(task&& other) noexcept
  {
    static_cast<pool_task&>(*this) = other;
    m_taskId = other.m_taskId;
    m_dependencies = other.m_dependencies;
    m_remaining_dependencies = other.m_remaining_dependencies.load();
    m_node = other.m_node;
    m_exec = other.m_exec;
    m_precedes = std::move(other.m_precedes);
    other.m_precedes.clear();
    return *this;
  }

  task(ossia::graph_node& node)
      : m_node{&node}
  {
  }

  void precede(task& other)
  {
    m_precedes.push_back(other.m_taskId);
    other.m_dependencies++;
  }

private:
  friend class taskflow;
  friend class executor;

  int m_taskId{};
  int m_dependencies{0};
  std::atomic_int m_remaining_dependencies{};

  ossia::graph_node* m_node{};
  executor* m_exec{};
  ossia::small_pod_vector<int, 4> m_precedes;
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

// Drives a taskflow over the process-wide ossia::task_pool. Holds no threads of
// its own; rebuilding the graph no longer churns the worker set.
class executor
{
public:
  executor() = default;

  void set_task_executor(task_function f) { m_func = std::move(f); }

  void run(taskflow& tf)
  {
    m_tf = &tf;
    if(tf.m_tasks.empty())
      return;

    auto& pool = ossia::task_pool::instance();

    m_batch.remaining.store(
        static_cast<int>(tf.m_tasks.size()), std::memory_order_relaxed);

    for(auto& task : tf.m_tasks)
    {
      task.execute = &executor::run_task;
      task.m_exec = this;
      task.m_remaining_dependencies.store(
          task.m_dependencies, std::memory_order_relaxed);
    }

    for(auto& task : tf.m_tasks)
    {
      if(task.m_dependencies == 0)
        enqueue(task);
    }

    pool.corun_until(m_batch);
  }

private:
  // pool_task entry point: recover the owning executor from the task itself.
  static void run_task(ossia::pool_task* pt) noexcept
  {
    auto* t = static_cast<task*>(pt);
    t->m_exec->execute(*t);
  }

  void enqueue(task& t)
  {
    auto& pool = ossia::task_pool::instance();
    if(t.m_node->not_threadable())
      pool.submit_owner_only(&t);
    else
      pool.submit(&t);
  }

  void execute(task& t) noexcept
  {
    try
    {
      m_func(*t.m_node);
    }
    catch(const std::exception& e)
    {
      fmt::print(
          stderr, "ossia: error executing node '{}': {}\n", t.m_node->label(),
          e.what());
    }
    catch(...)
    {
      fmt::print(
          stderr, "ossia: error executing node '{}'\n", t.m_node->label());
    }

    process_done(t);
  }

  void process_done(task& t) noexcept
  {
    auto& pool = ossia::task_pool::instance();
    for(int taskId : t.m_precedes)
    {
      auto& next = m_tf->m_tasks[taskId];
      // acq_rel: the decrement that observes 1->0 happens-after all sibling
      // decrements, so `next` sees every predecessor's writes when it runs.
      if(next.m_remaining_dependencies.fetch_sub(1, std::memory_order_acq_rel)
         == 1)
      {
        enqueue(next);
      }
    }
    pool.finish(m_batch);
  }

  task_function m_func;
  taskflow* m_tf{};
  ossia::task_batch m_batch;
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
