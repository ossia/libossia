#pragma once
#include <ossia/dataflow/bench_map.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>
#include <ossia/dataflow/graph/node_executors.hpp>
#include <ossia/dataflow/graph/transitive_closure.hpp>
#include <ossia/detail/flat_map.hpp>

#include <boost/circular_buffer.hpp>
#include <boost/graph/transitive_closure.hpp>

#include <ossia-config.hpp>

// #define OSSIA_GRAPH_DEBUG

namespace ossia
{

template <typename UpdateImpl, typename TickImpl>
struct graph_static final : public graph_util, public graph_base
{
public:
  UpdateImpl update_fun{*this};
  TickImpl tick_fun{*this};

  ~graph_static() override
  {
    clear();
  }

  void sort_all_nodes(const graph_t& gr)
  {
    try
    {
      // Get a total order on nodes
      m_all_nodes.clear();
      m_all_nodes.reserve(m_nodes.size());

      // TODO this should be doable with a single vector
      m_topo_order_cache.clear();
      m_topo_order_cache.reserve(m_nodes.size());
      boost::topological_sort(gr, std::back_inserter(m_topo_order_cache));

      for (auto vtx : m_topo_order_cache)
      {
        auto node = gr[vtx].get();
        assert(gr[vtx].get());
        m_all_nodes.push_back(node);
      }
    }
    catch (...)
    {
      std::cout << "Error: graph isn't a DAG: ";
      print_graph(gr, std::cout);
      std::cout << std::endl;
    }
  }

  void state(execution_state& e) override
  {
    try
    {
      if (m_dirty)
      {
        update_fun(*this, e.exec_devices());
        m_enabled_cache.clear();
        m_dirty = false;
      }

      // Filter disabled nodes (through strict relationships).
      m_enabled_cache.container.reserve(m_nodes.size());

      for (auto node : m_all_nodes)
      {
        ossia::graph_node& ptr = *node;
        if (ptr.enabled())
        {
          m_enabled_cache.insert(&ptr);
        }
        else
        {
          auto it = m_enabled_cache.find(&ptr);
          if (it != m_enabled_cache.end())
            m_enabled_cache.erase(it);
        }
      }

      disable_strict_nodes_rec(m_enabled_cache, m_disabled_cache);

      tick_fun(*this, update_fun, e, m_all_nodes);

      finish_nodes(m_nodes);
    }
    catch (const boost::not_a_dag&)
    {
      ossia::logger().error("Execution graph is not a DAG.");
      return;
    }
  }

  const graph_t& impl() const
  {
    return m_graph;
  }
  graph_t& impl()
  {
    return m_graph;
  }
  std::vector<graph_node*> m_all_nodes;

protected:
  void print(std::ostream& stream) override
  {
    print_graph(m_graph, stream);
  }

private:
  node_flat_set m_enabled_cache;
  node_flat_set m_disabled_cache;
  std::vector<graph_vertex_t> m_topo_order_cache;

  friend class ::DataflowTest;
};

struct simple_update
{
  ossia::graph_t& m_sub_graph;
  template <typename Graph_T>
  simple_update(Graph_T& g) : m_sub_graph{g.m_graph}
  {
  }

  template <typename Graph_T, typename DevicesT>
  void operator()(Graph_T& g, const DevicesT& devices)
  {
    g.sort_all_nodes(g.m_graph);
  }
};

struct bfs_update
{
public:
  template <typename Graph_T>
  bfs_update(Graph_T& g)
      : m_color{boost::make_two_bit_color_map_fast(
            0, boost::get(boost::vertex_index, g.m_graph))}
  {
  }

