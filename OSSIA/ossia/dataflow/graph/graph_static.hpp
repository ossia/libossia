#pragma once
#include <ossia/dataflow/graph/transitive_closure.hpp>
#include <boost/graph/transitive_closure.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_utils.hpp>

#include <boost/circular_buffer.hpp>
#include <boost/container/flat_map.hpp>

// #define OSSIA_GRAPH_DEBUG

namespace ossia
{
struct OSSIA_EXPORT graph_static_base
    : protected graph_util
    , public graph_base
{
  protected:
  static void tick_static(
      graph_static_base& g,
      execution_state& e,
      std::vector<graph_node*>& active_nodes)
  {
    for(auto node : active_nodes)
    {
      if(node->enabled())
      {
        assert(can_execute(*node, e));
        g.exec_node(*node, e);
      }
    }
  }
  static void tick_static(
      graph_static_base& g,
      execution_state& e,
      std::vector<graph_node*>& active_nodes,
      spdlog::logger& logger)
  {
    for(auto node : active_nodes)
    {
      if(node->enabled())
      {
        assert(can_execute(*node, e));
        if(!node->logged())
          g.exec_node(*node, e);
        else
          g.exec_node(*node, e, logger);
      }
    }
  }


public:
  ~graph_static_base() override
  {
    clear();
  }

  template<typename DevicesT>
  static auto find_address_connection(ossia::graph_node& source, ossia::graph_node& sink, const DevicesT& devices)
  {
    bool ok = false;
    for(const ossia::outlet_ptr& outlet : source.outputs())
    {
      for(const ossia::inlet_ptr& inlet : sink.inputs())
      {
        apply_to_destination(outlet->address, devices, [&] (ossia::net::parameter_base* p1, bool) {
          apply_to_destination(inlet->address, devices, [&] (ossia::net::parameter_base* p2, bool) {
            if(p1 == p2)
              ok = true;
          });
        });

        if(ok)
          break;
      }
      if(ok)
        break;
    }
    return ok;
  }

  void get_sorted_nodes(const graph_t& gr)
  {
    try {
      // Get a total order on nodes
      m_all_nodes.clear();
      m_all_nodes.reserve(m_nodes.size());

      // TODO this should be doable with a single vector
      m_topo_order_cache.clear();
      m_topo_order_cache.reserve(m_nodes.size());
      boost::topological_sort(gr, std::back_inserter(m_topo_order_cache));

      for(auto vtx : m_topo_order_cache)
      {
        auto node = gr[vtx].get();
        m_all_nodes.push_back(node);
      }
    }
    catch(...)
    {
      std::cout << "Error: graph isn't a DAG: ";
      print_graph(gr, std::cout);
      std::cout << std::endl;
    }
  }

  template<typename T>
  void compute_state(execution_state& e, T updateFun)
  {
    try
    {
      if(m_dirty)
      {
        updateFun(e.allDevices);
        m_dirty = false;
      }

      // Filter disabled nodes (through strict relationships).
      m_enabled_cache.reserve(m_nodes.size());

      for(auto node : m_all_nodes)
      {
        ossia::graph_node& ptr = *node;
        if(ptr.enabled()) {
          m_enabled_cache.insert(&ptr);
        }
        else
        {
          auto it = m_enabled_cache.find(&ptr);
          if(it != m_enabled_cache.end())
            m_enabled_cache.erase(it);
        }
      }

      disable_strict_nodes_rec(m_enabled_cache, m_disabled_cache);
      if(!logger)
        tick_static(*this, e, m_all_nodes);
      else
        tick_static(*this, e, m_all_nodes, *logger);

      finish_nodes(m_nodes);
    }
    catch (const boost::not_a_dag& e)
    {
      ossia::logger().error("Execution graph is not a DAG.");
      return;
    }
  }


  const graph_t& impl() const { return m_graph; }
  graph_t& impl() { return m_graph; }
  const graph_t& sub_impl() const { return m_sub_graph; }
  graph_t& sub_impl() { return m_sub_graph; }
  std::shared_ptr<spdlog::logger> logger;
  protected:
  void print(std::ostream& stream) override
  {
    print_graph(m_graph, stream);
  }

  node_flat_set m_enabled_cache;
  node_flat_set m_disabled_cache;
  std::vector<graph_node*> m_all_nodes;
  std::vector<graph_vertex_t> m_topo_order_cache;

  graph_t m_sub_graph;
  friend struct inlet;
  friend struct outlet;
  friend class ::DataflowTest;
};


struct bfs_graph final: public graph_static_base
{
public:
  bfs_graph()
  {
  }
  void state(execution_state& e) override
  {
    return graph_static_base::compute_state(e, [=] (auto& dev) { update_graph_bfs(dev); });
  }

