#pragma once

#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/network/base/device.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/bimap.hpp>
#include <boost/functional/hash.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
namespace std
{
template<typename T>
class hash<std::pair<T*, T*>>
{
public:
  std::size_t operator()(const std::pair<T*, T*>& p) const
  {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.first);
    boost::hash_combine(seed, p.second);
    return seed;
  }
};
}
namespace ossia
{

struct glutton_connection { };
struct strict_connection {
  enum required_sides_t {
    inbound = 1 << 0,
    outbound = 1 << 1,
    both = inbound | outbound
  };

  required_sides_t required_sides;
};
struct temporal_glutton_connection { };
struct delayed_glutton_connection {
  // delayed at the source or at the target
};
struct delayed_needful_connection {
  // same
};
struct parallel_connection {
};
struct replacing_connection {
};

// An explicit dependency required by the composer.
struct dependency_connection {
};

using connection = eggs::variant<
glutton_connection,
strict_connection,
delayed_glutton_connection,
delayed_needful_connection,
parallel_connection,
replacing_connection,
dependency_connection>;

struct audio_port {

};

struct midi_port {

};
struct port;
struct value_port {
  ossia::value val;
};

struct port
{
  using data_type = eggs::variant<audio_port, midi_port, value_port>;
  port(data_type d):
    data{std::move(d)}
  {
  }
  data_type data;

  eggs::variant<ossia::Destination, ossia::traversal::path, std::shared_ptr<port>> destination;

  enum allowed_input_type {
    none = 1 << 0,
    local = 1 << 1,
    global = 1 << 2,
    both = local | global
  };

  allowed_input_type input;
};
class graph_node;

// A pure dependency edge does not have in/out ports set

struct graph_edge
{
  graph_edge(connection c, std::shared_ptr<port> out, std::shared_ptr<port> in,
             std::shared_ptr<graph_node> out_node, std::shared_ptr<graph_node> in_node):
    con{c},
    out{out},
    in{in},
    out_node{out_node},
    in_node{in_node}
  {

  }

  connection con;
  std::shared_ptr<port> out;
  std::shared_ptr<port> in;
  std::shared_ptr<graph_node> out_node;
  std::shared_ptr<graph_node> in_node;
};

using target = eggs::variant<
ossia::net::address_base*,
std::string // ossia::traversal::path
>;

struct execution_state
{
  std::vector<ossia::net::device_base*> globalState;
  std::unordered_map<target, ossia::value> localState;
};

template<typename T, typename... Args>
auto make_port(Args&&... args)
{
  return std::make_shared<port>(T{std::forward<Args>(args)...});
}

using ports = std::vector<std::shared_ptr<port>>;

class graph_node
{
protected:
  // Note : pour QtQuick : Faire View et Model qui hérite de View, puis faire binding automatique entre propriétés de la vue et du modèle
  // Utiliser... DSPatch ? Pd ?
  // Ports : midi, audio, value

public:
  std::vector<std::shared_ptr<port>> in_ports;
  std::vector<std::shared_ptr<port>> out_ports;

  double previous_time{};
  double time{};

  bool m_enabled{};
  virtual ~graph_node()
  {
    // TODO moveme in cpp
  }

  graph_node()
  {
  }

  bool enabled() const { return m_enabled; }
  void set_enabled(bool b) { m_enabled = b; }

  virtual void event(time_value date, ossia::state_element& st)
  {

  }

  virtual bool consumes(execution_state&)
  {
    return false;
  }

  virtual void run(execution_state& e)
  {

  }

  void set_time(double d)
  {
    previous_time = time;
    time = d;
  }

  const ports& inputs() const { return in_ports; }
  const ports& outputs() const { return out_ports; }
};


template<typename T>
using set = boost::container::flat_set<T>;
template<typename T, typename U>
using bimap = boost::bimap<T, U>;
class graph : public ossia::time_process
{
  using graph_t = boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::directedS,
  std::shared_ptr<graph_node>,
  std::shared_ptr<graph_edge>>;

  using graph_vertex_t = graph_t::vertex_descriptor;
  using graph_edge_t = graph_t::edge_descriptor;

  using node_bimap = bimap<graph_vertex_t, std::shared_ptr<graph_node>>;
  using edge_bimap = bimap<graph_edge_t, std::shared_ptr<graph_edge>>;
  using node_bimap_v = node_bimap::value_type;
  using edge_bimap_v = edge_bimap::value_type;

