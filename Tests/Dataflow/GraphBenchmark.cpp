// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/dataflow/graph.hpp>
#include "../Editor/TestUtils.hpp"
#include <valgrind/callgrind.h>
using namespace ossia;

template<typename Port_T>
class node_empty_mock final : public graph_node {
public:
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


class GraphBenchmark : public QObject
{
  static const constexpr int NUM_TAKES = 500;
  Q_OBJECT
private Q_SLOTS:
  void test_graph_serial_connected()
  {

    for(int num_nodes : {1, 5, 10, 50, 100, 250, 500, 1000}) {
      ossia::graph_static_base<ossia::static_graph_policy::bfs> g{};
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


      ossia::execution_state e;
      g.state(e);
      int64_t count = 0;
      for(int i = 0; i < NUM_TAKES; i++)
      {
        for(auto& node : nodes)
          node->requested_tokens.push_back({});

        auto t0 = std::chrono::high_resolution_clock::now();
        CALLGRIND_START_INSTRUMENTATION;
        g.state(e);
        CALLGRIND_STOP_INSTRUMENTATION;
        auto t1 = std::chrono::high_resolution_clock::now();
        count += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
      }
      count = count / NUM_TAKES;
      std::cerr << num_nodes << " : " << count << "\n";
    }
    CALLGRIND_DUMP_STATS;

  }

  void test_graph_serial_address()
  {

  }


  void test_graph_parallel_1node_connected()
  {

  }

  void test_graph_parallel_1node_address()
  {

  }

  void test_graph_parallel_connected()
  {

  }

  void test_graph_parallel_address()
  {

  }

  void test_graph_random_connected()
  {

  }
  void test_graph_random_address()
  {

  }


  void test_graph_random()
  {

  }


};


QTEST_APPLESS_MAIN(GraphBenchmark)

#include "GraphBenchmark.moc"

