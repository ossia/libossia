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
#include <ModernMIDI/midi_message.h>
#include <deque>
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
using target = eggs::variant<
ossia::net::address_base*,
std::string // ossia::traversal::path
>;
class graph_node;
struct graph_edge;
using node_ptr = std::shared_ptr<graph_node>;
using edge_ptr = std::shared_ptr<graph_edge>;
struct port;
struct outlet;
struct inlet;
using inlet_ptr = std::shared_ptr<inlet>;
using outlet_ptr = std::shared_ptr<outlet>;
struct base_pair { graph_edge* edge{}; };
using inlet_pair = base_pair;
using outlet_pair = base_pair;
inline bool operator==(base_pair lhs, base_pair rhs) { return lhs.edge == rhs.edge; }


struct glutton_connection { };
struct strict_connection {
  enum required_sides_t {
    inbound = 1 << 0,
    outbound = 1 << 1,
    both = inbound | outbound
  };

  required_sides_t required_sides{both};
};
struct temporal_glutton_connection { };
struct delayed_glutton_connection {
  // delayed at the source or at the target
};
struct delayed_needful_connection {
  // same
};
struct reduction_connection {
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
reduction_connection,
replacing_connection,
dependency_connection>;

struct audio_port {
  std::vector<std::array<double, 64>> samples;
};

struct midi_port {
  std::deque<mm::MidiMessage> messages;
};

struct value_port {
  std::deque<ossia::value> data;
};

using data_type = eggs::variant<audio_port, midi_port, value_port>;
struct inlet_source_t : public eggs::variant<ossia::Destination, std::string, outlet_pair> { using variant::variant; };
struct outlet_target_t : public eggs::variant<ossia::Destination, std::string, inlet_pair> { using variant::variant; };
struct port
{
  data_type data;

  enum scope_t {
    none = 1 << 0,
    local = 1 << 1,
    global = 1 << 2,
    both = local | global
  };

  scope_t scope{scope_t::both};

  virtual ~port() = default;
protected:
  port(data_type d):
    data{std::move(d)}
  {
  }

  port() = delete;
  port(const port&) = delete;
  port(port&&) = delete;
  port& operator=(const port&) = delete;
  port& operator=(port&&) = delete;
};

struct inlet : public port
{
  virtual ~inlet() = default;
  inlet(data_type d): port{std::move(d)} { }
  inlet(data_type d, inlet_source_t dest):
    port{std::move(d)}
  {
    sources.push_back(std::move(dest));
  }

  std::vector<inlet_source_t> sources;
};
struct outlet : public port
{
  virtual ~outlet() = default;
  outlet(data_type d): port{std::move(d)} { }
  outlet(data_type d, outlet_target_t dest):
    port{std::move(d)}
  {
    targets.push_back(std::move(dest));
  }

  std::vector<outlet_target_t> targets;
};


// A pure dependency edge does not have in/out ports set

struct graph_edge
{
  graph_edge(connection c, outlet_ptr pout, inlet_ptr pin,
             std::shared_ptr<graph_node> pout_node, std::shared_ptr<graph_node> pin_node):
    con{c},
    out{std::move(pout)},
    in{std::move(pin)},
    out_node{std::move(pout_node)},
    in_node{std::move(pin_node)}
  {
    if(in && out)
    {
      out->targets.push_back(inlet_pair{this});
      in->sources.push_back(outlet_pair{this});
    }
  }

  ~graph_edge()
  {
    if(in && out)
    {
      boost::remove_erase(out->targets, inlet_pair{this});
      boost::remove_erase(in->sources, outlet_pair{this});
    }
  }

  connection con;
  outlet_ptr out;
  inlet_ptr in;
  std::shared_ptr<graph_node> out_node;
  std::shared_ptr<graph_node> in_node;
};

struct execution_state
{
  std::vector<ossia::net::device_base*> globalState;
  std::unordered_map<target, data_type> localState;
};

template<typename T, typename... Args>
auto make_inlet(Args&&... args)
{
  return std::make_shared<inlet>(T{}, std::forward<Args>(args)...);
}
template<typename T, typename... Args>
auto make_outlet(Args&&... args)
{
  return std::make_shared<outlet>(T{}, std::forward<Args>(args)...);
}

using ports = std::vector<std::shared_ptr<port>>;
using inlets = std::vector<inlet_ptr>;
using outlets = std::vector<outlet_ptr>;

class graph_node
{
protected:
  // Note : pour QtQuick : Faire View et Model qui hérite de View, puis faire binding automatique entre propriétés de la vue et du modèle
  // Utiliser... DSPatch ? Pd ?
  // Ports : midi, audio, value

public:
  inlets in_ports;
  outlets out_ports;

