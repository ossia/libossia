#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/graph_parallel.hpp>
#include <ossia/dataflow/graph_edge_helpers.hpp>
#include <ossia/dataflow/graph_node.hpp>

#include <random>
#define NUM_TAKES 10000
#define NUM_NODES 2500
#define EDGE_RATIO 0.1
using namespace ossia;

static std::mt19937 mt;
std::atomic<int> sum = 0;
class node_empty_mock final : public graph_node
{
public:
  std::string lbl{};
  std::string label() const noexcept override { return lbl; }
  node_empty_mock()
  {
    m_inlets.push_back(new ossia::value_inlet);
    m_outlets.push_back(new ossia::value_outlet);
  }

  void run(const token_request& t, exec_state_facade e) noexcept override
  {
    // sum++;
  }
};

template <typename T>
void setup_random_edges(const std::vector<std::shared_ptr<node_empty_mock>>& nodes, T& g)
{
  for(std::size_t i = 0; i < nodes.size(); i++)
  {
    for(std::size_t j = i + 1; j < nodes.size(); j++)
    {
      if(std::uniform_real_distribution<double>{0., 1.}(mt))
      {
        auto edge = g.allocate_edge(
            ossia::immediate_strict_connection{}, nodes[i]->root_outputs()[0],
            nodes[j]->root_inputs()[0], nodes[i], nodes[j]);
        g.connect(edge);
      }
    }
  }
}

struct setup_random
{
  template <typename T>
  auto operator()(int num_nodes, T& g) const
  {
    std::vector<std::shared_ptr<node_empty_mock>> nodes;

    for(int i = 0; i < num_nodes; i++)
    {
      nodes.push_back(std::make_shared<node_empty_mock>());
      g.add_node(nodes.back());
    }

    setup_random_edges(nodes, g);

    return nodes;
  }
};

struct setup_dawlike
{
  int chain_count = 100;
  template <typename T>
  auto operator()(int num_nodes, T& g) const
  {
    using chain = std::vector<std::shared_ptr<node_empty_mock>>;
    std::vector<std::shared_ptr<node_empty_mock>> nodes;
    std::vector<chain> chains;
    chains.resize(chain_count);

    for(int c = 0; c < chain_count; c++)
    {
      auto& chain = chains[c];

      for(int i = 0; i < num_nodes / chain_count; i++)
      {
        auto new_node = std::make_shared<node_empty_mock>();
        nodes.push_back(new_node);
        g.add_node(nodes.back());
        std::shared_ptr<node_empty_mock> prev_node;
        if(!chain.empty())
          prev_node = chain.back();
        chain.push_back(nodes.back());
        if(prev_node)
        {
          auto edge = g.allocate_edge(
              ossia::immediate_strict_connection{}, prev_node->root_outputs()[0],
              new_node->root_inputs()[0], prev_node, new_node);
          g.connect(edge);
        }
      }
    }

    // Simulate two levels of grouping
    std::vector<std::shared_ptr<node_empty_mock>> groups_1;
    int j = 0;
    for(int i = 0; i < chain_count / 4; i++)
    {

      auto new_node = std::make_shared<node_empty_mock>();
      nodes.push_back(new_node);
      groups_1.push_back(new_node);
      g.add_node(nodes.back());

      for(int end = j + 4; j < end; j++)
      {
        auto& prev_node = chains[j].back();
        auto edge = g.allocate_edge(
            ossia::immediate_strict_connection{}, prev_node->root_outputs()[0],
            new_node->root_inputs()[0], prev_node, new_node);
        g.connect(edge);
      }
    }

    {
      auto new_node = std::make_shared<node_empty_mock>();
      nodes.push_back(new_node);
      g.add_node(nodes.back());

      for(auto& prev_node : groups_1)
      {
        auto edge = g.allocate_edge(
            ossia::immediate_strict_connection{}, prev_node->root_outputs()[0],
            new_node->root_inputs()[0], prev_node, new_node);
        g.connect(edge);
      }
    }

    return nodes;
  }
};

struct measure_clean_tick
{
  template <typename T, typename U>
  auto operator()(T& g, const U& nodes)
  {
    ossia::execution_state e;

    // ensure that a tick happens to make it clean
    g.state(e);

    // measure
    for(int i = 0; i < NUM_TAKES; i++)
    {
      for(auto& node : nodes)
        node->request({});
      g.state(e);
    }
  }
};

int main()
{
#if 0
  mt.seed(12345678);
  ossia::graph_setup_options opt;
  opt.parallel = true;
  auto graph = new cpptf_tc_graph;

  std::cerr << "Graph\n";
  auto nodes = setup_dawlike{}(NUM_NODES, *graph);

  std::cerr << "Setup\n";

  auto t0 = std::chrono::high_resolution_clock::now();

  measure_clean_tick{}(*graph, nodes);

  auto t1 = std::chrono::high_resolution_clock::now();
  auto this_count
      = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()
        / NUM_TAKES;

  std::cerr << "Done\n" << this_count << " => " << sum << std::endl;
  delete graph;
#endif
}
