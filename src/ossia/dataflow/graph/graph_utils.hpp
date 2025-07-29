#pragma once
#include <ossia/dataflow/bench_map.hpp>
#include <ossia/dataflow/data_copy.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/for_each_port.hpp>
#include <ossia/dataflow/graph/breadth_first_search.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph/graph_ordering.hpp>
#include <ossia/dataflow/graph/small_graph.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/disable_fpe.hpp>
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/predef.h>
// broken due to dynamic_property_map requiring rtti...
// #include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>

#include <ankerl/unordered_dense.h>

#if BOOST_LIB_STD_GNU >= BOOST_VERSION_NUMBER(13, 0, 0) \
    && BOOST_VERSION_NUMBER <= BOOST_VERSION_NUMBER(1, 83, 0)
#define OSSIA_SMALL_VECTOR_ALLOCATOR_REBIND_FAILS 1
#endif

class DataflowTest;
namespace ossia
{
using graph_t = boost::adjacency_list<
    boost::smallvecS, boost::smallvecS, boost::directedS, node_ptr,
    std::shared_ptr<graph_edge>>;
}
namespace boost::detail
{

template <>
class stored_edge_property<unsigned long, std::shared_ptr<ossia::graph_edge>>
    : public stored_edge<unsigned long>
{
  using Vertex = unsigned long;
  using Property = std::shared_ptr<ossia::graph_edge>;
  typedef stored_edge_property self;
  typedef stored_edge<Vertex> Base;

public:
  typedef Property property_type;
  inline stored_edge_property() { }
  inline stored_edge_property(Vertex target, Property p = {})
      : stored_edge<Vertex>(target)
      , m_property(std::move(p))
  {
  }
  stored_edge_property(self&& x) noexcept
      : Base(static_cast<Base&&>(x))
      , m_property(std::move(x.m_property))
  {
  }
  stored_edge_property(self const& x)
      : Base(static_cast<Base const&>(x))
      , m_property(std::move(const_cast<self&>(x).m_property))
  {
  }
  self& operator=(self&& x)
  {
    // NOTE: avoid 'Base::operator=(x);' broken on SGI MIPSpro (bug
    // 55771 of Mozilla).
    static_cast<Base&>(*this) = static_cast<Base&&>(x);
    m_property = std::move(x.m_property);
    return *this;
  }
  self& operator=(self const& x)
  {
    // NOTE: avoid 'Base::operator=(x);' broken on SGI MIPSpro (bug
    // 55771 of Mozilla).
    static_cast<Base&>(*this) = static_cast<Base const&>(x);
    m_property = std::move(const_cast<self&>(x).m_property);
    return *this;
  }
  inline Property& get_property() noexcept { return m_property; }
  inline const Property& get_property() const noexcept { return m_property; }

protected:
  Property m_property;
};

}

