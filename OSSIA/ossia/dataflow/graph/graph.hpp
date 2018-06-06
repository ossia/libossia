#pragma once
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>

namespace ossia
{

class OSSIA_EXPORT graph final
    : private graph_util
    , public graph_base
{
public:
  template<typename Comp_T>
  static void tick(
      graph& g,
      execution_state& e,
      std::vector<graph_node*>& active_nodes,
      Comp_T&& comp)
  {
    std::size_t executed = 0;
    while (executed != active_nodes.size())
    {
      // Find all the nodes for which the inlets have executed
      // (or without cables on the inlets)

      auto end = active_nodes.end();
      auto cur_it = end;
      for(auto it = active_nodes.begin(); it != end - executed; ++it)
      {
        auto node = *it;
        if(cur_it != end)
        {
          if(!comp(*cur_it, node) && can_execute(*node, e))
            cur_it = it;
        }
        else
        {
          if(can_execute(*node, e))
          {
            cur_it = it;
          }
        }
      }

      if (cur_it != end)
      {
        g.exec_node(**cur_it, e);

        std::iter_swap(end - executed - 1, cur_it);
        executed++;
      }
      else
      {
        break; // nothing more to execute
      }
    }
  }


  template<typename Comp_T>
  static void tick(
      graph& g,
      execution_state& e,
      std::vector<graph_node*>& active_nodes,
      Comp_T&& comp,
      spdlog::logger& log)
  {
    std::size_t executed = 0;
    while (executed != active_nodes.size())
    {
      // Find all the nodes for which the inlets have executed
      // (or without cables on the inlets)

      auto end = active_nodes.end();
      auto cur_it = end;
      for(auto it = active_nodes.begin(); it != end - executed; ++it)
      {
        auto node = *it;
        if(cur_it != end)
        {
          if(!comp(*cur_it, node) && can_execute(*node, e))
            cur_it = it;
        }
        else
        {
          if(can_execute(*node, e))
          {
            cur_it = it;
          }
        }
      }

      if (cur_it != end)
      {
        ossia::graph_node& node = **cur_it;
        if(!node.logged())
          g.exec_node(node, e);
        else
          g.exec_node(node, e, log);

        std::iter_swap(end - executed - 1, cur_it);
        executed++;
      }
      else
      {
        break; // nothing more to execute
      }
    }
  }
  /*
  template<typename Comp_T>
  static void tick_ok(
      graph_base& g,
      execution_state& e,
      std::vector<graph_node*>& active_nodes,
      Comp_T&& comp)
  {
    while (!active_nodes.empty())
    {
      // Find all the nodes for which the inlets have executed
      // (or without cables on the inlets)
      const auto end = active_nodes.end();
      auto cur_it = end;
      for(auto it = active_nodes.begin(); it != end; ++it)
      {
        auto node = *it;
        if(can_execute(*node, e))
        {
          if(cur_it != end)
          {
            if(!comp(*cur_it, node))
              cur_it = it;
          }
          else
          {
            cur_it = it;
          }
        }
      }

      if (cur_it != end)
      {
        g.exec_node(**cur_it, e);
        active_nodes.erase(cur_it);
      }
      else
      {
        break; // nothing more to execute
      }
    }
  }*/

  /*
  template<typename Comp_T>
  static void tick_slow(
      graph_base& g,
      execution_state& e,
      std::vector<graph_node*>& active_nodes,
      Comp_T&& comp)
  {
    auto it = active_nodes.begin();
    auto end_it = active_nodes.end();

    while (it != end_it)
    {
      // Find all the nodes for which the inlets have executed
      // (or without cables on the inlets)
      auto cur_it = end_it;
      for(auto sub_it = it; sub_it != end_it; ++sub_it)
      {
        ossia::graph_node* node = *sub_it;
        if(can_execute(*node, e))
        {
          if(cur_it != end_it)
          {
            if(!comp(*cur_it, node))
              cur_it = sub_it;
          }
          else
          {
            cur_it = sub_it;
          }
        }
      }

      if (cur_it != end_it)
      {
        g.exec_node(**cur_it, e);
        std::iter_swap(it, cur_it);
        ++it;
      }
      else
      {
        break; // nothing more to execute
      }
    }
  }
  */

/*
  void get_sorted_nodes(const graph_t& gr)
  {
    // Get a total order on nodes
    m_active_nodes.clear();
    m_active_nodes.reserve(m_nodes.size());

    // TODO this should be doable with a single vector
    if(m_topo_dirty)
    {
      m_topo_order_cache.clear();
      m_topo_order_cache.reserve(m_nodes.size());
      boost::topological_sort(gr, std::back_inserter(m_topo_order_cache));
      m_topo_dirty = false;
    }

    for(auto vtx : m_topo_order_cache)
    {
      auto node = gr[vtx].get();
      if(node->enabled())
        m_active_nodes.push_back(node);
    }
  }
  */

  struct simple_topo_sort
  {
    simple_topo_sort(const graph_t& g): impl{g} {}
    const graph_t& impl;
    std::vector<graph_vertex_t> m_topo_order_cache;
    std::vector<graph_node*> m_node_cache;
    void operator()(const graph_t& gr, std::vector<graph_node*>& nodes)
    {
      const auto N = boost::num_vertices(impl);
      m_topo_order_cache.clear();
      m_topo_order_cache.reserve(N);
      boost::topological_sort(gr, std::back_inserter(m_topo_order_cache));

      nodes.clear();
      nodes.reserve(N);
      for(auto vtx : m_topo_order_cache)
      {
        nodes.push_back(gr[vtx].get());
      }
    }
  };

  void sort_nodes()
  {
    assert(sort_fun);
    assert(m_nodes.size() == boost::num_vertices(m_graph));

    sort_fun(m_graph, m_node_static_sort);
  }

  void get_enabled_nodes(const graph_t& gr)
  {
    m_active_nodes.clear();
    m_active_nodes.reserve(m_nodes.size());

    assert(m_node_static_sort.size() == boost::num_vertices(gr));
    for(auto node : m_node_static_sort)
    {
      if(node->enabled())
        m_active_nodes.push_back(node);
    }
  }

public:
  ~graph() override;

  void state(execution_state& e) override
  {
    try
    {
      // TODO in the future, temporal_graph, space_graph that can be used as
      // processes.
      if(m_dirty)
      {
        sort_nodes();
        m_dirty = false;
      }

      // Filter disabled nodes (through strict relationships).
      m_enabled_cache.clear();
      m_enabled_cache.reserve(m_nodes.size());

      for(auto it = boost::vertices(m_graph).first; it != boost::vertices(m_graph).second; ++it)
      {
        ossia::graph_node& ptr = *m_graph[*it];
        if(ptr.enabled()) {
          m_enabled_cache.insert(&ptr);
        }
      }

      disable_strict_nodes_rec(m_enabled_cache, m_disabled_cache);

      // Start executing the nodes
      get_enabled_nodes(m_graph);
      if(!logger)
        tick(*this, e, m_active_nodes, node_sorter{m_node_static_sort, e});
      else
        tick(*this, e, m_active_nodes, node_sorter{m_node_static_sort, e}, *logger);

      finish_nodes(m_nodes);
    }
    catch (const boost::not_a_dag&)
    {
      ossia::logger().error("Execution graph is not a DAG.");
      return;
    }
  }

  const graph_t& impl() const { return m_graph; }
  std::function<void(const graph_t& gr, std::vector<graph_node*>& nodes)> sort_fun{simple_topo_sort{m_graph}};

  std::shared_ptr<spdlog::logger> logger;
  private:
  void print(std::ostream& stream) override
  {
    print_graph(m_graph, stream);
  }

  node_flat_set m_enabled_cache;
  node_flat_set m_disabled_cache;
  std::vector<graph_node*> m_active_nodes;
  std::vector<graph_node*> m_node_static_sort;

  friend struct inlet;
  friend struct outlet;
  friend class ::DataflowTest;
};



}
