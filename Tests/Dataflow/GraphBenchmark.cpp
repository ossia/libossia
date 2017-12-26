// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/dataflow/graph.hpp>
#include "../Editor/TestUtils.hpp"

using namespace ossia;

template<typename Port_T>
class node_empty_mock final : public graph_node {
public:
  node_empty_mock()
  {
    m_inlets.push_back(ossia::make_inlet<Port_T>());
    m_inlets.push_back(ossia::make_outlet<Port_T>());
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
  Q_OBJECT
private Q_SLOTS:
  void test_graph_serial_connected()
  {

    for(int num_nodes : {1, 5, 10, 50, 100, 250, 500, 1000}) {
      ossia::graph g;

      std::shared_ptr<ossia::graph_node> prev{};
      for(int i = 0; i < num_nodes; i++)
      {
        auto n = std::make_shared<value_mock>();
        if(prev)
        {
          g.add_edge(ossia::make_edge(ossia::immediate_strict_connection{}
                                      , prev->outputs()[0], n->inputs()[0]
                                      , prev, n));

        }
        prev = n;
        g.add_node(std::move(n));
      }
    }

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

