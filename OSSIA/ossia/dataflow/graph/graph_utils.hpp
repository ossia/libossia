#pragma once
#include <ossia/dataflow/bench_map.hpp>
#include <ossia/dataflow/data_copy.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/breadth_first_search.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_ordering.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>

class DataflowTest;

namespace ossia
{
using graph_t = boost::adjacency_list<
    boost::vecS, boost::vecS, boost::directedS, node_ptr,
    std::shared_ptr<graph_edge>>;

using graph_vertex_t = graph_t::vertex_descriptor;
using graph_edge_t = graph_t::edge_descriptor;

using node_map = ossia::shared_ptr_map<ossia::graph_node, graph_vertex_t>;
using edge_map = ossia::shared_ptr_map<ossia::graph_edge, graph_edge_t>;

using node_flat_set = ossia::flat_set<graph_node*>;
enum class node_ordering
{
  topological,
  temporal,
  hierarchical
};

template <typename T>
auto apply_con(const T& visitor, ossia::connection& con)
{
  auto tgt = con.target();
  switch (con.which())
  {
    case 0:
      return visitor(*reinterpret_cast<immediate_glutton_connection*>(tgt));
      break;
    case 1:
      return visitor(*reinterpret_cast<immediate_strict_connection*>(tgt));
      break;
    case 2:
      return visitor(*reinterpret_cast<delayed_glutton_connection*>(tgt));
      break;
    case 3:
      return visitor(*reinterpret_cast<delayed_strict_connection*>(tgt));
      break;
    case 4:
      return visitor(*reinterpret_cast<dependency_connection*>(tgt));
      break;
    default:
      return visitor();
      break;
  }
}
template <typename T>
auto apply_con(const T& visitor, const ossia::connection& con)
{
  auto tgt = con.target();
  switch (con.which())
  {
    case 0:
      return visitor(
          *reinterpret_cast<const immediate_glutton_connection*>(tgt));
      break;
    case 1:
      return visitor(
          *reinterpret_cast<const immediate_strict_connection*>(tgt));
      break;
    case 2:
      return visitor(
          *reinterpret_cast<const delayed_glutton_connection*>(tgt));
      break;
    case 3:
      return visitor(*reinterpret_cast<const delayed_strict_connection*>(tgt));
      break;
    case 4:
      return visitor(*reinterpret_cast<const dependency_connection*>(tgt));
      break;
    default:
      return visitor();
      break;
  }
}

template <typename Graph_T, typename IO>
void print_graph(Graph_T& g, IO& stream)
{
  std::stringstream s;
  boost::write_graphviz(
      s, g,
      [&](auto& out, auto v) {
        if (g[v] && !g[v]->label().empty())
          out << "[label=\"" << g[v]->label() << "\"]";
        else
          out << "[]";
      },
      [](auto&&...) {});

  stream << s.str() << "\n";
}

struct OSSIA_EXPORT graph_util
{
  static void pull_from_parameter(inlet& in, execution_state& e)
  {
    apply_to_destination(
        in.address, e.exec_devices(),
        [&](ossia::net::parameter_base* addr, bool) {
          if (in.scope & port::scope_t::local)
          {
            e.find_and_copy(*addr, in);
          }
          else if (in.scope & port::scope_t::global)
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
      bool must_copy = in->sources.empty();

      for (const graph_edge* edge : in->sources)
      {
        must_copy
            |= ossia::apply_con(init_must_copy_visitor{*edge}, edge->con);
      }

      if (must_copy)
        pull_from_parameter(*in, e);

      for (auto edge : in->sources)
      {
        ossia::apply_con(init_node_visitor{*in, *edge, e}, edge->con);
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
      bool must_copy = out->targets.empty();

      // If some following glutton nodes aren't enabled, then we copy to the
      // env.
      for (const auto& tgt : out->targets)
      {
        must_copy |= ossia::apply_con(env_writer{*out, *tgt}, tgt->con);
      }

      // if there are two outgoing glutton connections, one active, the other
      // inactive then we want to copy through cable for the first one, and
      // through env for the second one
      if (must_copy)
        out->write(e);
    }
  }
  /*
    static void disable_strict_nodes_bfs(const graph_t& graph)
    {
      // while(Find a non-marked disabled node)
      // Do a BFS from it
      std::map<graph_vertex_t, boost::two_bit_color_type> mark;
      struct disable_visitor : public boost::default_bfs_visitor
      {
          void discover_vertex(graph_vertex_t vtx, graph_t& g) const
          {
            auto ptr = g[vtx].get();

          }
      };
    }
  */
  static bool disable_strict_nodes(const graph_node* node)
  {
    if (node->muted())
      return true;
    for (const auto& in : node->inputs())
    {
      for (const auto& edge : in->sources)
      {
        assert(edge->out_node);

        if (immediate_strict_connection* sc
            = edge->con.target<immediate_strict_connection>())
        {
          if ((sc->required_sides
               & immediate_strict_connection::required_sides_t::outbound)
              && node->enabled() && !edge->out_node->enabled())
          {
            return true;
          }
        }
        else if (
            delayed_strict_connection* delay
            = edge->con.target<delayed_strict_connection>())
        {
          const auto n = ossia::apply(data_size{}, delay->buffer);
          if (n == 0 || delay->pos >= n)
          {
            return true;
          }
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
            return true;
          }
        }
      }
    }

    return false;
  }

  static void
  disable_strict_nodes(const node_flat_set& enabled_nodes, node_flat_set& ret)
  {
    for (graph_node* node : enabled_nodes)
    {
      if (disable_strict_nodes(node))
        ret.insert(node);
    }
  }

  static void disable_strict_nodes_rec(
      node_flat_set& cur_enabled_node, node_flat_set& disabled_cache)
  {
    do
    {
      disabled_cache.clear();
      disable_strict_nodes(cur_enabled_node, disabled_cache);
      for (ossia::graph_node* n : disabled_cache)
      {
        n->disable();

        cur_enabled_node.erase(n);
      }

    } while (!disabled_cache.empty());
  }

  static void log_inputs(const graph_node&, spdlog::logger& logger);
  static void log_outputs(const graph_node&, spdlog::logger& logger);

  static void run_scaled(graph_node& first_node, execution_state& e);

  static void exec_node(graph_node& first_node, execution_state& e)
  {
    init_node(first_node, e);
    if (first_node.start_discontinuous())
    {
      first_node.requested_tokens.front().start_discontinuous = true;
      first_node.set_start_discontinuous(false);
    }
    if (first_node.end_discontinuous())
    {
      first_node.requested_tokens.front().end_discontinuous = true;
      first_node.set_end_discontinuous(false);
    }

    for (const auto& request : first_node.requested_tokens)
    {
      first_node.run(request, {e});
    }
    /*
        auto all_normal = ossia::all_of(first_node.requested_tokens,
                                       [] (const ossia::token_request& tk) {
       return tk.speed == 1.;}); if(all_normal)
        {
          for(const auto& request : first_node.requested_tokens)
          {
            first_node.run(request, e);
            first_node.set_prev_date(request.date);
          }
        }
        else
        {
          run_scaled(first_node, e);
        }
    */
    first_node.set_executed(true);
    teardown_node(first_node, e);
  }

  static void
  exec_node(graph_node& first_node, execution_state& e, spdlog::logger& logger)
  {
    init_node(first_node, e);
    if (first_node.start_discontinuous())
    {
      first_node.requested_tokens.front().start_discontinuous = true;
      first_node.set_start_discontinuous(false);
    }
    if (first_node.end_discontinuous())
    {
      first_node.requested_tokens.front().end_discontinuous = true;
      first_node.set_end_discontinuous(false);
    }

    log_inputs(first_node, logger);
    for (const auto& request : first_node.requested_tokens)
    {
      first_node.run(request, {e});
    }
    log_outputs(first_node, logger);

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
      bool previous_nodes_executed = ossia::all_of(
          inlet->sources, [&](graph_edge* edge) {
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

  template <typename DevicesT>
  static auto find_address_connection(
      ossia::graph_node& source, ossia::graph_node& sink,
      const DevicesT& devices)
  {
    bool ok = false;
    for (const ossia::outlet_ptr& outlet : source.outputs())
    {
      for (const ossia::inlet_ptr& inlet : sink.inputs())
      {
        apply_to_destination(
            outlet->address, devices,
            [&](ossia::net::parameter_base* p1, bool) {
              apply_to_destination(
                  inlet->address, devices,
                  [&](ossia::net::parameter_base* p2, bool) {
                    if (p1 == p2)
                      ok = true;
                  });
            });

        if (ok)
          break;
      }
      if (ok)
        break;
    }
    return ok;
  }
};

struct OSSIA_EXPORT graph_base : graph_interface
{
  const std::vector<ossia::graph_node*>& get_nodes() const
      noexcept final override
  {
    return m_node_list;
  }

  void recompute_maps()
  {
    m_nodes.clear();
    m_edges.clear();
    auto vertices = boost::vertices(m_graph);
    for (auto it = vertices.first; it != vertices.second; ++it)
    {
      graph_vertex_t k = *it;
      node_ptr n = m_graph[k];
      assert(n);

      m_nodes.insert({n, k});
    }

    auto edges = boost::edges(m_graph);
    for (auto it = edges.first; it != edges.second; ++it)
    {
      graph_edge_t k = *it;
      edge_ptr n = m_graph[k];
      assert(n);

      m_edges.insert({n, k});
    }
  }
  auto add_node_impl(node_ptr n)
  {
    // auto& bench = *ossia::bench_ptr();
    // bench[n.get()];

    auto vtx = boost::add_vertex(n, m_graph);
    // m_nodes.insert({std::move(n), vtx});
    m_node_list.push_back(n.get());
    m_dirty = true;
    recompute_maps();
    return vtx;
  }

  void add_node(node_ptr n) final override
  {
    if (m_nodes.find(n) == m_nodes.end())
    {
      add_node_impl(std::move(n));
    }
  }

  void remove_node(const node_ptr& n) final override
  {
    for (auto& port : n->inputs())
    {
      auto s = port->sources;
      for (auto edge : s)
        disconnect(edge);
    }
    for (auto& port : n->outputs())
    {
      auto s = port->targets;
      for (auto edge : s)
        disconnect(edge);
    }

    auto it = m_nodes.find(n);
    if (it != m_nodes.end())
    {
      auto vtx = boost::vertices(m_graph);
      if (std::find(vtx.first, vtx.second, it->second) != vtx.second)
      {
        boost::clear_vertex(it->second, m_graph);
        boost::remove_vertex(it->second, m_graph);

        recompute_maps();
      }
      // no need to erase it since it won't be here after recompute_maps
    }
    ossia::remove_one(m_node_list, n.get());
    m_dirty = true;
  }

  void connect(std::shared_ptr<graph_edge> edge) final override
  {
    if (edge)
    {
      edge->init();

      graph_vertex_t in_vtx, out_vtx;
      auto it1 = m_nodes.find(edge->in_node);
      if (it1 == m_nodes.end())
        in_vtx = add_node_impl(edge->in_node);
      else
        in_vtx = it1->second;

      auto it2 = m_nodes.find(edge->out_node);
      if (it2 == m_nodes.end())
        out_vtx = add_node_impl(edge->out_node);
      else
        out_vtx = it2->second;

      // TODO check that two edges can be added
      boost::add_edge(in_vtx, out_vtx, edge, m_graph);
      recompute_maps();
      m_dirty = true;
    }
  }

  void disconnect(const std::shared_ptr<graph_edge>& edge) final override
  {
    disconnect(edge.get());
  }

  void disconnect(graph_edge* edge) final override
  {
    if (edge)
    {
      edge->clear();
      auto it = m_edges.find(edge);
      if (it != m_edges.end())
      {
        auto edg = boost::edges(m_graph);
        if (std::find(edg.first, edg.second, it->second) != edg.second)
        {
          boost::remove_edge(it->second, m_graph);
          recompute_maps();
        }
        m_dirty = true;
        // no need to erase it since it won't be here after recompute_maps
      }
    }
  }

  void clear() final override
  {
    // TODO clear all the connections, ports, etc, to ensure that there is no
    // shared_ptr loop
    for (auto& edge : m_edges)
    {
      edge.first->clear();
    }
    for (auto& node : m_nodes)
    {
      node.first->clear();
    }
    m_dirty = true;
    m_nodes.clear();
    m_node_list.clear();
    m_edges.clear();
    m_graph.clear();
  }

  void mark_dirty() final override
  {
    m_dirty = true;
  }

  ~graph_base() override
  {
    clear();
  }

  node_map m_nodes;
  edge_map m_edges;
  std::vector<ossia::graph_node*> m_node_list;

  graph_t m_graph;

  bool m_dirty{};
};
}
