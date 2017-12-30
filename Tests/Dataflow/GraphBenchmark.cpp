// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph.hpp>
#include <ossia/network/base/node_functions.hpp>
#include "../Editor/TestUtils.hpp"
#include <valgrind/callgrind.h>
#include <brigand/algorithms/for_each.hpp>
#include <brigand/sequences/list.hpp>
#include <random>
#include <QCoreApplication>
#include <QTextStream>
#include <QFile>

static const constexpr int NUM_TAKES = 100;
static const constexpr auto NUM_NODES = {1, 2, 3, 4, 5, 10, 15, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100, 150, 200, 250, 500, 750, 1000};
//static const constexpr auto NUM_NODES = {1, 30, 50};

static std::random_device rd{};
static std::mt19937 mt{rd()};

using namespace ossia;
namespace ossia
{
using bfs_graph = ossia::graph_static_base<ossia::static_graph_policy::bfs>;
using tc_graph = ossia::graph_static_base<ossia::static_graph_policy::transitive_closure>;
}
std::string graph_kind(const ossia::graph& g)
{
  return "dynamic";
}
std::string graph_kind(const ossia::bfs_graph& g)
{
  return "bfs";
}
std::string graph_kind(const ossia::tc_graph& g)
{
  return "transitive_closure";
}

template<typename Port_T>
class node_empty_mock final : public graph_node {
public:
  std::string lbl{};
  std::string_view label() const override
  {
    return lbl;
  }
  node_empty_mock()
  {
    m_inlets.push_back(ossia::make_inlet<Port_T>());
    m_outlets.push_back(ossia::make_outlet<Port_T>());
  }

  void run(token_request t, execution_state& e) override
  {
  }
};

using value_mock = node_empty_mock<ossia::value_port>;
using audio_mock = node_empty_mock<ossia::audio_port>;
using midi_mock = node_empty_mock<ossia::midi_port>;

using benchmark = std::map<int, double>;
struct benchmarks
{
  benchmark dynamic;
  benchmark static_clean;
  benchmark bfs;
  benchmark tc;
};


template<typename T, typename U>
auto measure_dirty_tick(T& g, const U& nodes)
{
  ossia::execution_state e;

  double count = 0;
  for(int i = 0; i < NUM_TAKES; i++)
  {
    g.mark_dirty();
    for(auto& node : nodes)
      node->requested_tokens.push_back({});
    auto t0 = std::chrono::high_resolution_clock::now();
    CALLGRIND_START_INSTRUMENTATION;
    g.state(e);
    CALLGRIND_STOP_INSTRUMENTATION;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto this_count = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    if(this_count > 1000 * 1000)
      throw std::runtime_error("too long");
    count += this_count;

  }
  return count / double(NUM_TAKES);
}

template<typename T, typename U>
auto measure_clean_tick(T& g, const U& nodes)
{
  ossia::execution_state e;

  // ensure that a tick happens to make it clean
  g.state(e);

  // measure
  double count = 0;
  for(int i = 0; i < NUM_TAKES; i++)
  {
    for(auto& node : nodes)
      node->requested_tokens.push_back({});
    auto t0 = std::chrono::high_resolution_clock::now();
    CALLGRIND_START_INSTRUMENTATION;
    g.state(e);
    CALLGRIND_STOP_INSTRUMENTATION;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto this_count = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    if(this_count > 1000 * 1000)
      throw std::runtime_error("too long");
    count += this_count;
  }

  return count / double(NUM_TAKES);
}

template<typename Fun>
auto test_graph(Fun setup_fun)
{
  benchmarks benchs;
  for(int num_nodes : NUM_NODES)
  {
    ossia::graph g;
    auto nodes = setup_fun(num_nodes, g);
    try {
      auto count = measure_dirty_tick(g, nodes);
      benchs.dynamic.insert({num_nodes, count});
    } catch(...) { break; }
  }
  for(int num_nodes : NUM_NODES)
  {
    ossia::bfs_graph g;
    auto nodes = setup_fun(num_nodes, g);
    try {
      auto count = measure_clean_tick(g, nodes);
      benchs.static_clean.insert({num_nodes, count});
    } catch(...) { break; }
  }
  for(int num_nodes : NUM_NODES)
  {
    ossia::bfs_graph g;
    auto nodes = setup_fun(num_nodes, g);
    try {
      auto count = measure_dirty_tick(g, nodes);
      benchs.bfs.insert({num_nodes, count});
    } catch(...) { break; }
  }
  for(int num_nodes : NUM_NODES)
  {
    ossia::tc_graph g;
    auto nodes = setup_fun(num_nodes, g);
    try {
      auto count = measure_dirty_tick(g, nodes);
      benchs.tc.insert({num_nodes, count});
    } catch(...) { break; }
  }
  CALLGRIND_DUMP_STATS;
  return benchs;
}