  double previous_time{};
  double time{};

  bool m_enabled{};
  bool m_executed{};
  virtual ~graph_node()
  {
    // TODO moveme in cpp
  }

  graph_node()
  {
  }

  bool enabled() const { return m_enabled; }
  void set_enabled(bool b) { m_enabled = b; }

  bool executed() const { return m_executed; }
  void set_executed(bool b) { m_executed = b; }

  virtual void event(time_value date, ossia::state_element& st)
  {

  }

  virtual bool consumes(const std::string&)
  {
    return false;
  }
  virtual bool consumes(const Destination&)
  {
    return false;
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

  const auto& inputs() const { return in_ports; }
  const auto& outputs() const { return out_ports; }
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

  set<std::shared_ptr<graph_node>> user_enabled_nodes;

  std::vector<int> delay_ringbuffers;


  graph_t user_graph;

  std::vector<std::function<void(execution_state&)>> call_list;

  using edge_map_t = std::unordered_map<std::pair<graph_node*, graph_node*>, graph_edge*>;
  edge_map_t edge_map;

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
    user_enabled_nodes.insert(n);
    n->set_enabled(true);
    // TODO handle temporal ordering here.
    reconfigure();
  }

  void disable(const std::shared_ptr<graph_node>& n)
  {
    user_enabled_nodes.erase(n);
    n->set_enabled(false);
    reconfigure();
  }