  template<typename DevicesT>
  void update_graph_bfs(const DevicesT& devices)
  {
    const auto N = boost::num_vertices(m_graph);
    //m_color.clear();
    //m_color.reserve(N);
    m_sub_graph = m_graph;

    get_sorted_nodes(m_graph);
    // m_active_nodes is in topo order

    for(std::size_t i = 0; i < N; i++)
    {
      ossia::graph_node* n1 = m_all_nodes[i];
      for(std::size_t j = i + 1; j < N; j++)
      {
        ossia::graph_node* n2 = m_all_nodes[j];

        auto source_vtx = m_nodes.find(n1)->second;
        auto sink_vtx = m_nodes.find(n2)->second;
        if(find_path(source_vtx, sink_vtx, m_sub_graph))
          continue;
        if(find_path(sink_vtx, source_vtx, m_sub_graph))
          continue;

        if(find_address_connection(*n1, *n2, devices))
        {
          auto src_it = m_nodes.find(n1);
          auto sink_it = m_nodes.find(n2);
          assert(src_it != m_nodes.end());
          assert(sink_it != m_nodes.end());
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
        else if(find_address_connection(*n2, *n1, devices))
        {
          auto src_it = m_nodes.find(n2);
          auto sink_it = m_nodes.find(n1);
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
      }
    }

    get_sorted_nodes(m_sub_graph);
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
        if(u == node_to_find)
          ok = true;
        return ok;
      }
    } to_find{sink, ok};

    m_queue.clear();

    const auto N = boost::num_vertices(m_graph);
    if(m_queue.capacity() <= N)
      m_queue.set_capacity(N);

    m_color.resize(N);

    ossia::bfs::breadth_first_search_simple(graph, source, to_find, m_queue, m_color);
    return ok;
  }
private:
  boost::circular_buffer<graph_vertex_t> m_queue;

  using pmap_type = decltype(boost::make_two_bit_color_map_fast(0, get(boost::vertex_index, m_graph)));
  pmap_type make_pmap(std::size_t N)
  {
    using namespace boost;
    return make_two_bit_color_map_fast(N, get(vertex_index, m_graph));
  }
  pmap_type m_color{make_pmap(0)};
};

struct tc_graph final: public graph_static_base
{
public:
  using graph_static_base::graph_static_base;

  void state(execution_state& e) override
  {
    return graph_static_base::compute_state(e, [=] (auto& dev) { update_graph_tclosure(dev); });
  }

  using transitive_closure_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ossia::graph_node*, int64_t>;
  void compute_transitive_closure(const graph_t& sub_graph, transitive_closure_t& tclos)
  {
    tclos = transitive_closure_t{};
    ossia::transitive_closure(sub_graph, tclos, m_tcState);

#if defined(OSSIA_GRAPH_DEBUG)
    auto vertices = boost::vertices(sub_graph);
    for(auto i = vertices.first; i != vertices.second; i++)
    {
      tclos[*i] = sub_graph[*i].get();
      assert(tclos[*i]);
    }
    print_graph(tclos, std::cout);
#endif
  }

