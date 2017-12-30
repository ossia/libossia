#pragma once
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/breadth_first_search.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/range/adaptors.hpp>
#include <ossia/dataflow/graph_ordering.hpp>
#include <boost/container/flat_set.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/transitive_closure.hpp>
class DataflowTest;
namespace ossia
{

using graph_t = boost::
    adjacency_list<boost::vecS, boost::vecS, boost::directedS, node_ptr, std::shared_ptr<graph_edge>>;

using graph_vertex_t = graph_t::vertex_descriptor;
using graph_edge_t = graph_t::edge_descriptor;

using node_map = ossia::shared_ptr_map<ossia::graph_node, graph_vertex_t>;
using edge_map = ossia::shared_ptr_map<ossia::graph_edge, graph_edge_t>;

using node_flat_set = boost::container::flat_set<graph_node*>;
enum class node_ordering
{
  topological, temporal, hierarchical
};

template<typename Graph_T, typename IO>
void print_graph(Graph_T& g, IO& stream)
{
  std::stringstream s;
  boost::write_graphviz(s, g, [&] (auto& out, auto v) {
    if(g[v] && !g[v]->label().empty())
      out << "[label=\"" << g[v]->label() << "\"]";
    else
      out << "[]";
  },
  [] (auto&&...) {});

  stream << s.str() << "\n";
}


struct OSSIA_EXPORT graph_util
{
  static void copy_from_local(const data_type& out, inlet& in)
  {
    if (out.which() == in.data.which() && out && in.data)
    {
      eggs::variants::apply(copy_data{}, out, in.data);
    }
  }

  static void copy(const delay_line_type& out, std::size_t pos, inlet& in)
  {
    if (out.which() == in.data.which() && out && in.data)
    {
      eggs::variants::apply(copy_data_pos{pos}, out, in.data);
    }
  }

  static void copy(const outlet& out, inlet& in)
  {
    copy_from_local(out.data, in);
  }

  static void copy_to_local(
      const data_type& out, const destination& d, execution_state& in)
  {
    in.insert(destination_t{&d.address()}, out);
  }

  static void copy_to_global(
      const data_type& out, const destination& d, execution_state& in)
  {
    // TODO
  }

  static void pull_from_parameter(inlet& in, execution_state& e)
  {
    apply_to_destination(in.address, e.allDevices, [&] (ossia::net::parameter_base* addr) {
      if (in.scope & port::scope_t::local)
      {
        e.find_and_copy(*addr, in);
      }
      else
      {
        e.copy_from_global(*addr, in);
      }
    });
  }

  static void init_node(graph_node& n, execution_state& e)
  {
    // Clear the outputs of the node
    for (const outlet_ptr& out : n.outputs())
    {
      if (out->data)
        eggs::variants::apply(clear_data{}, out->data);
    }

    // Copy from environment and previous ports to inputs
    for (const inlet_ptr& in : n.inputs())
    {
      if (!in->sources.empty())
      {
        for (auto edge : in->sources)
        {
          ossia::apply(init_node_visitor<graph_util>{*in, *edge, e}, edge->con);
        }
      }
      else
      {
        pull_from_parameter(*in, e);
      }
    }
  }

  static void teardown_node(const graph_node& n, execution_state& e)
  {
    for (const inlet_ptr& in : n.inputs())
    {
      if (in->data)
        eggs::variants::apply(clear_data{}, in->data);
    }

    // Copy from output ports to environment
    for (const outlet_ptr& out : n.outputs())
    {
      out->write(e);
      for (auto tgt : out->targets)
      {
        ossia::apply(env_writer{*out, *tgt, e}, tgt->con);
      }
    }
  }

  static void disable_strict_nodes(const node_flat_set& enabled_nodes, node_flat_set& ret)
  {
    for (graph_node* node : enabled_nodes)
    {
      for (const auto& in : node->inputs())
      {
        for (const auto& edge : in->sources)
        {
          assert(edge->out_node);

          if (immediate_strict_connection* sc = edge->con.target<immediate_strict_connection>())
          {
            if ((sc->required_sides
                 & immediate_strict_connection::required_sides_t::outbound)
                && node->enabled() && !edge->out_node->enabled())
            {
              ret.insert(node);
            }
          }
          else if (delayed_strict_connection* delay = edge->con.target<delayed_strict_connection>())
          {
            const auto n = ossia::apply(data_size{}, delay->buffer);
            if (n == 0 || delay->pos >= n)
              ret.insert(node);
          }
        }
      }

      for (const auto& out : node->outputs())
      {
        for (const auto& edge : out->targets)
        {
          assert(edge->in_node);

          if (auto sc = edge->con.target<immediate_strict_connection>())
          {
            if ((sc->required_sides
                 & immediate_strict_connection::required_sides_t::inbound)
                && node->enabled() && !edge->in_node->enabled())
            {
              ret.insert(node);
            }
          }
        }
      }
    }
  }