  template <typename Graph_T, typename DevicesT>
  void operator()(Graph_T& g, const DevicesT& devices)
  {
    auto& m_graph = g.m_graph;
    auto& m_nodes = g.m_nodes;
    auto& m_all_nodes = g.m_all_nodes;
    const auto N = boost::num_vertices(m_graph);
    // m_color.clear();
    // m_color.reserve(N);
    m_sub_graph = m_graph;

    g.sort_all_nodes(m_graph);
    // m_active_nodes is in topo order

    for (std::size_t i = 0; i < N; i++)
    {
      ossia::graph_node* n1 = m_all_nodes[i];
      for (std::size_t j = i + 1; j < N; j++)
      {
        ossia::graph_node* n2 = m_all_nodes[j];

        auto source_vtx = m_nodes.find(n1)->second;
        auto sink_vtx = m_nodes.find(n2)->second;
        if (find_path(source_vtx, sink_vtx, m_sub_graph))
          continue;
        if (find_path(sink_vtx, source_vtx, m_sub_graph))
          continue;

        if (graph_util::find_address_connection(*n1, *n2, devices))
        {
          auto src_it = m_nodes.find(n1);
          auto sink_it = m_nodes.find(n2);
          assert(src_it != m_nodes.end());
          assert(sink_it != m_nodes.end());
          auto edge = ossia::make_edge(
              ossia::dependency_connection{}, ossia::outlet_ptr{},
              ossia::inlet_ptr{}, src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          sort_all_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
        else if (graph_util::find_address_connection(*n2, *n1, devices))
        {
          auto src_it = m_nodes.find(n2);
          auto sink_it = m_nodes.find(n1);
          auto edge = ossia::make_edge(
              ossia::dependency_connection{}, ossia::outlet_ptr{},
              ossia::inlet_ptr{}, src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          sort_all_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
      }
    }

    g.sort_all_nodes(m_sub_graph);
  }

  bool find_path(graph_vertex_t source, graph_vertex_t sink, graph_t& graph)
  {
    bool ok = false;
    struct bfs_find_visitor
    {
      graph_vertex_t node_to_find{};
      bool& ok;
      bool discover_vertex(graph_vertex_t u, const graph_t&) const
      {
        if (u == node_to_find)
          ok = true;
        return ok;
      }
    } to_find{sink, ok};

    m_queue.clear();

    const auto N = boost::num_vertices(graph);
    if (m_queue.capacity() <= N)
      m_queue.set_capacity(N);

    m_color.resize(N);

    ossia::bfs::breadth_first_search_simple(
        graph, source, to_find, m_queue, m_color);
    return ok;
  }
  graph_t m_sub_graph;

private:
  boost::circular_buffer<graph_vertex_t> m_queue;

  using pmap_type = decltype(boost::make_two_bit_color_map_fast(
      0, get(boost::vertex_index, graph_t{})));

  pmap_type m_color;
};

template <typename Impl>
struct tc_update
{
  Impl impl;

public:
  template <typename Graph_T>
  tc_update(Graph_T& g)
  {
  }
  template <typename Graph_T, typename DevicesT>
  void operator()(Graph_T& g, const DevicesT& devices)
  {
    m_sub_graph = g.m_graph;

    g.sort_all_nodes(m_sub_graph);

    impl.update(m_sub_graph);

    tc_add_addresses(
        g.m_graph, m_sub_graph, g.m_nodes, g.m_all_nodes, impl, devices);

    g.sort_all_nodes(m_sub_graph);
  }

  graph_t m_sub_graph;

private:
  template <
      typename BaseGraph, typename TCGraph, typename NodeMap,
      typename AllNodes, typename TC, typename Devices>
  static void tc_add_addresses(
      BaseGraph& m_graph, TCGraph& m_sub_graph, NodeMap& m_nodes,
      AllNodes& m_all_nodes, TC& tc, Devices& devices)
  {
    // m_active_nodes is in topo order

    // note: this is not enough.
    // eg consider
    //                 n1
    //              /       \  ..
    //  /a->n2->/b            /b->n3->/a
    // depending on the sort the connection may not happen, if n3 happens
    // before n2
    //.. is it a problem ? we should just sort every "non-connected" node ?
    // What we do is : do the topo sort, and only add edges if they don't
    // create cycles. That is, if there is already path from n2 to n1 we don't
    // add the edge. The order is defined... by what ? maybe we have to run
    // this every tick ? :( If using the topo sort order (eg DFS) we can do it
    // statically, else it's dynamically.

    // another case : [b -> c]   [a -> b]
    // if the first node occurs before the second there won't be any chaining,
    // while we want to ensure that there will be chainings. So: for each pair:
    // check if there is a path from one to the other. Problem: [a -> b]  [b ->
    // a] : which comes first ? one has to resolve the ambiguity manually.

    for (std::size_t i = 0; i < m_all_nodes.size(); i++)
    {
      ossia::graph_node* n1 = m_all_nodes[i];
      for (std::size_t j = i + 1; j < m_all_nodes.size(); j++)
      {
        ossia::graph_node* n2 = m_all_nodes[j];

        auto source_vtx = m_nodes.find(n1)->second;
        auto sink_vtx = m_nodes.find(n2)->second;
        if (tc.has_edge(source_vtx, sink_vtx))
          continue;
        if (tc.has_edge(sink_vtx, source_vtx))
          continue;

        if (graph_util::find_address_connection(*n1, *n2, devices))
        {
          auto src_it = m_nodes.find(n1);
          auto sink_it = m_nodes.find(n2);
          auto edge = ossia::make_edge(
              ossia::dependency_connection{}, ossia::outlet_ptr{},
              ossia::inlet_ptr{}, src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          tc.update(m_sub_graph);

#if defined(OSSIA_GRAPH_DEBUG)
          print_graph(transitive_closure, std::cout);
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          sort_all_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
        else if (graph_util::find_address_connection(*n2, *n1, devices))
        {
          auto src_it = m_nodes.find(n2);
          auto sink_it = m_nodes.find(n1);
          auto edge = ossia::make_edge(
              ossia::dependency_connection{}, ossia::outlet_ptr{},
              ossia::inlet_ptr{}, src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          tc.update(m_sub_graph);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          sort_all_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
      }
    }
  }
};

struct fast_tc
{
public:
  using transitive_closure_t = boost::adjacency_list<
      boost::vecS, boost::vecS, boost::directedS, ossia::graph_node*, int64_t>;

  bool has_edge(int source_vtx, int sink_vtx) const
  {
    return boost::edge(source_vtx, sink_vtx, m_transitive_closure).second;
  }
  void update(const graph_t& sub_graph)
  {
    m_transitive_closure = transitive_closure_t{};
    ossia::transitive_closure(sub_graph, m_transitive_closure, m_tcState);

#if defined(OSSIA_GRAPH_DEBUG)
    auto vertices = boost::vertices(sub_graph);
    for (auto i = vertices.first; i != vertices.second; i++)
    {
      tclos[*i] = sub_graph[*i].get();
      assert(tclos[*i]);
    }
    print_graph(tclos, std::cout);
#endif
  }
  transitive_closure_t m_transitive_closure;
  ossia::TransitiveClosureState<graph_t, transitive_closure_t> m_tcState;
};

struct boost_tc
{
public:
  using transitive_closure_t = boost::adjacency_list<
      boost::vecS, boost::vecS, boost::directedS, ossia::graph_node*, int64_t>;

  bool has_edge(int source_vtx, int sink_vtx) const
  {
    return boost::edge(source_vtx, sink_vtx, m_transitive_closure).second;
  }

  void update(const graph_t& sub_graph)
  {
    m_transitive_closure = transitive_closure_t{};
    boost::transitive_closure(sub_graph, m_transitive_closure);

#if defined(OSSIA_GRAPH_DEBUG)
    auto vertices = boost::vertices(sub_graph);
    for (auto i = vertices.first; i != vertices.second; i++)
    {
      tclos[*i] = sub_graph[*i].get();
      assert(tclos[*i]);
    }
    print_graph(tclos, std::cout);
#endif
  }

private:
  transitive_closure_t m_transitive_closure;
};

using tc_graph = graph_static<tc_update<fast_tc>, static_exec>;
using bfs_graph = graph_static<bfs_update, static_exec>;

using logged_tc_graph = graph_static<tc_update<fast_tc>, static_exec_logger>;
}
