#include <QtTest>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/base/address.hpp>
#include <Editor/TestUtils.hpp>

namespace ossia
{
class node_mock : public graph_node {

public:
  node_mock(ports in, ports out)
  {
    in_ports = std::move(in);
    out_ports = std::move(out);
  }

  std::function<void(execution_state& e)> fun;
  void run(execution_state& e) override
  {
    if(fun)
      fun(e);
  }
};
}


struct simple_explicit_graph
{
  ossia::graph g;
  std::shared_ptr<ossia::node_mock> n1, n2;
  simple_explicit_graph(ossia::TestUtils& test, ossia::connection c)
  {
    using namespace ossia;
    auto n1_in = make_port<value_port>(*test.tuple_addr);
    auto n1_out = make_port<value_port>();
    n1 = std::make_shared<node_mock>(ports{n1_in}, ports{n1_out});
    n1->fun = [&] (const execution_state& state) {
      auto val = n1->in_ports[0]->data.target<value_port>()->val.target<std::vector<ossia::value>>();
      if(val)
      {
        auto v = *val;
        v.push_back(1);
        n1->out_ports[0]->data.target<value_port>()->val = std::move(v);
      }
    };

    auto n2_in = make_port<value_port>();
    auto n2_out = make_port<value_port>(*test.tuple_addr);
    n2 = std::make_shared<node_mock>(ports{n2_in}, ports{n2_out});
    n1->fun = [&] (const execution_state& state) {
      auto val = n1->in_ports[0]->data.target<value_port>()->val.target<std::vector<ossia::value>>();
      if(val)
      {
        auto v = *val;
        v.push_back(2);
        n1->out_ports[0]->data.target<value_port>()->val = std::move(v);
      }
    };

    g.add_node(n1);
    g.add_node(n2);
    g.connect(std::make_shared<graph_edge>(c, n1_out, n2_in, n1, n2));
  }
};

class DataflowTest : public QObject
{
  Q_OBJECT

private slots:
  void test_mock()
  {
    using namespace ossia;
    graph g;

    auto n1 = std::make_shared<node_mock>(ports{make_port<value_port>()}, ports{make_port<value_port>()});
    auto n2 = std::make_shared<node_mock>(ports{make_port<value_port>()}, ports{make_port<value_port>()});
    auto n3 = std::make_shared<node_mock>(ports{make_port<value_port>()}, ports{make_port<value_port>()});
    auto n4 = std::make_shared<node_mock>(ports{make_port<value_port>(), make_port<value_port>()}, ports{make_port<value_port>()});

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);
    g.add_node(n4);

    auto c1 = std::make_shared<graph_edge>(connection{glutton_connection{}}, n1->outputs()[0], n2->inputs()[0], n1, n2);
    auto c2 = std::make_shared<graph_edge>(connection{glutton_connection{}}, n1->outputs()[0], n3->inputs()[0], n1, n3);
    auto c3 = std::make_shared<graph_edge>(connection{glutton_connection{}}, n2->outputs()[0], n4->inputs()[0], n2, n4);
    auto c4 = std::make_shared<graph_edge>(connection{glutton_connection{}}, n3->outputs()[0], n4->inputs()[1], n3, n4);

    g.connect(c1);
    g.connect(c2);
    g.connect(c3);
    g.connect(c4);
  }


  void strict_explicit_relationship()
  {
    using namespace ossia;
    TestUtils test;

    // Functional dependency
    simple_explicit_graph g(test, strict_connection{});
    // Execution : f1, f2 o f1, f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.disable(g.n2);
    g.n1->set_time(0);
    g.n2->set_time(0);

    g.g.state(); // f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n2);
    g.n1->set_time(1);
    g.n2->set_time(1);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1, 2}));

    g.g.disable(g.n1);
    g.n1->set_time(2);
    g.n2->set_time(2);

    g.g.state(); // f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1, 2}));
  }

  void strict_implicit_relationship()
  {

  }

  void glutton_implicit_relationship()
  {

  }

  void glutton_explicit_relationship()
  {
    using namespace ossia;
    TestUtils test;

    // Functional dependency
    simple_explicit_graph g(test, glutton_connection{});
    // Execution : f1, f2 o f1, f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.disable(g.n2);
    g.n1->set_time(0);
    g.n2->set_time(0);

    g.g.state(); // f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1}));

    g.g.enable(g.n2);
    g.n1->set_time(1);
    g.n2->set_time(1);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1, 1, 2}));

    g.g.disable(g.n1);
    g.n1->set_time(2);
    g.n2->set_time(2);

    g.g.state(); // f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1, 1, 2, 2}));

  }

  void delayed_relationship()
  {
    using namespace ossia;
    TestUtils test;

    // Functional dependency
    simple_explicit_graph g(test, glutton_connection{});
    // Execution : f1, f2 o f1, f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.disable(g.n2);
    g.n1->set_time(0);
    g.n2->set_time(0);

    g.g.state(); // f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n2);
    g.n1->set_time(1);
    g.n2->set_time(1);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1, 1, 2}));

    g.g.disable(g.n1);
    g.n1->set_time(2);
    g.n2->set_time(2);

    g.g.state(); // f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1, 1, 2, 2}));

  }

  void reduced_implicit_relationship()
  {

  }

  void reduced_explicit_relationship()
  {

  }

  void replaced_implicit_relationship()
  {

  }

  void replaced_explicit_relationship()
  {

  }


  void temporal_ordering()
  {
    // what if both happen at exact same time.
    // orders should superimperpose themselves so that there is always
    // a "default" ordering (this could be the hierarchical one)

  }

  void hierarchical_ordering()
  {

  }
};

QTEST_APPLESS_MAIN(DataflowTest)

#include "DataflowTest.moc"