  template<typename DevicesT>
  void update_graph_tclosure(const DevicesT& devices)
  {
    m_sub_graph = m_graph;

    get_sorted_nodes(m_sub_graph);

    // m_active_nodes is in topo order

    // note: this is not enough.
    // eg consider
    //                 n1
    //              /       \  ..
    //  /a->n2->/b            /b->n3->/a
    // depending on the sort the connection may not happen, if n3 happens before n2
    //.. is it a problem ? we should just sort every "non-connected" node ?
    // What we do is : do the topo sort, and only add edges if they don't create cycles.
    // That is, if there is already path from n2 to n1 we don't add the edge.
    // The order is defined... by what ? maybe we have to run this every tick ? :(
    // If using the topo sort order (eg DFS) we can do it statically, else it's dynamically.

    // another case : [b -> c]   [a -> b]
    // if the first node occurs before the second there won't be any chaining, while we want to ensure that there
    // will be chainings.
    // So: for each pair: check if there is a path from one to the other.
    // Problem: [a -> b]  [b -> a] : which comes first ? one has to resolve the ambiguity manually.

    compute_transitive_closure(m_sub_graph, m_transitive_closure);

    for(std::size_t i = 0; i < m_all_nodes.size(); i++)
    {
      ossia::graph_node* n1 = m_all_nodes[i];
      for(std::size_t j = i + 1; j < m_all_nodes.size(); j++)
      {
        ossia::graph_node* n2 = m_all_nodes[j];

        auto source_vtx = m_nodes.find(n1)->second;
        auto sink_vtx = m_nodes.find(n2)->second;
        if(boost::edge(source_vtx, sink_vtx, m_transitive_closure).second)
          continue;
        if(boost::edge(sink_vtx, source_vtx, m_transitive_closure).second)
          continue;

        if(find_address_connection(*n1, *n2, devices))
        {
          auto src_it = m_nodes.find(n1);
          auto sink_it = m_nodes.find(n2);
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          compute_transitive_closure(m_sub_graph, m_transitive_closure);

#if defined(OSSIA_GRAPH_DEBUG)
          print_graph(transitive_closure, std::cout);
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
        else if(find_address_connection(*n2, *n1, devices))
        {
          auto src_it = m_nodes.find(n2);
          auto sink_it = m_nodes.find(n1);
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          compute_transitive_closure(m_sub_graph, m_transitive_closure);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
      }
    }

    get_sorted_nodes(m_sub_graph);
  }
private:
  transitive_closure_t m_transitive_closure;
  ossia::TransitiveClosureState<graph_t, transitive_closure_t> m_tcState;
};


struct boost_tc_graph final: public graph_static_base
{
public:
  using graph_static_base::graph_static_base;

  void state(execution_state& e) override
  {
    return graph_static_base::compute_state(e, [=] (auto& dev) { update_graph_tclosure(dev); });
  }

  using transitive_closure_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ossia::graph_node*, int64_t>;
  void compute_transitive_closure(const graph_t& sub_graph, transitive_closure_t& tclos)
  {
    tclos = transitive_closure_t{};
    boost::transitive_closure(sub_graph, tclos);

#if defined(OSSIA_GRAPH_DEBUG)
    auto vertices = boost::vertices(sub_graph);
    for(auto i = vertices.first; i != vertices.second; i++)
    {
      tclos[*i] = sub_graph[*i].get();
      assert(tclos[*i]);
    }
    print_graph(tclos, std::cout);
#endif
  }

  template<typename DevicesT>
  void update_graph_tclosure(const DevicesT& devices)
  {
    m_sub_graph = m_graph;

    get_sorted_nodes(m_sub_graph);

    // m_active_nodes is in topo order

    // note: this is not enough.
    // eg consider
    //                 n1
    //              /       \  ..
    //  /a->n2->/b            /b->n3->/a
    // depending on the sort the connection may not happen, if n3 happens before n2
    //.. is it a problem ? we should just sort every "non-connected" node ?
    // What we do is : do the topo sort, and only add edges if they don't create cycles.
    // That is, if there is already path from n2 to n1 we don't add the edge.
    // The order is defined... by what ? maybe we have to run this every tick ? :(
    // If using the topo sort order (eg DFS) we can do it statically, else it's dynamically.

    // another case : [b -> c]   [a -> b]
    // if the first node occurs before the second there won't be any chaining, while we want to ensure that there
    // will be chainings.
    // So: for each pair: check if there is a path from one to the other.
    // Problem: [a -> b]  [b -> a] : which comes first ? one has to resolve the ambiguity manually.

    compute_transitive_closure(m_sub_graph, m_transitive_closure);

    for(std::size_t i = 0; i < m_all_nodes.size(); i++)
    {
      ossia::graph_node* n1 = m_all_nodes[i];
      for(std::size_t j = i + 1; j < m_all_nodes.size(); j++)
      {
        ossia::graph_node* n2 = m_all_nodes[j];

        auto source_vtx = m_nodes.find(n1)->second;
        auto sink_vtx = m_nodes.find(n2)->second;
        if(boost::edge(source_vtx, sink_vtx, m_transitive_closure).second)
          continue;
        if(boost::edge(sink_vtx, source_vtx, m_transitive_closure).second)
          continue;

        if(find_address_connection(*n1, *n2, devices))
        {
          auto src_it = m_nodes.find(n1);
          auto sink_it = m_nodes.find(n2);
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          compute_transitive_closure(m_sub_graph, m_transitive_closure);

#if defined(OSSIA_GRAPH_DEBUG)
          print_graph(transitive_closure, std::cout);
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
        else if(find_address_connection(*n2, *n1, devices))
        {
          auto src_it = m_nodes.find(n2);
          auto sink_it = m_nodes.find(n1);
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          compute_transitive_closure(m_sub_graph, m_transitive_closure);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
      }
    }

    get_sorted_nodes(m_sub_graph);
  }
private:
  transitive_closure_t m_transitive_closure;
};
}