namespace boost
{
namespace detail
{
template <typename G>
using DFSVertexInfo = std::pair<
    typename graph_traits<G>::vertex_descriptor,
    std::pair<
        boost::optional<typename graph_traits<G>::edge_descriptor>,
        std::pair<
            typename graph_traits<G>::out_edge_iterator,
            typename graph_traits<G>::out_edge_iterator>>>;

template <class IncidenceGraph, class DFSVisitor, class ColorMap>
void custom_depth_first_visit_impl(
    const IncidenceGraph& g, typename graph_traits<IncidenceGraph>::vertex_descriptor u,
    DFSVisitor& vis, ColorMap color, std::vector<DFSVertexInfo<IncidenceGraph>>& stack)
{
  constexpr detail::nontruth2 func;
  BOOST_CONCEPT_ASSERT((IncidenceGraphConcept<IncidenceGraph>));
  BOOST_CONCEPT_ASSERT((DFSVisitorConcept<DFSVisitor, IncidenceGraph>));
  typedef typename graph_traits<IncidenceGraph>::vertex_descriptor Vertex;
  typedef typename graph_traits<IncidenceGraph>::edge_descriptor Edge;
  BOOST_CONCEPT_ASSERT((ReadWritePropertyMapConcept<ColorMap, Vertex>));
  typedef typename property_traits<ColorMap>::value_type ColorValue;
  BOOST_CONCEPT_ASSERT((ColorValueConcept<ColorValue>));
  typedef color_traits<ColorValue> Color;
  typedef typename graph_traits<IncidenceGraph>::out_edge_iterator Iter;
  typedef std::pair<Vertex, std::pair<boost::optional<Edge>, std::pair<Iter, Iter>>>
      VertexInfo;

  boost::optional<Edge> src_e;
  Iter ei, ei_end;

  // Possible optimization for vector
  stack.clear();
  stack.reserve(num_vertices(g));

  put(color, u, Color::gray());
  vis.discover_vertex(u, g);
  boost::tie(ei, ei_end) = out_edges(u, g);
  if(func(u, g))
  {
    // If this vertex terminates the search, we push empty range
    stack.push_back(std::make_pair(
        u, std::make_pair(boost::optional<Edge>(), std::make_pair(ei_end, ei_end))));
  }
  else
  {
    stack.push_back(std::make_pair(
        u, std::make_pair(boost::optional<Edge>(), std::make_pair(ei, ei_end))));
  }
  while(!stack.empty())
  {
    VertexInfo& back = stack.back();
    u = back.first;
    src_e = back.second.first;
    boost::tie(ei, ei_end) = back.second.second;
    stack.pop_back();
    // finish_edge has to be called here, not after the
    // loop. Think of the pop as the return from a recursive call.
    if(src_e)
    {
      call_finish_edge(vis, src_e.get(), g);
    }
    while(ei != ei_end)
    {
      Vertex v = target(*ei, g);
      vis.examine_edge(*ei, g);
      ColorValue v_color = get(color, v);
      if(v_color == Color::white())
      {
        vis.tree_edge(*ei, g);
        src_e = *ei;
        stack.push_back(
            std::make_pair(u, std::make_pair(src_e, std::make_pair(++ei, ei_end))));
        u = v;
        put(color, u, Color::gray());
        vis.discover_vertex(u, g);
        boost::tie(ei, ei_end) = out_edges(u, g);
        if(func(u, g))
        {
          ei = ei_end;
        }
      }
      else
      {
        if(v_color == Color::gray())
        {
          vis.back_edge(*ei, g);
        }
        else
        {
          vis.forward_or_cross_edge(*ei, g);
        }
        call_finish_edge(vis, *ei, g);
        ++ei;
      }
    }
    put(color, u, Color::black());
    vis.finish_vertex(u, g);
  }
}
}

template <class VertexListGraph, class DFSVisitor, class ColorMap>
void custom_depth_first_search(
    const VertexListGraph& g, DFSVisitor vis, ColorMap color,
    typename graph_traits<VertexListGraph>::vertex_descriptor start_vertex,
    std::vector<boost::detail::DFSVertexInfo<VertexListGraph>>& stack)
{
  typedef typename graph_traits<VertexListGraph>::vertex_descriptor Vertex;
  BOOST_CONCEPT_ASSERT((DFSVisitorConcept<DFSVisitor, VertexListGraph>));
  typedef typename property_traits<ColorMap>::value_type ColorValue;
  typedef color_traits<ColorValue> Color;

  typename graph_traits<VertexListGraph>::vertex_iterator ui, ui_end;
  for(boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui)
  {
    Vertex u = implicit_cast<Vertex>(*ui);
    put(color, u, Color::white());
    vis.initialize_vertex(u, g);
  }

  if(start_vertex != detail::get_default_starting_vertex(g))
  {
    vis.start_vertex(start_vertex, g);
    detail::custom_depth_first_visit_impl(g, start_vertex, vis, color, stack);
  }

  for(boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui)
  {
    Vertex u = implicit_cast<Vertex>(*ui);
    ColorValue u_color = get(color, u);
    if(u_color == Color::white())
    {
      vis.start_vertex(u, g);
      detail::custom_depth_first_visit_impl(g, u, vis, color, stack);
    }
  }
}

}
namespace ossia
{
inline void remove_vertex(typename graph_t::vertex_descriptor v, graph_t& g)
{
  typedef typename graph_t::directed_category Cat;
  g.removing_vertex(v, boost::graph_detail::iterator_stability(g.m_vertices));
  boost::detail::remove_vertex_dispatch(g, v, Cat());
}

template <typename VertexListGraph, typename OutputIterator>
void custom_topological_sort(
    VertexListGraph& g, OutputIterator result,
    std::vector<boost::default_color_type>& color_map,
    std::vector<boost::detail::DFSVertexInfo<VertexListGraph>>& stack)
{
  color_map.clear();
  color_map.resize(boost::num_vertices(g));

  auto map = boost::make_iterator_property_map(
      color_map.begin(), boost::get(boost::vertex_index, g), color_map[0]);

  boost::custom_depth_first_search(
      g, boost::topo_sort_visitor<OutputIterator>(result), map,
      boost::detail::get_default_starting_vertex(g), stack);

  // depth_first_search(g, params.visitor(TopoVisitor(result)));
}

using graph_vertex_t = graph_t::vertex_descriptor;
using graph_edge_t = graph_t::edge_descriptor;

#if !defined(OSSIA_NO_FAST_CONTAINERS)
template <typename T, typename V>
using dense_shared_ptr_map = ankerl::unordered_dense::map<
    std::shared_ptr<T>, V, egur_hash, pointer_equal,
#if defined(OSSIA_SMALL_VECTOR_ALLOCATOR_REBIND_FAILS)
    std::vector<std::pair<std::shared_ptr<T>, V>>
#else
    ossia::small_vector<std::pair<std::shared_ptr<T>, V>, 1024>
#endif
    >;
#else
template <typename T, typename V>
using dense_shared_ptr_map
    = ossia::hash_map<std::shared_ptr<T>, V, egur_hash, pointer_equal>;
#endif
using node_map = ossia::dense_shared_ptr_map<ossia::graph_node, graph_vertex_t>;
using edge_map = ossia::dense_shared_ptr_map<ossia::graph_edge, graph_edge_t>;

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
  switch(con.which().index())
  {
    case ossia::connection::index_of<immediate_glutton_connection>().index():
      return visitor(*reinterpret_cast<immediate_glutton_connection*>(tgt));
      break;
    case ossia::connection::index_of<immediate_strict_connection>().index():
      return visitor(*reinterpret_cast<immediate_strict_connection*>(tgt));
      break;
    case ossia::connection::index_of<delayed_glutton_connection>().index():
      return visitor(*reinterpret_cast<delayed_glutton_connection*>(tgt));
      break;
    case ossia::connection::index_of<delayed_strict_connection>().index():
      return visitor(*reinterpret_cast<delayed_strict_connection*>(tgt));
      break;
    case ossia::connection::index_of<dependency_connection>().index():
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
  switch(con.which().index())
  {
    case ossia::connection::index_of<immediate_glutton_connection>().index():
      return visitor(*reinterpret_cast<const immediate_glutton_connection*>(tgt));
      break;
    case ossia::connection::index_of<immediate_strict_connection>().index():
      return visitor(*reinterpret_cast<const immediate_strict_connection*>(tgt));
      break;
    case ossia::connection::index_of<delayed_glutton_connection>().index():
      return visitor(*reinterpret_cast<const delayed_glutton_connection*>(tgt));
      break;
    case ossia::connection::index_of<delayed_strict_connection>().index():
      return visitor(*reinterpret_cast<const delayed_strict_connection*>(tgt));
      break;
    case ossia::connection::index_of<dependency_connection>().index():
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
#if 0
  std::stringstream s;
  boost::write_graphviz(
      s, g,
      [&](auto& out, auto v) {
    if(g[v] && !g[v]->label().empty())
      out << "[label=\"" << g[v]->label() << "\"]";
    else
      out << "[]";
      },
      [](auto&&...) {});

  stream << s.str() << "\n";
#endif
}

struct OSSIA_EXPORT graph_util
{
  static void pull_from_parameter(inlet& in, execution_state& e)
  {
    apply_to_destination(
        in.address, e.exec_devices(),
        [&](ossia::net::parameter_base* addr, bool) {
      if(in.scope & port::scope_t::local)
      {
        e.find_and_copy(*addr, in);
      }
      else if(in.scope & port::scope_t::global)
      {
        e.copy_from_global(*addr, in);
      }
        },
        [&](ossia::net::node_base* node, bool) { e.copy_from_global_node(*node, in); });
  }

  /// Init : what happens just before a node is going to be executed
  static void init_outlet(outlet& out, execution_state&)
  {
    out.visit(clear_data{});

    out.pre_process();
  }

  static void init_inlet(inlet& in, execution_state& e)
  {
    bool must_copy = in.sources.empty();

    for(const graph_edge* edge : in.sources)
    {
      must_copy |= ossia::apply_con(init_must_copy_visitor{*edge}, edge->con);
    }

    if(must_copy)
      pull_from_parameter(in, e);

    for(auto edge : in.sources)
    {
      ossia::apply_con(init_node_visitor{in, *edge, e}, edge->con);
    }

    in.pre_process();
  }

  static void init_node(graph_node& n, execution_state& e)
  {
    if(n.not_fp_safe())
    {
      [[unlikely]];
      disable_fpe();
    }

    // Clear the outputs of the node
    for_each_outlet(n, [&](auto& port) { init_outlet(port, e); });

    // Copy from environment and previous ports to inputs
    for_each_inlet(n, [&](auto& port) { init_inlet(port, e); });
  }

  /// Teardown : what happens just after a node has executed
  static void teardown_outlet(outlet& out, execution_state& e)
  {
    out.post_process();
    bool must_copy = out.targets.empty();

    // If some following glutton nodes aren't enabled, then we copy to the
    // env.
    for(const auto& tgt : out.targets)
    {
      must_copy |= ossia::apply_con(env_writer{out, *tgt}, tgt->con);
    }

    // if there are two outgoing glutton connections, one active, the other
    // inactive then we want to copy through cable for the first one, and
    // through env for the second one
    if(must_copy)
      out.write(e);
  }

  static void teardown_inlet(inlet& in, execution_state&)
  {
    in.post_process();
    in.visit(clear_data{});
  }

  static void teardown_node(const graph_node& n, execution_state& e)
  {
    if(n.not_fp_safe())
    {
      [[unlikely]];
      disable_fpe();
    }

    // Copy from output ports to environment
    // Clear the outputs of the node
    for_each_outlet(n, [&](auto& port) { teardown_outlet(port, e); });

    // Copy from environment and previous ports to inputs
    for_each_inlet(n, [&](auto& port) { teardown_inlet(port, e); });
  }

  /*
    static void disable_strict_nodes_bfs(const graph_t& graph)
    {
      // while(Find a non-marked disabled node)
      // Do a BFS from it
      ossia::flat_map<graph_vertex_t, boost::two_bit_color_type> mark;
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
    if(node->muted())
      return true;

    auto test_disable_inlet = [&](const ossia::inlet& inlet) {
      for(const auto& edge : inlet.sources)
      {
        assert(edge);
        assert(edge->out_node);

        if(immediate_strict_connection* sc
           = edge->con.target<immediate_strict_connection>())
        {
          if((sc->required_sides
              & immediate_strict_connection::required_sides_t::outbound)
             && node->enabled() && !edge->out_node->enabled())
          {
            return true;
          }
        }
        else if(
            delayed_strict_connection* delay
            = edge->con.target<delayed_strict_connection>())
        {
          const auto n = ossia::apply(data_size{}, delay->buffer);
          if(n == 0 || delay->pos >= n)
          {
            return true;
          }
        }
      }
      return false;
    };

    auto test_disable_outlet = [&](const ossia::outlet& outlet) {
      for(const auto& edge : outlet.targets)
      {
        assert(edge->in_node);

        if(auto sc = edge->con.target<immediate_strict_connection>())
        {
          if((sc->required_sides
              & immediate_strict_connection::required_sides_t::inbound)
             && node->enabled() && !edge->in_node->enabled())
          {
            return true;
          }
        }
      }
      return false;
    };

    if(any_of_inlet(*node, test_disable_inlet))
      return true;
    if(any_of_outlet(*node, test_disable_outlet))
      return true;

    return false;
  }

  static void
  disable_strict_nodes(const node_flat_set& enabled_nodes, node_flat_set& ret)
  {
    for(graph_node* node : enabled_nodes)
    {
      if(disable_strict_nodes(node))
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
      for(ossia::graph_node* n : disabled_cache)
      {
        n->disable();

        cur_enabled_node.erase(n);
      }

    } while(!disabled_cache.empty());
  }

  static void log_inputs(const graph_node&, ossia::logger_type& logger);
  static void log_outputs(const graph_node&, ossia::logger_type& logger);

  static void check_inputs(const graph_node&, execution_state& e);
  static void
  check_outputs(const graph_node&, execution_state& e, const ossia::token_request& req);
  static void run_scaled(graph_node& first_node, execution_state& e);

#if !defined(NDEBUG)
#define OSSIA_DEBUG_MISBEHAVING_NODES 1
#endif
  static void exec_node(graph_node& first_node, execution_state& e)
  {
    init_node(first_node, e);

#if defined(OSSIA_DEBUG_MISBEHAVING_NODES)
    check_inputs(first_node, e);
#endif

    if(!first_node.requested_tokens.empty())
    {
      if(first_node.start_discontinuous())
      {
        first_node.requested_tokens.front().start_discontinuous = true;
        first_node.set_start_discontinuous(false);
      }
      if(first_node.end_discontinuous())
      {
        first_node.requested_tokens.front().end_discontinuous = true;
        first_node.set_end_discontinuous(false);
      }
    }

    for(const auto& request : first_node.requested_tokens)
    {
      first_node.run(request, {&e});
      first_node.process_time(request, e);
#if defined(OSSIA_DEBUG_MISBEHAVING_NODES)
      check_outputs(first_node, e, request);
#endif
    }

    first_node.set_executed(true);
    teardown_node(first_node, e);
  }

  static void
  exec_node(graph_node& first_node, execution_state& e, ossia::logger_type& logger)
  {
    init_node(first_node, e);

#if defined(OSSIA_DEBUG_MISBEHAVING_NODES)
    check_inputs(first_node, e);
#endif

    if(!first_node.requested_tokens.empty())
    {
      if(first_node.start_discontinuous())
      {
        first_node.requested_tokens.front().start_discontinuous = true;
        first_node.set_start_discontinuous(false);
      }
      if(first_node.end_discontinuous())
      {
        first_node.requested_tokens.front().end_discontinuous = true;
        first_node.set_end_discontinuous(false);
      }
    }

    log_inputs(first_node, logger);
    for(const auto& request : first_node.requested_tokens)
    {
      first_node.run(request, {&e});
      first_node.process_time(request, e);
#if defined(OSSIA_DEBUG_MISBEHAVING_NODES)
      check_outputs(first_node, e, request);
#endif
    }
    log_outputs(first_node, logger);

    first_node.set_executed(true);
    teardown_node(first_node, e);
  }

  // These methods are only accessed by ossia::graph
  static bool can_execute(graph_node& node, const execution_state&)
  {
    return all_of_inlet(node, [](const auto& inlet) {
      // A port can execute if : - it has source ports and all its source
      // ports have executed

      // if there was a strict connection, this node would have been
      // disabled so we can just do the following check.
      bool previous_nodes_executed = ossia::all_of(inlet.sources, [&](graph_edge* edge) {
        return edge->out_node->executed()
               || (!edge->out_node->enabled() /* && bool(inlet->address) */
                   /* TODO check that it's in scope */);
      });

      // it does not have source ports ; we have to check for variables :
      // find if address available in local / global scope
      return !inlet.sources.empty() ? previous_nodes_executed : true; // TODO
    });
  }

  static void finish_nodes(const node_map& nodes)
  {
    for(auto& node : nodes)
    {
      ossia::graph_node& n = *node.first;
      n.set_executed(false);
      n.disable();
    }
  }

  template <typename DevicesT>
  static auto find_address_connection(
      ossia::graph_node& source, ossia::graph_node& sink, const DevicesT& devices)
  {
    return any_of_outlet(source, [&](auto& outlet) {
      return any_of_inlet(sink, [&](auto& inlet) {
        bool ok = false;
        apply_to_destination(
            outlet.address, devices,
            [&](ossia::net::parameter_base* p1, bool) {
          apply_to_destination(
              inlet.address, devices,
              [&](ossia::net::parameter_base* p2, bool) {
            if(p1 == p2)
              ok = true;
              },
              do_nothing_for_nodes{});
            },
            do_nothing_for_nodes{});
        return ok;
      });
    });
  }
};

