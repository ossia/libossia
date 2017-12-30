#pragma once
#include <ossia/dataflow/graph_static.hpp>

namespace ossia
{
template<typename Ordering>
struct OSSIA_EXPORT graph_base
    : private graph_util
{
public:
  using Comparator = node_sorter<Ordering>;

  template<typename Comp_T>
  static void tick(
      graph_base& g,
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

  void get_enabled_nodes(const graph_t& gr)
  {
    m_active_nodes.clear();

    auto vtx = boost::vertices(gr);
    m_active_nodes.reserve(m_nodes.size());

    for(auto it = vtx.first; it != vtx.second; ++it)
    {
      auto node = gr[*it].get();
      if(node->enabled())
        m_active_nodes.push_back(node);
    }
  }


public:
  void mark_dirty()
  {
    m_topo_dirty = true;
  }
  ~graph_base()
  {
    clear();
  }

  void add_node(node_ptr n)
  {
    if(m_nodes.find(n) == m_nodes.end())
    {
      auto vtx = boost::add_vertex(n, m_graph);
      m_nodes.insert({std::move(n), vtx});
      m_topo_dirty = true;
    }
  }

  void remove_node(const node_ptr& n)
  {
    for(auto& port : n->inputs())
      for(auto edge : port->sources)
        disconnect(edge);
    for(auto& port : n->outputs())
      for(auto edge : port->targets)
        disconnect(edge);

    auto it = m_nodes.find(n);
    if (it != m_nodes.end())
    {
      auto vtx = boost::vertices(m_graph);
      if(std::find(vtx.first, vtx.second, it->second) != vtx.second)
      {
        boost::clear_vertex(it->second, m_graph);
        boost::remove_vertex(it->second, m_graph);
      }
      m_nodes.erase(it);
    }
    m_topo_dirty = true;
  }

  void connect(const std::shared_ptr<graph_edge>& edge)
  {
    if(edge)
    {
      edge->init();
      auto it1 = m_nodes.find(edge->in_node);
      auto it2 = m_nodes.find(edge->out_node);
      if (it1 != m_nodes.end() && it2 != m_nodes.end())
      {
        // TODO check that two edges can be added
        auto res = boost::add_edge(it1->second, it2->second, edge, m_graph);
        if (res.second)
        {
          m_edges.insert({std::move(edge), res.first});
        }
        m_topo_dirty = true;
      }
    }
  }
  void disconnect(const std::shared_ptr<graph_edge>& edge)
  {
    disconnect(edge.get());
  }
  void disconnect(graph_edge* edge)
  {
    if(edge)
    {
      auto it = m_edges.find(edge);
      if (it != m_edges.end())
      {
        auto edg = boost::edges(m_graph);
        if(std::find(edg.first, edg.second, it->second) != edg.second)
          boost::remove_edge(it->second, m_graph);
        m_topo_dirty = true;
        m_edges.erase(it);
      }
      edge->clear();
    }
  }

  void clear()
  {
    // TODO clear all the connections, ports, etc, to ensure that there is no
    // shared_ptr loop
    for (auto& node : m_nodes)
    {
      node.first->clear();
    }
    for (auto& edge : m_edges)
    {
      edge.first->clear();
    }
    m_topo_dirty = true;
    m_nodes.clear();
    m_edges.clear();
    m_graph.clear();
  }

  void state()
  {
    execution_state e;
    state(e);
    e.commit();
  }

  void state(execution_state& e)
  {
    try
    {
      // TODO in the future, temporal_graph, space_graph that can be used as
      // processes.

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
      if constexpr(std::is_same<Ordering, topological_ordering>::value)
      {
        get_sorted_nodes(m_graph);
        tick(*this, e, m_active_nodes, node_sorter<>{topological_ordering{m_active_nodes}, e});
      }
      else
      {
        get_enabled_nodes(m_graph);
        tick(*this, e, m_active_nodes, node_sorter<Ordering>{Ordering{}, e});
      }

      finish_nodes(m_nodes);
    }
    catch (const boost::not_a_dag& e)
    {
      ossia::logger().error("Execution graph is not a DAG.");
      return;
    }
  }

  const graph_t& impl() const { return m_graph; }
  private:

  node_map m_nodes;
  edge_map m_edges;

  graph_t m_graph;

  node_flat_set m_enabled_cache;
  node_flat_set m_disabled_cache;
  std::vector<graph_node*> m_active_nodes;
  std::vector<graph_vertex_t> m_topo_order_cache;
  bool m_topo_dirty{};

  friend struct inlet;
  friend struct outlet;
  friend class ::DataflowTest;
};




class graph : public graph_base<topological_ordering>
{ public: using graph_base<topological_ordering>::graph_base; };



}