struct setup_serial_connected
{
  template<typename T>
  auto operator()(int num_nodes, T& g) const
  {
    std::vector<std::shared_ptr<ossia::graph_node>> nodes;

    std::shared_ptr<ossia::graph_node> prev{};
    for(int i = 0; i < num_nodes; i++)
    {
      auto n = std::make_shared<value_mock>();
      if(prev)
      {
        g.connect(ossia::make_edge(ossia::immediate_strict_connection{}
                                   , prev->outputs()[0], n->inputs()[0]
            , prev, n));
      }
      prev = n;
      nodes.push_back(n);
      g.add_node(std::move(n));
    }
    return nodes;
  }
};

struct setup_serial_address
{
  mutable TestDevice dev{};
  template<typename T>
  auto operator()(int num_nodes, T& g) const
  {
    std::vector<std::shared_ptr<ossia::graph_node>> nodes;

    auto prev_addr = ossia::net::create_node(dev.device.get_root_node(), "0").create_parameter(ossia::val_type::FLOAT);

    for(int i = 0; i < num_nodes; i++)
    {
      auto next_addr = ossia::net::create_node(dev.device.get_root_node(), std::to_string(i+1)).create_parameter(ossia::val_type::FLOAT);
      auto n = std::make_shared<value_mock>();
      n->inputs()[0]->address = prev_addr;
      n->outputs()[0]->address = next_addr;
      prev_addr = next_addr;
      nodes.push_back(n);
      g.add_node(std::move(n));
    }
    return nodes;
  }
};

struct setup_parallel_1node_address
{
  mutable TestDevice dev{};
  template<typename T>
  auto operator()(int num_port, T& g) const
  {
    std::vector<std::shared_ptr<ossia::graph_node>> nodes;

    auto n1 = std::make_shared<value_mock>();
    auto n2 = std::make_shared<value_mock>();

    g.add_node(n1);
    g.add_node(n2);
    nodes.push_back(n1);
    nodes.push_back(n2);
    for(int i = 0; i < num_port; i++)
    {
      auto out_addr = ossia::net::create_node(dev.device.get_root_node(), "out" + std::to_string(i+1)).create_parameter(ossia::val_type::FLOAT);
      auto outlet = ossia::make_outlet<ossia::value_port>();
      outlet->address = out_addr;
      n1->outputs().push_back(outlet);

      auto in_addr = ossia::net::create_node(dev.device.get_root_node(), "in" + std::to_string(i+1)).create_parameter(ossia::val_type::FLOAT);
      auto inlet = ossia::make_inlet<ossia::value_port>();
      inlet->address = in_addr;
      n2->inputs().push_back(inlet);
    }
    return nodes;
  }
};

struct setup_parallel_1node_connected
{
  mutable TestDevice dev{};
  template<typename T>
  auto operator()(int num_port, T& g) const
  {
    std::vector<std::shared_ptr<ossia::graph_node>> nodes;

    auto n1 = std::make_shared<value_mock>();
    auto n2 = std::make_shared<value_mock>();

    g.add_node(n1);
    g.add_node(n2);
    nodes.push_back(n1);
    nodes.push_back(n2);
    for(int i = 0; i < num_port; i++)
    {
      auto outlet = ossia::make_outlet<ossia::value_port>();
      n1->outputs().push_back(outlet);

      auto inlet = ossia::make_inlet<ossia::value_port>();
      n2->inputs().push_back(inlet);

      auto edge = ossia::make_edge(ossia::immediate_strict_connection{}, outlet, inlet, n1, n2);
      g.connect(edge);
    }
    return nodes;
  }
};

struct setup_parallel_address
{
  mutable TestDevice dev{};
  template<typename T>
  auto operator()(int num_nodes, T& g) const
  {
    std::vector<std::shared_ptr<ossia::graph_node>> nodes;

    for(int i = 0; i < num_nodes; i+=2)
    {
      auto addr = ossia::net::create_node(dev.device.get_root_node(), std::to_string(i)).create_parameter(ossia::val_type::FLOAT);
      auto n1 = std::make_shared<value_mock>();
      auto n2 = std::make_shared<value_mock>();
      n1->outputs()[0]->address = addr;
      n2->inputs()[0]->address = addr;
      nodes.push_back(n1);
      nodes.push_back(n2);
      g.add_node(std::move(n1));
      g.add_node(std::move(n2));
    }

    return nodes;
  }
};