struct OSSIA_EXPORT graph_base : graph_interface
{
  graph_base() noexcept
  {
#if !defined(OSSIA_FREESTANDING)
    m_nodes.reserve(1024);
    m_node_list.reserve(1024);
    m_edges.reserve(1024);
#endif
  }
  [[nodiscard]] std::span<ossia::graph_node* const>
  get_nodes() const noexcept final override
  {
    return std::span{m_node_list};
  }

  void recompute_maps()
  {
    // TODO we should instead mark it for cleaning and do it once per tick ?
    m_nodes.clear();
    m_edges.clear();
    auto vertices = boost::vertices(m_graph);
    m_nodes.reserve(m_graph.m_vertices.size());
    for(auto it = vertices.first; it != vertices.second; ++it)
    {
      graph_vertex_t k = *it;
      node_ptr n = m_graph[k];
      assert(n);

      m_nodes.insert({n, k});
    }

    // https://svn.boost.org/trac10/ticket/5706#no1
#if !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    auto edges = boost::edges(m_graph);
    m_edges.reserve(std::distance(edges.first, edges.second));
    for(auto it = edges.first; it != edges.second; ++it)
    {
      graph_edge_t k = *it;
      edge_ptr n = m_graph[k];
      assert(n);

      m_edges.insert({n, k});
    }
#if !defined(__clang__)
#pragma GCC diagnostic pop
#endif
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
    if(m_nodes.find(n) == m_nodes.end())
    {
      add_node_impl(std::move(n));
    }
  }

