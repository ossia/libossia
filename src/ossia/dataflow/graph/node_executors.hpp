#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>
#include <ossia/dataflow/telemetry.hpp>

#include <chrono>

namespace ossia
{
//! Runs a node, adding the time it took to its bench tap when measuring.
template <typename F>
inline void exec_timed(graph_node& node, bool measure, F&& exec)
{
  if(auto* tap = node.bench_tap.get(); measure && tap)
  {
    const auto t0 = std::chrono::steady_clock::now();
    exec();
    const auto t1 = std::chrono::steady_clock::now();
    tap->pending.add(
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
  }
  else
  {
    exec();
  }
}

struct node_exec
{
  execution_state*& g;

  void operator()(graph_node& node)
  try
  {
    if(node.enabled())
    {
      assert(graph_util::can_execute(node, *g));
      graph_util::exec_node(node, *g);
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};
struct node_exec_bench
{
  execution_state*& g;
  bench_state& perf;

  void operator()(graph_node& node)
  try
  {
    if(node.enabled())
    {
      assert(graph_util::can_execute(node, *g));
      exec_timed(node, perf.measure.load(std::memory_order_relaxed), [&] {
        graph_util::exec_node(node, *g);
      });
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};

struct node_exec_logger
{
  execution_state*& g;
  ossia::logger_type& logger;

  void operator()(graph_node& node)
  try
  {
    if(node.enabled())
    {
      assert(graph_util::can_execute(node, *g));
      if(!node.logged())
        graph_util::exec_node(node, *g);
      else
        graph_util::exec_node(node, *g, logger);
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};

struct node_exec_logger_bench
{
  execution_state*& g;
  bench_state& perf;
  ossia::logger_type& logger;

  void operator()(graph_node& node)
  try
  {
    if(node.enabled())
    {
      assert(graph_util::can_execute(node, *g));
      exec_timed(node, perf.measure.load(std::memory_order_relaxed), [&] {
        if(!node.logged())
          graph_util::exec_node(node, *g);
        else
          graph_util::exec_node(node, *g, logger);
      });
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};

struct static_exec
{
  template <typename Graph_T>
  static_exec(Graph_T&)
  {
  }

  template <typename T>
  void set_logger(const T&)
  {
  }
  template <typename T>
  void set_bench(const T&)
  {
  }

  template <typename Graph_T, typename Impl_T>
  void operator()(
      Graph_T& g, Impl_T& impl, execution_state& e,
      std::vector<graph_node*>& active_nodes)
  try
  {
    for(auto node : active_nodes)
    {
      if(node->enabled())
      {
        assert(graph_util::can_execute(*node, e));
        graph_util::exec_node(*node, e);
      }
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};

struct static_exec_bench
{
  std::shared_ptr<bench_state> perf;
  template <typename Graph_T>
  static_exec_bench(Graph_T&)
  {
  }

  template <typename T>
  void set_logger(const T&)
  {
  }
  template <typename T>
  void set_bench(const T& t)
  {
    perf = t;
  }

  template <typename Graph_T, typename Impl_T>
  void operator()(
      Graph_T& g, Impl_T& impl, execution_state& e,
      std::vector<graph_node*>& active_nodes)
  try
  {
    const bool measure = perf->measure.load(std::memory_order_relaxed);
    for(auto node : active_nodes)
    {
      if(node->enabled())
      {
        assert(graph_util::can_execute(*node, e));
        exec_timed(*node, measure, [&] { graph_util::exec_node(*node, e); });
      }
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};

struct static_exec_logger
{
  template <typename Graph_T>
  static_exec_logger(Graph_T&)
  {
  }
  template <typename T>
  void set_logger(const T& t)
  {
    logger = t;
  }
  template <typename T>
  void set_bench(const T& t)
  {
  }

  std::shared_ptr<bench_state> perf;
  std::shared_ptr<ossia::logger_type> logger;
  template <typename Graph_T, typename Impl_T>
  void operator()(
      Graph_T& g, Impl_T& impl, execution_state& e,
      std::vector<graph_node*>& active_nodes)
  try
  {
    for(auto node : active_nodes)
    {
      if(node->enabled())
      {
        assert(graph_util::can_execute(*node, e));
        if(!node->logged())
          graph_util::exec_node(*node, e);
        else
          graph_util::exec_node(*node, e, *logger);
      }
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};

struct static_exec_logger_bench
{
  template <typename T>
  void set_logger(const T& t)
  {
    logger = t;
  }
  template <typename T>
  void set_bench(const T& t)
  {
    perf = t;
  }

  std::shared_ptr<bench_state> perf;
  std::shared_ptr<ossia::logger_type> logger;

  template <typename Graph_T>
  static_exec_logger_bench(Graph_T&)
  {
  }
  template <typename Graph_T, typename Impl_T>
  void operator()(
      Graph_T& g, Impl_T& impl, execution_state& e,
      std::vector<graph_node*>& active_nodes)
  try
  {
    const bool measure = perf->measure.load(std::memory_order_relaxed);
    for(auto node : active_nodes)
    {
      if(node->enabled())
      {
        assert(graph_util::can_execute(*node, e));
        exec_timed(*node, measure, [&] {
          if(!node->logged())
            graph_util::exec_node(*node, e);
          else
            graph_util::exec_node(*node, e, *logger);
        });
      }
    }
  }
  catch(...)
  {
    ossia::logger().error("Error while executing a node");
  }
};
}