  node_bimap nodes;
  edge_bimap edges;

  set<std::shared_ptr<graph_node>> enabled_nodes;

  std::vector<int> delay_ringbuffers;


  graph_t user_graph;

  std::vector<std::function<void(execution_state&)>> call_list;

  std::unordered_map<std::pair<graph_node*, graph_node*>, connection> connection_types;

  time_value current_time{};
public:
  void add_node(const std::shared_ptr<graph_node>& n)
  {
    nodes.insert(node_bimap_v{boost::add_vertex(n, user_graph), n});
    reconfigure();
  }

  void remove_node(const std::shared_ptr<graph_node>& n)
  {
    auto it = nodes.right.find(n);
    if(it != nodes.right.end())
    {
      boost::remove_vertex(it->second, user_graph);
      nodes.right.erase(it);
      reconfigure();
    }
  }

  void enable(const std::shared_ptr<graph_node>& n)
  {
    enabled_nodes.insert(n);
    n->set_enabled(true);
    reconfigure();
  }

  void disable(const std::shared_ptr<graph_node>& n)
  {
    enabled_nodes.erase(n);
    n->set_enabled(false);
    reconfigure();
  }

  void connect(const std::shared_ptr<graph_edge>& edge)
  {
    auto it1 = nodes.right.find(edge->in_node);
    auto it2 = nodes.right.find(edge->out_node);
    if(it1 != nodes.right.end() && it2 != nodes.right.end())
    {
      auto res = boost::add_edge(it1->second, it2->second, edge, user_graph);
      if(res.second)
      {
        edges.insert(edge_bimap_v{res.first, edge});
      }
    }

    reconfigure();
  }

  void disconnect(const std::shared_ptr<graph_edge>& edge)
  {
    auto it = edges.right.find(edge);
    if(it != edges.right.end())
    {
      boost::remove_edge(it->second, user_graph);
      reconfigure();
    }
  }

  void reconfigure()
  {
    return;
    call_list.clear();
    std::deque<graph_vertex_t> topo_order;
    try {
      boost::topological_sort(user_graph, std::front_inserter(topo_order));
    }
    catch(const boost::not_a_dag& e)
    {
      return;
    }

    // topo_order contains a list of connected things starting from the root and going through the leaves
    // now how to handle nodes with the same parameters without specification ?

    // two kinds of edges : explicit edges and implicit edges

    // note : implicit edges must be checked at runtime (imagine a js process that writes to random adresses on each tick)
    // this also means that besides the policy, the adresses should have a default behaviour set.
    // Or maybe it should be per node or per port ?

    // Maybe we should have the "connection graph", between ports, and the "node graph" where the user explicitely says
    // he wants a process to be executed before another. And a check that both aren't contradictory.

    // What we cannot do, however, is know automatically which process reads which address : it works only for outbound ports.
    // Inbound ports have to be specified. Or maybe we can have an evaluation step where the "undefined" inputs are asked "would you accept this address" ?


    // We can make a proper graph with "defined" outputs.
    // But if it has "undefined" outputs, we have to make a step to check where the undefined value should go.

    // Maybe in the course of execution, we should have a "sub-environment" for a given tick (map<address, value>) ?
    // Also, the "order of evalution" graph saves us. It should be given first by the order of the constraints / processes / etc. and then adapted with the io ports.

    // This allows "wildcards" & stuff like this.

    // We are trying to do two things : first make a good run-time algorithm (ran at each tick, like i-score's current message algorithm)
    // and then try to optimize by doing most of the cabling before run-time. (or at each change of the graph).

    // Note : clock accuracy : see LibAudioStream. while(command in stream) { apply(command) }
    // we have to give the ability of graph nodes to give dates to messages
    for(graph_vertex_t vtx : topo_order)
    {
      const auto& node = user_graph[vtx];
      call_list.push_back([=] (execution_state& e) {
        node->run(e);
      });
    }

    // To put in the article :


    // Two orthogonal problems, but one stems from the another.
    // We want to have temporal dataflows. We want this flow to control outisde parameters. Due to the temporal nature,
    // some nodes of the graph might not always be available. So what must we do ? And what new opportunities does this bring ?

    // Give example of video filter : first change brightness, then from t=5 to t=25 change contrast

  }