  void connect(const std::shared_ptr<graph_edge>& edge)
  {
    auto it1 = nodes.right.find(edge->in_node);
    auto it2 = nodes.right.find(edge->out_node);
    if(it1 != nodes.right.end() && it2 != nodes.right.end())
    {
      // TODO check that two edges can be added
      auto res = boost::add_edge(it1->second, it2->second, edge, user_graph);
      if(res.second)
      {
        edges.insert(edge_bimap_v{res.first, edge});
        edge_map.insert(std::make_pair(std::make_pair(edge->in_node.get(), edge->out_node.get()), edge.get()));
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
      edge_map.erase(std::make_pair(edge->in_node.get(), edge->out_node.get()));
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

  void value_reduction(const outlets& outs)
  {

  }

  void audio_reduction(const outlets& outs)
  {

  }

  void midi_reduction(const outlets& outs)
  {

  }

  static auto disable_strict_nodes(std::set<node_ptr> enabled_nodes)
  {
    decltype(enabled_nodes) ret;

    for(auto node : enabled_nodes)
    {
      for(auto in : node->in_ports)
      {
        for(auto src : in->sources)
        {
          if(auto outlet = src.target<outlet_pair>())
          {
            auto edge = outlet->edge;
            assert(edge);
            assert(edge->out_node);

            if(auto sc = edge->con.target<strict_connection>())
            {
              if((sc->required_sides & strict_connection::required_sides_t::outbound) && node->enabled() && !edge->out_node->enabled())
              {
                ret.insert(node);
              }
            }
          }
        }
      }

      for(auto out : node->out_ports)
      {
        for(auto src : out->targets)
        {
          if(auto inlet = src.target<inlet_pair>())
          {
            auto edge = inlet->edge;
            assert(edge);
            assert(edge->in_node);

            if(auto sc = edge->con.target<strict_connection>())
            {
              if((sc->required_sides & strict_connection::required_sides_t::inbound) && node->enabled() && !edge->in_node->enabled())
              {
                ret.insert(node);
              }
            }
          }
        }
      }
    }

    return ret;
  }

  void disable_strict_nodes_rec(std::set<node_ptr> cur_enabled_node)
  {
    std::set<node_ptr> to_disable;
    do
    {
      to_disable = disable_strict_nodes(cur_enabled_node);
      for(auto n : to_disable)
      {
        disable(n);
        cur_enabled_node.erase(n);
        // note: we have to add a dependency between all the inlets and outlets
      }

    } while(!to_disable.empty());

  }

  bool can_execute(const graph_node& n, const execution_state& st)
  {
    return ossia::any_of(n.in_ports, [] (const inlet_ptr& inlet) {
      return ossia::all_of(inlet->sources, [] (const inlet_source_t& pt) {
        if(auto e = pt.target<outlet_pair>())
        {
          return e->edge->out_node->executed();
        }
        else
        {
          return true;
          // find if address available in local / global scope
        }
      });
    });
  }

  struct active_node_sorter
  {
    edge_map_t& edge_map;
    execution_state& st;
    bool has_port_inputs(graph_node& n) const
    {
      return ossia::any_of(n.in_ports, [] (const inlet_ptr& inlet) {
        return ossia::any_of(inlet->sources, [] (const inlet_source_t& pt) {
          return pt.target<outlet_pair>() != nullptr;
        });
      });
    }
    bool has_global_inputs(graph_node& n) const
    {
      return ossia::any_of(n.in_ports, [&] (const inlet_ptr& inlet) {
        return inlet->scope & port::scope_t::global;
      });
    }

    bool has_local_inputs(graph_node& n) const
    {
      return ossia::any_of(n.in_ports, [&] (const inlet_ptr& inlet) {
        if(inlet->scope & port::scope_t::local)
        {
          bool is_explicit = ossia::any_of(inlet->sources, [&] (const inlet_source_t& pt) {
            if(auto dest = pt.target<ossia::Destination>())
            {
              return ossia::any_of(st.localState, [&] (const auto& pair) {
                auto addr = pair.first.template target<ossia::net::address_base*>();
                if(addr && *dest == **addr)
                  return true;

                //                if(auto match = pair.second.target<std::string>())
                {
                  // TODO check if there is any pattern matching the current destination
                }

                return false;
              });
            }
            // else if(auto pattern = pt.target<std::string>())
            // {
            // what happens if a pattern matches anotehr pattern
            //   return n.consumes(*pattern);
            // }
            return false;
          });

          if(is_explicit)
            return true;
          else if(n.consumes(st))
            return true;
        }
        return false;
      });
    }

    bool edge_order(graph_node* lhs, graph_node* rhs) const
    {
      if(edge_map.find({lhs, rhs}) != edge_map.end())
      {
        return true;
      }
      else
      {
        // Actually there are two cases here:
        // There is an edge from rhs to lhs so rhs < lhs
        // Or there is no edge and we do not define the behaviour
        return false;
      }
    }

    bool operator()(graph_node* lhs, graph_node* rhs) const
    {
      bool c1 = has_port_inputs(*lhs);
      bool c2 = has_port_inputs(*rhs);
      if(c1 && !c2) return true;
      else if(!c1 && c2) return false;
      else if(c1 && c2) return edge_order(lhs, rhs);

      bool l1 = has_local_inputs(*lhs);
      bool l2 = has_local_inputs(*rhs);

      if(l1 && !l2) return true;
      else if(!l1 && l2) return false;
      else if(l1 && l2) return edge_order(lhs, rhs);

      bool g1 = has_global_inputs(*lhs);
      bool g2 = has_global_inputs(*rhs);
      if(g1 && !g2) return true;
      else if(!g1 && g2) return false;
      else if(g1 && g2) return edge_order(lhs, rhs);

      return true;

    }
  };

  struct clear_data
  {
    void operator()(value_port& p)
    {
      p.data.clear();
    }
    void operator()(midi_port& p)
    {
      p.messages.clear();
    }
    void operator()(audio_port& p)
    {
      p.samples.clear();
    }
  };

  struct copy_data
  {
    template<typename T, typename U>
    void operator()(const T&, const U&) const { }

    void operator()(const value_port& out, value_port& in)
    {
      for(const auto& v : out.data)
        in.data.push_back(v);
    }

    void operator()(const audio_port& out, audio_port& in)
    {
      in.samples.reserve(in.samples.size() + out.samples.size());
      for(const auto& s : out.samples)
        in.samples.push_back(s);
    }

    void operator()(const midi_port& out, midi_port& in)
    {
      for(const auto& v : out.messages)
        in.messages.push_back(v);
    }
  };


  void copy(const data_type& out, inlet& in)
  {
    if(out.which() == in.data.which() && out && in.data)
    {
      eggs::variants::apply(copy_data{}, out, in.data);
    }
  }

  void copy(const outlet& out, inlet& in)
  {
    copy(out.data, in);
  }


  void copy_local(const data_type& out, const Destination& d, execution_state& in)
  {
    in.localState[target{&d.address()}]= out;
  }
  void copy_global(const data_type& out, const Destination& d, execution_state& in)
  {
    // TODO
  }

  void copy(const ossia::net::address_base& out, inlet& in)
  {
    if(value_port* val = in.data.target<value_port>())
    {
      val->data.push_back(out.cloneValue());
    }
  }
  void init_node(graph_node& n, execution_state& e)
  {
    // Clear the outputs of the node
    for(const outlet_ptr& out : n.out_ports)
    {
      if(out->data)
        eggs::variants::apply(clear_data{}, out->data);
    }

    // Copy from environment and previous ports to inputs
    for(const inlet_ptr& in : n.in_ports)
    {
      for(const inlet_source_t& src : in->sources)
      {
        if(auto outlet = src.target<outlet_pair>())
        {
          copy(*outlet->edge->out, *in);
        }
        else if(auto addr = src.target<ossia::Destination>())
        {
          if(in->scope & port::scope_t::local)
          {
            auto it = e.localState.find(target{&addr->address()});
            if(it != e.localState.end())
              copy(it->second, *in);
            else if(in->scope & port::scope_t::global)
              copy(addr->address(), *in);
          }
          else if(in->scope & port::scope_t::global)
            copy(addr->address(), *in);
        }
        else if(auto pattern = src.target<std::string>())
        {
          // TODO
        }
      }
    }
  }

  void teardown_node(graph_node& n, execution_state& e)
  {
    for(const inlet_ptr& in : n.in_ports)
    {
      std::cerr << in->data.target<value_port>() << std::endl;
      if(in->data)
        eggs::variants::apply(clear_data{}, in->data);
    }

    // Copy from output ports to environment
    for(const outlet_ptr& out : n.out_ports)
    {
      for(const outlet_target_t& tgt : out->targets)
      {
        if(auto addr = tgt.target<ossia::Destination>())
        {
          if(out->scope & port::scope_t::local)
          {
            copy_local(out->data, *addr, e);
          }
          else if(out->scope & port::scope_t::global)
          {
            copy_global(out->data, *addr, e);
          }
        }
        else if(auto pattern = tgt.target<std::string>())
        {
          // TODO
        }
      }
    }

  }

  state_element state() override
  {
    // There should be a first "activation" pass from the temporal algorithm

    // Filter disabled nodes (through strict relationships).
    disable_strict_nodes_rec(std::set<node_ptr>(user_enabled_nodes.begin(), user_enabled_nodes.end()));

    // Get a total order on nodes
    std::vector<graph_node*> ordered_nodes;

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
      auto n = user_graph[vtx].get();
      if(n->enabled())
        ordered_nodes.push_back(n);
    }

    { // debug
      for(auto n : ordered_nodes)
        std::cerr << "node: " << (void*) n << "; ";
      std::cerr << "size: " << ordered_nodes.size() << std::endl << std::endl;
    }
    auto active_nodes = ordered_nodes;
    while(!active_nodes.empty())
    {
      std::vector<graph_node*> next_nodes;
      std::size_t max_exec{};
      for(; max_exec < active_nodes.size(); max_exec++)
        if(can_execute(*active_nodes[max_exec], e))
          next_nodes.push_back(active_nodes[max_exec]);

      std::sort(next_nodes.begin(), next_nodes.end(), active_node_sorter{edge_map, e});

      { // debug
        for(auto n : next_nodes)
          std::cerr << "node: " << (void*) n << "; ";
        std::cerr << "size: " << active_nodes.size() << std::endl << std::endl;
      }
      if(!next_nodes.empty())
      {
        // First look if there is a replacement or reduction relationship between the first n nodes
        // If there is, we run all the nodes

        // If there is not we just run the first node
        init_node(*next_nodes[0], e);
        next_nodes[0]->run(e);
        next_nodes[0]->set_executed(true);
        teardown_node(*next_nodes[0], e);
        active_nodes.erase(ossia::find(active_nodes, next_nodes[0]));
      }
      else
      {
        break; // nothing more to execute
      }
      // N nodes have executed already, and the state has some values in it.
      // First look for all the nodes with direct inputs that can be executed. The first that can't is a max bound.
      // Then look for all the nodes with glutton inputs that can be executed.

      // For each step, get the list of the nodes that should execute at this tick
      // i.e. find the first node that may depend on another node executing.
      // note : there is an incompatibility between :
      // - the reduce operation which needs to work on "n" nodes
      // - the implicit operation which requires to execute each node in order to find if a following node may have something it requires

      // Simple case :
      // copy data to input ports of node
      // run node
      // copy data from input to output of node

      // Execute them

      // Then remove them from the list of nodes to execute at the current step.

      // Then when there is no remaining node, commit the current execution state

    }

    for(auto& elt: e.localState)
    {
      auto addr = elt.first.target<ossia::net::address_base*>();
      auto val = elt.second.target<value_port>();

      if(addr && val)
      {
        for(auto v : val->data)
        {
          (*addr)->pushValue(v);
        }
      }
    }

    for(auto& node : nodes.right)
    {
      node.first->set_executed(false);
    }
    return {};



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


    // First we have to set-up the graph for the current tick, depending on the enabled nodes, their state,
    // and connection status.

    // Transitivity of dependencies : if a -> b -> c and b is disabled and c is enabled, we keeep a -> b
    // ((maybe  it will do nothing if there are no compatible ports)



    // Then we run it.
    /*
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
    */

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
