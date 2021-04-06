#pragma once
#include <ossia/dataflow/graph/graph_utils.hpp>

namespace ossia
{
struct node_exec
{
  execution_state*& g;
  graph_node& node;

  void operator()()
  try
  {
    if (node.enabled())
    {
      assert(graph_util::can_execute(node, *g));
      graph_util::exec_node(node, *g);
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
  }
};
struct node_exec_bench
{
  execution_state*& g;
  graph_node& node;
  bench_map& perf;

  void operator()()
  try
  {
    if (perf.measure)
    {
      if (node.enabled())
      {
        assert(graph_util::can_execute(node, *g));

        auto t0 = std::chrono::steady_clock::now();
        graph_util::exec_node(node, *g);
        auto t1 = std::chrono::steady_clock::now();
        perf[&node]
            = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0)
                  .count();
      }
      else
      {
        perf[&node] = 0;
      }
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
  }
};

struct node_exec_logger
{
  execution_state*& g;
  spdlog::logger& logger;
  graph_node& node;

  void operator()()
  try
  {
    if (node.enabled())
    {
      assert(graph_util::can_execute(node, *g));
      if (!node.logged())
        graph_util::exec_node(node, *g);
      else
        graph_util::exec_node(node, *g, logger);
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
  }
};

struct node_exec_logger_bench
{
  execution_state*& g;
  bench_map& perf;
  spdlog::logger& logger;
  graph_node& node;

  void operator()()
  try
  {
    if (perf.measure)
    {
      if (node.enabled())
      {
        assert(graph_util::can_execute(node, *g));

        auto t0 = std::chrono::steady_clock::now();
        if (!node.logged())
          graph_util::exec_node(node, *g);
        else
          graph_util::exec_node(node, *g, logger);
        auto t1 = std::chrono::steady_clock::now();
        perf[&node]
            = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0)
                  .count();
      }
      else
      {
        perf[&node] = 0;
      }
    }
    else
    {
      if (!node.logged())
        graph_util::exec_node(node, *g);
      else
        graph_util::exec_node(node, *g, logger);
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
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
    for (auto node : active_nodes)
    {
      if (node->enabled())
      {
        assert(graph_util::can_execute(*node, e));
        graph_util::exec_node(*node, e);
      }
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
  }
};

struct static_exec_bench
{
  std::shared_ptr<bench_map> perf;
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
    auto& p = *perf;
    if (p.measure)
    {
      for (auto node : active_nodes)
      {
        if (node->enabled())
        {
          assert(graph_util::can_execute(*node, e));
          auto t0 = std::chrono::steady_clock::now();
          graph_util::exec_node(*node, e);
          auto t1 = std::chrono::steady_clock::now();
          p[node]
              = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0)
                    .count();
        }
        else
        {
          p[node] = 0;
        }
      }
    }
    else
    {
      for (auto node : active_nodes)
      {
        if (node->enabled())
        {
          assert(graph_util::can_execute(*node, e));
          graph_util::exec_node(*node, e);
        }
      }
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
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

  std::shared_ptr<bench_map> perf;
  std::shared_ptr<spdlog::logger> logger;
  template <typename Graph_T, typename Impl_T>
  void operator()(
      Graph_T& g, Impl_T& impl, execution_state& e,
      std::vector<graph_node*>& active_nodes)
  try
  {
    for (auto node : active_nodes)
    {
      if (node->enabled())
      {
        assert(graph_util::can_execute(*node, e));
        if (!node->logged())
          graph_util::exec_node(*node, e);
        else
          graph_util::exec_node(*node, e, *logger);
      }
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
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

  std::shared_ptr<bench_map> perf;
  std::shared_ptr<spdlog::logger> logger;

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
    auto& p = *perf;
    if (p.measure)
    {
      for (auto node : active_nodes)
      {
        if (node->enabled())
        {
          assert(graph_util::can_execute(*node, e));
          auto t0 = std::chrono::steady_clock::now();
          if (!node->logged())
            graph_util::exec_node(*node, e);
          else
            graph_util::exec_node(*node, e, *logger);
          auto t1 = std::chrono::steady_clock::now();
          p[node]
              = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0)
                    .count();
        }
        else
        {
          p[node] = 0;
        }
      }
    }
    else
    {
      for (auto node : active_nodes)
      {
        if (node->enabled())
        {
          assert(graph_util::can_execute(*node, e));
          if (!node->logged())
            graph_util::exec_node(*node, e);
          else
            graph_util::exec_node(*node, e, *logger);
        }
      }
    }
  }
  catch(...)
  {
    std::cerr << "Error while executing a node\n";
  }
};
}