  state_element offset(time_value) override
  {
    return {};
  }

  void glutton_exec()
  {

  }


  state_element state() override
  {
    auto active_nodes = enabled_nodes;

    // Should we first filter disable nodes (through strict relationships) or get a total order ?

    /*
    // We first have to do a partition in connected components
    std::vector<int> components(active_nodes.size());

    boost::connected_components(user_graph, components.data());

    std::vector<std::vector<int>> components_2(std::max(components.begin(), components.end()));
    for(auto& vec : components_2)
      vec.reserve(std::log2(active_nodes.size()));
    for(int vtx = 0; vtx < components.size(); vtx++)
    {
      components_2[components[vtx]].push_back(vtx);
    }

    // Remove disabled subgraphs if they have strict connections
    boost::range::remove_erase_if(components_2, [&] (const auto& vec) {
      return any_of(*it, [&] (const int vtx) {
              return !user_graph[vtx]->enabled();
            });
    });
    */


    // For strict relationships : disable all the nodes if one is not connected.
    std::vector<graph_node*> to_disable;
    for(auto e_pair : this->edges.right)
    {
      auto e = e_pair.first;

      /*

      if(e->con != strict_connection{})
        continue;

      // Strict relationship : inbound, outbound, both ?
      if(e->in_node && !e->in_node->enabled() && e->out_node && e->out_node->enabled())
        to_disable.push_back(out_node.get());
      else if(e->out_node && !e->out_node->enabled() && e->in_node && e->in_node->enabled())
        to_disable.push_back(in_node.get());
        */
    }

    while(!to_disable.empty())
    {
      for(graph_node* e : to_disable)
        e->set_enabled(false);
    }

    // First we have to set-up the graph for the current tick, depending on the enabled nodes, their state,
    // and connection status.

    // Transitivity of dependencies : if a -> b -> c and b is disabled and c is enabled, we keeep a -> b
    // ((maybe  it will do nothing if there are no compatible ports)



    // Then we run it.
    std::deque<graph_vertex_t> topo_order;
    try {
      boost::topological_sort(user_graph, std::back_inserter(topo_order));
    }
    catch(const boost::not_a_dag& e)
    {
      return {};
    }

    execution_state e;

    for(graph_vertex_t vtx : topo_order)
    {
      const auto node = user_graph[vtx];
      node->run(e);
      // Check if new nodes may be reachable with the state ? does this ever happen ?
    }

    // Commit the localstate to the globalstate


    // First algorithm : doing it by hand.

    // We have to have an order between processes at some point.


    // Note : the graph traversal has to be done by hand, since it depends on the output value of each node.

    // Think of the case : [ /b -> f1 -> ...] in glutton mode : if there is no other node to get /b from we have to get it from the tree.

    // How to do it in the implicit and explicit case ? Implicit : acceptedData() ? it's simple for adresses, but what about audio, etc... ?
    // Between two calls : look for all the nodes that are after this one and are able to accept the address / etc.

    // The data graph maybe have to be hierarchical, too ? this way for instance we can mix sounds only at a given level.

    // Propagate :
    // - Input Data
    // - Tick
    // - Find the next set of nodes that will be called
    // - Input Data
    // - Tick

    // First : find the "roots" of the graph, and call on them with an empty execution state.
    /*
    execution_state s;
    // Pull the graph
    for(auto& call : call_list)
      call(s);

    */

    // We have to define a "sum operation" for each kinde of data we want to manipulate.
    // "parameters" : replace
    // midi : add notes, replace CCs
    // audio : sum
    return {};

  }
};


class graph_process : public ossia::time_process
{
  std::shared_ptr<ossia::graph> graph;
  std::shared_ptr<ossia::graph_node> node;

public:
  ossia::state_element offset(ossia::time_value) override
  {
    return {};
  }

  ossia::state_element state() override
  {
    node->set_time(parent()->getDate() / parent()->getDurationNominal());
    return {};
  }

  void start() override
  {
    graph->enable(node);
  }

  void stop() override
  {
    graph->disable(node);
  }

  void pause() override
  {
  }

  void resume() override
  {
  }

  void mute_impl(bool) override
  {
  }
};
}