  void remove_node(const node_ptr& n) final override
  {
    for_each_inlet(*n, [&](auto& port) {
      auto s = port.sources;
      for(auto edge : s)
        disconnect(edge);
    });
    for_each_outlet(*n, [&](auto& port) {
      auto s = port.targets;
      for(auto edge : s)
        disconnect(edge);
    });

    auto it = m_nodes.find(n);
    if(it != m_nodes.end())
    {
      auto vtx = boost::vertices(m_graph);
      if(std::find(vtx.first, vtx.second, it->second) != vtx.second)
      {
        boost::clear_vertex(it->second, m_graph);
        remove_vertex(it->second, m_graph);

        recompute_maps();
      }
      // no need to erase it since it won't be here after recompute_maps
    }
    ossia::remove_one(m_node_list, n.get());
    m_dirty = true;
  }

  void connect(std::shared_ptr<graph_edge> edge) final override
  {
    if(edge)
    {
      edge->init();

      graph_vertex_t in_vtx, out_vtx;
      auto it1 = m_nodes.find(edge->in_node);
      if(it1 == m_nodes.end())
        in_vtx = add_node_impl(edge->in_node);
      else
        in_vtx = it1->second;

      auto it2 = m_nodes.find(edge->out_node);
      if(it2 == m_nodes.end())
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
    if(edge)
    {
      edge->clear();
      auto it = m_edges.find(edge);
      if(it != m_edges.end())
      {
        auto edg = boost::edges(m_graph);
        if(std::find(edg.first, edg.second, it->second) != edg.second)
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
    for(auto& edge : m_edges)
    {
      edge.first->clear();
    }
    for(auto& node : m_nodes)
    {
      node.first->clear();
    }
    m_dirty = true;
    m_nodes.clear();
    m_node_list.clear();
    m_edges.clear();
    m_graph.clear();
  }

  void mark_dirty() final override { m_dirty = true; }

  ~graph_base() override { clear(); }

  node_map m_nodes;
  edge_map m_edges;
#if defined(OSSIA_FREESTANDING)
  ossia::small_vector<ossia::graph_node*, 16> m_node_list;
#else
  ossia::small_vector<ossia::graph_node*, 1024> m_node_list;
#endif

  graph_t m_graph;

  bool m_dirty{};
};
}