struct setup_parallel_connected
{
  mutable TestDevice dev{};
  template<typename T>
  auto operator()(int num_nodes, T& g) const
  {
    std::vector<std::shared_ptr<ossia::graph_node>> nodes;

    for(int i = 0; i < num_nodes; i+=2)
    {
      auto n1 = std::make_shared<value_mock>();
      auto n2 = std::make_shared<value_mock>();

      nodes.push_back(n1);
      nodes.push_back(n2);

      auto edge = ossia::make_edge(ossia::immediate_strict_connection{}, n1->outputs()[0], n2->inputs()[0], n1, n2);
      g.add_node(std::move(n1));
      g.add_node(std::move(n2));
      g.connect(edge);
    }

    return nodes;
  }
};


template<typename T>
void setup_random_edges(const std::vector<std::shared_ptr<value_mock>>& nodes, T& g, double edge_chance)
{
  for(std::size_t i = 0; i < nodes.size(); i++)
  {
    for(std::size_t j = i + 1; j < nodes.size(); j++)
    {
      if(std::uniform_real_distribution<double>{0., 1.}(mt) < edge_chance)
      {
        auto edge = ossia::make_edge(ossia::immediate_strict_connection{},
                                     nodes[i]->outputs()[0], nodes[j]->inputs()[0],
                                     nodes[i], nodes[j]);
        g.connect(edge);
      }
    }
  }
}

void setup_random_address(value_mock& node, const std::vector<ossia::net::parameter_base*>& addresses, double addr_chance)
{
  if(!addresses.empty())
  {
    if(std::uniform_real_distribution<double>{0., 1.}(mt) <= addr_chance)
    {
      auto idx = std::uniform_int_distribution<std::size_t>{0, addresses.size() - 1}(mt);
      node.inputs()[0]->address = addresses[idx];
      node.lbl += "in: " + ossia::net::osc_parameter_string(addresses[idx]->get_node());
    }
    if(std::uniform_real_distribution<double>{0., 1.}(mt) <= addr_chance)
    {
      auto idx = std::uniform_int_distribution<std::size_t>{0, addresses.size() - 1}(mt);
      node.outputs()[0]->address = addresses[idx];
      node.lbl += "\nout: " + ossia::net::osc_parameter_string(addresses[idx]->get_node());
    }
  }
}


struct setup_random
{
  double edge_chance{1.}; // 0: no edges. 1: maximal.
  double addr_chance{1.}; // 0: no edges. 1: maximal.
  int num_addresses{0};
  mutable TestDevice dev{};
  template<typename T>
  auto operator()(int num_nodes, T& g) const
  {
    std::vector<ossia::net::parameter_base*> addresses;
    std::vector<std::shared_ptr<value_mock>> nodes;

    for(int i = 0; i < num_addresses; i++)
    {
      addresses.push_back(ossia::net::create_node(dev.device.get_root_node(), std::to_string(i)).create_parameter(ossia::val_type::FLOAT));
    }

    for(int i = 0; i < num_nodes; i++)
    {
      nodes.push_back(std::make_shared<value_mock>());
      setup_random_address(*nodes.back(), addresses, addr_chance);
      g.add_node(nodes.back());
    }

    setup_random_edges(nodes, g, edge_chance);

    return nodes;
  }
};

