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

  //! Runs a node outside of any taskflow, on the calling thread.
  void run_inline(ossia::graph_node& node) noexcept { call(node); }

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

  void call(ossia::graph_node& node) noexcept
  {
    try
    {
      m_func(node);
    }
    catch(const std::exception& e)
    {
      fmt::print(stderr, "ossia: error executing node '{}': {}\n", node.label(), e.what());
    }
    catch(...)
    {
      fmt::print(stderr, "ossia: error executing node '{}'\n", node.label());
    }
  }

  void execute(task& t) noexcept
  {
    call(*t.m_node);
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
  std::shared_ptr<bench_state> perf_map;

  template <typename Graph_T>
  custom_parallel_update(Graph_T& g, const ossia::graph_setup_options& opt)
      : impl{g, opt}
  {
  }

  //! The nodes without ports, the states, are not tasks: each tick runs them in
  //! order on the calling thread before the others, so that what they push to
  //! an address reaches the nodes reading it in the same tick. They have no
  //! edge, having no ports; were one given an edge, every node becomes a task
  //! again and they run in parallel with the others.
  void update_graph(
      ossia::node_map& nodes, const std::vector<graph_node*>& topo_order,
      std::size_t nodes_without_ports, ossia::graph_t& graph)
  {
    flow_nodes.clear();
    flow_graph.clear();
    prologue.clear();

    flow_graph.reserve(nodes.size());

    std::size_t first = std::min(nodes_without_ports, topo_order.size());
    if(first > 0)
    {
      const auto has_ports = [](const ossia::graph_node& n) {
        return !(n.root_inputs().empty() && n.root_outputs().empty());
      };
      for(auto [ei, ei_end] = boost::edges(graph); ei != ei_end; ++ei)
      {
        if(!has_ports(*graph[(*ei).m_source]) || !has_ports(*graph[(*ei).m_target]))
        {
          first = 0;
          break;
        }
      }
    }
    prologue.assign(topo_order.begin(), topo_order.begin() + first);

    if(logger)
    {
      if(perf_map)
      {
        executor.set_task_executor(
            node_exec_logger_bench{cur_state, *perf_map, *logger});
      }
      else
      {
        executor.set_task_executor(node_exec_logger{cur_state, *logger});
      }
    }
    else if(perf_map)
    {
      executor.set_task_executor(node_exec_bench{cur_state, *perf_map});
    }
    else
    {
      executor.set_task_executor(node_exec{cur_state});
    }

    for(std::size_t i = first; i < topo_order.size(); i++)
    {
      auto node = topo_order[i];
      flow_nodes[node] = flow_graph.emplace(*node);
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
    update_graph(g.m_nodes, g.m_all_nodes, g.m_nodes_without_ports, impl.m_sub_graph);
  }

private:
  friend struct custom_parallel_exec;

  Impl impl;
  execution_state* cur_state{};

  ossia::taskflow flow_graph;
  ossia::executor executor;
  ossia::hash_map<graph_node*, ossia::task*> flow_nodes;
  //! The nodes run in order before the taskflow.
  std::vector<ossia::graph_node*> prologue;
};

struct custom_parallel_exec
{
  //! Runs the nodes without ports first, in order, then the others in any order
  //! their edges allow, several at once.
  static constexpr bool in_order = false;

  template <typename Graph_T>
  custom_parallel_exec(Graph_T&)
  {
  }

  template <typename Graph_T, typename Impl>
  void operator()(
      Graph_T& g, custom_parallel_update<Impl>& self, ossia::execution_state& e,
      std::span<ossia::graph_node* const>)
  {
    self.cur_state = &e;
    if(!self.prologue.empty())
    {
      for(auto node : self.prologue)
        self.executor.run_inline(*node);
      e.receive_new_values();
    }
    self.executor.run(self.flow_graph);
  }
};

using custom_parallel_tc_graph
    = graph_static<custom_parallel_update<tc_update<fast_tc>>, custom_parallel_exec>;
}