  static void disable_strict_nodes_rec(node_flat_set& cur_enabled_node, node_flat_set& disabled_cache)
  {
    do
    {
      disabled_cache.clear();
      disable_strict_nodes(cur_enabled_node, disabled_cache);
      for (ossia::graph_node* n : disabled_cache)
      {
        if(!n->requested_tokens.empty())
          n->set_prev_date(n->requested_tokens.back().date);
        n->disable();

        cur_enabled_node.erase(n);
      }

    } while (!disabled_cache.empty());
  }

  static void exec_node(graph_node& first_node,
                 execution_state& e)
  {
    init_node(first_node, e);
    if(first_node.start_discontinuous()) {
      first_node.requested_tokens.front().start_discontinuous = true;
      first_node.set_start_discontinuous(false);
    }
    if(first_node.end_discontinuous()) {
      first_node.requested_tokens.front().end_discontinuous = true;
      first_node.set_end_discontinuous(false);
    }

    for(const auto& request : first_node.requested_tokens) {
      first_node.run(request, e);
      first_node.set_prev_date(request.date);
    }

    first_node.set_executed(true);
    teardown_node(first_node, e);
  }

  // These methods are only accessed by ossia::graph
  static bool can_execute(graph_node& node, const execution_state&)
  {
    return ossia::all_of(node.inputs(), [](const auto& inlet) {
      // A port can execute if : - it has source ports and all its source ports
      // have executed

      // if there was a strict connection, this node would have been disabled
      // so we can just do the following check.
      bool previous_nodes_executed
          = ossia::all_of(inlet->sources, [&](graph_edge* edge) {
          return edge->out_node->executed()
          || (!edge->out_node->enabled() /* && bool(inlet->address) */
              /* TODO check that it's in scope */);
    });

      // it does not have source ports ; we have to check for variables :
      // find if address available in local / global scope
      return !inlet->sources.empty() ? previous_nodes_executed : true // TODO
                                       ;
    });
  }

  static void finish_nodes(const node_map& nodes)
  {
    for (auto& node : nodes)
    {
      ossia::graph_node& n = *node.first;
      n.set_executed(false);
      n.disable();
      for (const outlet_ptr& out : node.first->outputs())
      {
        if (out->data)
          eggs::variants::apply(clear_data{}, out->data);
      }
    }
  }

  static bool find_path(int source, int sink, graph_t& graph)
  {
    bool ok = false;
    struct bfs_time_visitor : boost::default_bfs_visitor
    {
      int node_to_find{};
      bool& ok;
      bool discover_vertex(int u, const graph_t&) const
      {
        if(u == node_to_find)
          ok = true;
        return ok;
      }
    } to_find{{}, sink, ok};

    ossia::bfs::breadth_first_search_simple(graph, source, to_find);
    return ok;
  }
};

enum class static_graph_policy { bfs, transitive_closure };
template<static_graph_policy Policy>
struct OSSIA_EXPORT graph_static_base
    : private graph_util
{
  private:
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

public:
  void mark_dirty()
  {
    m_dirty = true;
  }
  ~graph_static_base()
  {
    clear();
  }

  void add_node(node_ptr n)
  {
    if(m_nodes.find(n) == m_nodes.end())
    {
      auto vtx = boost::add_vertex(n, m_graph);
      m_nodes.insert({std::move(n), vtx});
      m_dirty = true;
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
    m_dirty = true;
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
        m_dirty = true;
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
        m_dirty = true;
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
    m_dirty = true;
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


  template<typename DevicesT>
  static auto find_address_connection(ossia::graph_node& source, ossia::graph_node& sink, const DevicesT& devices)
  {
    bool ok = false;
    for(const ossia::outlet_ptr& outlet : source.outputs())
    {
      for(const ossia::inlet_ptr& inlet : sink.inputs())
      {
        apply_to_destination(outlet->address, devices, [&] (ossia::net::parameter_base* p1) {
          apply_to_destination(inlet->address, devices, [&] (ossia::net::parameter_base* p2) {
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
// #define OSSIA_GRAPH_DEBUG
  template<typename DevicesT>
  void update_graph_bfs(const DevicesT& devices)
  {
    m_sub_graph = m_graph;

    get_sorted_nodes(m_graph);
    // m_active_nodes is in topo order

    for(std::size_t i = 0; i < m_all_nodes.size(); i++)
    {
      ossia::graph_node* n1 = m_all_nodes[i];
      for(std::size_t j = i + 1; j < m_all_nodes.size(); j++)
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
      exit(1);
    }
  }

  void state(execution_state& e)
  {
    try
    {
      if(m_dirty)
      {
        switch(Policy)
        {
          case static_graph_policy::bfs:
            update_graph_bfs(e.allDevices);
            break;
          case static_graph_policy::transitive_closure:
            update_graph_tclosure(e.allDevices);
            break;
        }

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
      tick_static(*this, e, m_all_nodes);

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
  std::vector<graph_node*> m_all_nodes;
  std::vector<graph_vertex_t> m_topo_order_cache;
  bool m_dirty{};

  graph_t m_sub_graph;
  transitive_closure_t m_transitive_closure;
  friend struct inlet;
  friend struct outlet;
  friend class ::DataflowTest;
};

}