int main()
{
  ossia::string_map<benchmarks> benchs;
  benchs.insert(std::make_pair("serial connected", test_graph(setup_serial_connected{})));
  benchs.insert(std::make_pair("serial address", test_graph(setup_serial_address{})));

  benchs.insert(std::make_pair("parallel 1 node connected", test_graph(setup_parallel_1node_connected{})));
  benchs.insert(std::make_pair("parallel 1 node address", test_graph(setup_parallel_1node_address{})));

  benchs.insert(std::make_pair("parallel connected", test_graph(setup_parallel_connected{})));
  benchs.insert(std::make_pair("parallel address", test_graph(setup_parallel_address{})));

  benchs.insert(std::make_pair("random edge (10%)", test_graph(setup_random{0.1})));
  benchs.insert(std::make_pair("random edge (50%)", test_graph(setup_random{0.5})));
  benchs.insert(std::make_pair("random edge (90%)", test_graph(setup_random{0.9})));
  benchs.insert(std::make_pair("random edge (100%)", test_graph(setup_random{1.})));

  benchs.insert(std::make_pair("random 10 addresses (10%)", test_graph(setup_random{0., 0.1, 10})));
  benchs.insert(std::make_pair("random 10 addresses (50%)", test_graph(setup_random{0., 0.5, 10})));
  benchs.insert(std::make_pair("random 10 addresses (90%)", test_graph(setup_random{0., 0.9, 10})));
  benchs.insert(std::make_pair("random 10 addresses (100%)", test_graph(setup_random{0., 1., 10})));

  benchs.insert(std::make_pair("random 100 addresses (10%)", test_graph(setup_random{0., 0.1, 100})));
  benchs.insert(std::make_pair("random 100 addresses (50%)", test_graph(setup_random{0., 0.5, 100})));
  benchs.insert(std::make_pair("random 100 addresses (90%)", test_graph(setup_random{0., 0.9, 100})));
  benchs.insert(std::make_pair("random 100 addresses (100%)", test_graph(setup_random{0., 1., 100})));

  benchs.insert(std::make_pair("random 10 addresses (edge 10%, addr 10%)", test_graph(setup_random{0.1, 0.1, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 10%, addr 50%)", test_graph(setup_random{0.1, 0.5, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 10%, addr 90%)", test_graph(setup_random{0.1, 0.9, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 10%, addr 100%)", test_graph(setup_random{0.1, 1., 10})));
  benchs.insert(std::make_pair("random 100 addresses (edge 10%, addr 10%)", test_graph(setup_random{0.1, 0.1, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 10%, addr 50%)", test_graph(setup_random{0.1, 0.5, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 10%, addr 90%)", test_graph(setup_random{0.1, 0.9, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 10%, addr 100%)", test_graph(setup_random{0.1, 1.0, 100})));

  benchs.insert(std::make_pair("random 10 addresses (edge 50%, addr 10%)", test_graph(setup_random{0.5, 0.1, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 50%, addr 50%)", test_graph(setup_random{0.5, 0.5, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 50%, addr 90%)", test_graph(setup_random{0.5, 0.9, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 50%, addr 100%)", test_graph(setup_random{0.5, 1., 10})));
  benchs.insert(std::make_pair("random 100 addresses (edge 50%, addr 10%)", test_graph(setup_random{0.5, 0.1, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 50%, addr 50%)", test_graph(setup_random{0.5, 0.5, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 50%, addr 90%)", test_graph(setup_random{0.5, 0.9, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 50%, addr 100%)", test_graph(setup_random{0.5, 1.0, 100})));

  benchs.insert(std::make_pair("random 10 addresses (edge 90%, addr 10%)", test_graph(setup_random{0.9, 0.1, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 90%, addr 50%)", test_graph(setup_random{0.9, 0.5, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 90%, addr 90%)", test_graph(setup_random{0.9, 0.9, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 90%, addr 100%)", test_graph(setup_random{0.9, 1., 10})));
  benchs.insert(std::make_pair("random 100 addresses (edge 90%, addr 10%)", test_graph(setup_random{0.9, 0.1, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 90%, addr 50%)", test_graph(setup_random{0.9, 0.5, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 90%, addr 90%)", test_graph(setup_random{0.9, 0.9, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 90%, addr 100%)", test_graph(setup_random{0.9, 1.0, 100})));

  benchs.insert(std::make_pair("random 10 addresses (edge 100%, addr 10%)", test_graph(setup_random{1.0, 0.1, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 100%, addr 50%)", test_graph(setup_random{1.0, 0.5, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 100%, addr 90%)", test_graph(setup_random{1.0, 0.9, 10})));
  benchs.insert(std::make_pair("random 10 addresses (edge 100%, addr 100%)", test_graph(setup_random{1.0, 1., 10})));
  benchs.insert(std::make_pair("random 100 addresses (edge 100%, addr 10%)", test_graph(setup_random{1.0, 0.1, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 100%, addr 50%)", test_graph(setup_random{1.0, 0.5, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 100%, addr 90%)", test_graph(setup_random{1.0, 0.9, 100})));
  benchs.insert(std::make_pair("random 100 addresses (edge 100%, addr 100%)", test_graph(setup_random{1.0, 1.0, 100})));

  // The plots we want:
  // For each bench: comparison between dynamic, bfs and transitive closure, in the clean and dirty case. So 6 curves
  for(const auto& bench : benchs)
  {
    QFile f(bench.first.c_str());
    f.open(QIODevice::WriteOnly);
    QTextStream ts(&f);
    // NumNodes Dynamic StaticClean BfsDirty TClosDirty
    ts << "$N$"   << "\t"
       << "Dyn" << "\t"
       << "StaticClean" << "\t"
       << "BFSDirty" << "\t"
       << "TCDirty"
       << "\n";

    const auto& dyn_bench = bench.second.dynamic;
    const auto& sta_bench = bench.second.static_clean;
    const auto& bfs_bench = bench.second.bfs;
    const auto& tc_bench = bench.second.tc;
    for(int n : NUM_NODES)
    {
      ts << n << "\t";

      if(dyn_bench.find(n) != dyn_bench.end())
        ts << dyn_bench.at(n);
      ts << "\t";

      if(sta_bench.find(n) != sta_bench.end())
        ts << sta_bench.at(n);
      ts << "\t";

      if(bfs_bench.find(n) != bfs_bench.end())
        ts << bfs_bench.at(n);
      ts << "\t";

      if(tc_bench.find(n) != tc_bench.end())
        ts << tc_bench.at(n);
      ts << "\n";
    }
  }
}
