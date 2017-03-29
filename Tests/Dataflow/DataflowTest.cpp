#include <QtTest/QTest>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/base/address.hpp>
#include <Editor/TestUtils.hpp>

namespace ossia
{
class node_mock : public graph_node {

public:
  node_mock(inlets in, outlets out)
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


template<typename T>
auto pop_front(T& container)
{
  if(container.size() > 0)
  {
    auto val = container.front();
    container.pop_front();
    return val;
  }
  else
  {
    throw std::runtime_error("invalid pop");
  }
}

struct execution_mock
{
  const int factor = 1;
  std::shared_ptr<node_mock> node;
  void operator()(const execution_state& )
  {
    auto elt = pop_front(node->in_ports[0]->data.target<value_port>()->data);
    if(auto val = elt.target<std::vector<ossia::value>>())
    {
      auto v = *val;
      v.push_back(factor * (1+int(node->time)));
      node->out_ports[0]->data.target<value_port>()->data.push_back(std::move(v));
    }
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
    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>(*test.tuple_addr);
    n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>(*test.tuple_addr);
    n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    qDebug() << "N1:" << (void*) n1.get() << "N2: " << (void*) n2.get();

    g.add_node(n1);
    g.add_node(n2);
    g.connect(std::make_shared<graph_edge>(c, n1_out, n2_in, n1, n2));
  }
};

struct simple_implicit_graph
{
  ossia::graph g;
  std::shared_ptr<ossia::node_mock> n1, n2;
  simple_implicit_graph(ossia::TestUtils& test, ossia::connection c)
  {
    using namespace ossia;
    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>(*test.tuple_addr);
    n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>(*test.tuple_addr);
    n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    g.add_node(n1);
    g.add_node(n2);
  }
};


struct three_outputs_one_input_explicit_graph
{
  ossia::graph g;
  std::shared_ptr<ossia::node_mock> n1, n2, n3, nin;
  three_outputs_one_input_explicit_graph(ossia::TestUtils& test, ossia::connection c)
  {
    using namespace ossia;

    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>();
    n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>();
    n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    auto n3_in = make_inlet<value_port>(*test.tuple_addr);
    auto n3_out = make_outlet<value_port>();
    n3 = std::make_shared<node_mock>(inlets{n3_in}, outlets{n3_out});
    n3->fun = execution_mock{100, n3};

    auto nin_in = make_inlet<value_port>();
    auto nin_out = make_outlet<value_port>(*test.tuple_addr);
    nin = std::make_shared<node_mock>(inlets{nin_in}, outlets{nin_out});
    nin->fun = execution_mock{1000, nin};

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);
    g.add_node(nin);

    g.connect(std::make_shared<graph_edge>(c, n1_out, nin_in, n1, nin));
    g.connect(std::make_shared<graph_edge>(c, n2_out, nin_in, n2, nin));
    g.connect(std::make_shared<graph_edge>(c, n3_out, nin_in, n3, nin));

    g.connect(std::make_shared<graph_edge>(dependency_connection{}, outlet_ptr{}, inlet_ptr{}, n1, n2));
    g.connect(std::make_shared<graph_edge>(dependency_connection{}, outlet_ptr{}, inlet_ptr{}, n2, n3));
  }
};


struct three_serial_nodes_explicit_graph
{
  ossia::graph g;
  std::shared_ptr<ossia::node_mock> n1, n2, n3;
  three_serial_nodes_explicit_graph(ossia::TestUtils& test, ossia::connection c)
  {
    using namespace ossia;

    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>();
    n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>();
    auto n2_out = make_outlet<value_port>();
    n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    auto n3_in = make_inlet<value_port>();
    auto n3_out = make_outlet<value_port>(*test.tuple_addr);
    n3 = std::make_shared<node_mock>(inlets{n3_in}, outlets{n3_out});
    n3->fun = execution_mock{100, n3};

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);

    g.connect(std::make_shared<graph_edge>(c, n1_out, n2_in, n1, n2));
    g.connect(std::make_shared<graph_edge>(c, n2_out, n3_in, n2, n3));
  }
};


struct three_serial_nodes_implicit_graph
{
  ossia::graph g;
  std::shared_ptr<ossia::node_mock> n1, n2, n3;
  three_serial_nodes_implicit_graph(ossia::TestUtils& test, ossia::connection c)
  {
    using namespace ossia;

    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>(*test.tuple_addr);
    n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>(*test.tuple_addr);
    n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    auto n3_in = make_inlet<value_port>(*test.tuple_addr);
    auto n3_out = make_outlet<value_port>(*test.tuple_addr);
    n3 = std::make_shared<node_mock>(inlets{n3_in}, outlets{n3_out});
    n3->fun = execution_mock{100, n3};

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);
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

    auto n1 = std::make_shared<node_mock>(inlets{make_inlet<value_port>()}, outlets{make_outlet<value_port>()});
    auto n2 = std::make_shared<node_mock>(inlets{make_inlet<value_port>()}, outlets{make_outlet<value_port>()});
    auto n3 = std::make_shared<node_mock>(inlets{make_inlet<value_port>()}, outlets{make_outlet<value_port>()});
    auto n4 = std::make_shared<node_mock>(inlets{make_inlet<value_port>(), make_inlet<value_port>()}, outlets{make_outlet<value_port>(), make_outlet<value_port>()});
    auto n5 = std::make_shared<node_mock>(inlets{make_inlet<value_port>(), make_inlet<value_port>()}, outlets{make_outlet<value_port>(), make_outlet<value_port>()});

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);
    g.add_node(n4);
    g.add_node(n5);

    auto c1 = std::make_shared<graph_edge>(connection{immediate_glutton_connection{}}, n1->outputs()[0], n2->inputs()[0], n1, n2);
    auto c2 = std::make_shared<graph_edge>(connection{immediate_glutton_connection{}}, n1->outputs()[0], n3->inputs()[0], n1, n3);
    auto c3 = std::make_shared<graph_edge>(connection{immediate_glutton_connection{}}, n2->outputs()[0], n4->inputs()[0], n2, n4);
    auto c4 = std::make_shared<graph_edge>(connection{immediate_glutton_connection{}}, n3->outputs()[0], n4->inputs()[1], n3, n4);

    auto c5 = std::make_shared<graph_edge>(connection{immediate_glutton_connection{}}, n4->outputs()[0], n5->inputs()[0], n4, n5);
    auto c6 = std::make_shared<graph_edge>(connection{immediate_glutton_connection{}}, n4->outputs()[1], n5->inputs()[1], n4, n5);

    g.connect(c1);
    g.connect(c2);
    g.connect(c3);
    g.connect(c4);
    g.connect(c5);
    g.connect(c6);

    QVERIFY(n1->outputs()[0]->targets[0]);
    QCOMPARE(n1->outputs()[0]->targets[0]->in_node, std::shared_ptr<graph_node>(n2));
    QCOMPARE(n1->outputs()[0]->targets[0]->in, n2->inputs()[0]);
    QCOMPARE(n1->outputs()[0]->targets[0]->out_node, std::shared_ptr<graph_node>(n1));
    QCOMPARE(n1->outputs()[0]->targets[0]->out, n1->outputs()[0]);

    QVERIFY(n2->inputs()[0]->sources[0]);
    QCOMPARE(n2->inputs()[0]->sources[0]->in_node, std::shared_ptr<graph_node>(n2));
    QCOMPARE(n2->inputs()[0]->sources[0]->in, n2->inputs()[0]);
    QCOMPARE(n2->inputs()[0]->sources[0]->out_node, std::shared_ptr<graph_node>(n1));
    QCOMPARE(n2->inputs()[0]->sources[0]->out, n1->outputs()[0]);
  }


  void test_disable_strict_nodes()
  {
    using namespace ossia;
    graph g;

    auto n1 = std::make_shared<node_mock>(inlets{make_inlet<value_port>()}, outlets{make_outlet<value_port>()});
    auto n2 = std::make_shared<node_mock>(inlets{make_inlet<value_port>()}, outlets{make_outlet<value_port>()});
    auto n3 = std::make_shared<node_mock>(inlets{make_inlet<value_port>()}, outlets{make_outlet<value_port>()});
    auto n4 = std::make_shared<node_mock>(inlets{make_inlet<value_port>(), make_inlet<value_port>()}, outlets{make_outlet<value_port>()});

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);
    g.add_node(n4);

    g.connect(std::make_shared<graph_edge>(connection{immediate_strict_connection{}}, n1->outputs()[0], n2->inputs()[0], n1, n2));
    g.connect(std::make_shared<graph_edge>(connection{immediate_strict_connection{}}, n1->outputs()[0], n3->inputs()[0], n1, n3));
    g.connect(std::make_shared<graph_edge>(connection{immediate_strict_connection{}}, n2->outputs()[0], n4->inputs()[0], n2, n4));
    g.connect(std::make_shared<graph_edge>(connection{immediate_strict_connection{}}, n3->outputs()[0], n4->inputs()[1], n3, n4));

    {
      n1->set_enabled(true);
      n2->set_enabled(true);
      n3->set_enabled(true);
      n4->set_enabled(true);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      QVERIFY(res.empty());
    }

    {
      n1->set_enabled(false);
      n2->set_enabled(false);
      n3->set_enabled(false);
      n4->set_enabled(false);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      QVERIFY(res.empty());
    }

    {
      n1->set_enabled(true);
      n2->set_enabled(true);
      n3->set_enabled(true);
      n4->set_enabled(false);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      decltype(res) expected{n2, n3};
      QCOMPARE(res, expected);
    }

    {
      n1->set_enabled(true);
      n2->set_enabled(false);
      n3->set_enabled(false);
      n4->set_enabled(false);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      decltype(res) expected{n1};
      QCOMPARE(res, expected);
    }

    {
      n1->set_enabled(true);
      n2->set_enabled(true);
      n3->set_enabled(false);
      n4->set_enabled(false);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      decltype(res) expected{n1, n2};
      QCOMPARE(res, expected);
    }

    {
      n1->set_enabled(false);
      n2->set_enabled(true);
      n3->set_enabled(true);
      n4->set_enabled(true);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      decltype(res) expected{n2, n3};
      QCOMPARE(res, expected);
    }
    {
      n1->set_enabled(false);
      n2->set_enabled(false);
      n3->set_enabled(false);
      n4->set_enabled(true);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      decltype(res) expected{n4};
      QCOMPARE(res, expected);
    }
  }

  void strict_explicit_relationship_simple()
  {
    using namespace ossia;
    TestUtils test;

    // Functional dependency
    simple_explicit_graph g(test, immediate_strict_connection{});
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.disable(g.n2);
    g.n1->set_time(0);
    g.n2->set_time(0);

    g.g.state(); // nothing
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(1);
    g.n2->set_time(1);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 2, 10 * 2}));

    g.g.disable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(2);
    g.n2->set_time(2);

    g.g.state(); // nothing
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 2, 10 * 2}));
  }


  void strict_explicit_relationship_serial()
  {
    using namespace ossia;
    TestUtils test;

    // Functional dependency
    three_serial_nodes_explicit_graph g(test, immediate_strict_connection{});
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.enable(g.n2);
    g.g.enable(g.n3);
    g.n1->set_time(0);
    g.n2->set_time(0);
    g.n3->set_time(0);

    g.g.state(); // nothing
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1, 10 * 1, 100 * 1}));

  }
  void strict_implicit_relationship()
  {

  }

  void glutton_implicit_relationship()
  {
    using namespace ossia;
    TestUtils test;

    simple_implicit_graph g(test, immediate_glutton_connection{});
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.disable(g.n2);
    g.n1->set_time(0);
    g.n2->set_time(0);

    g.g.state(); // f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1}));

    g.g.enable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(1);
    g.n2->set_time(1);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2}));

    g.g.disable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(2);
    g.n2->set_time(2);

    g.g.state(); // f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2, 10 * 3}));

  }

  void glutton_explicit_relationship()
  {
    // Does it make sense ??
    // Cables : used to reduce "where" things go, so yeah, makes sense
    using namespace ossia;
    TestUtils test;

    simple_explicit_graph g(test, immediate_glutton_connection{});
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.disable(g.n2);
    g.n1->set_time(0);
    g.n1->set_time(0);

    g.g.state(); // f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1}));

    g.g.enable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(1);
    g.n2->set_time(1);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2}));

    g.g.disable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(2);
    g.n2->set_time(2);

    g.g.state(); // f2
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2, 10 * 3}));

  }

  void delayed_relationship()
  {
    using namespace ossia;
    TestUtils test;

    simple_explicit_graph g(test, delayed_strict_connection{});
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.disable(g.n2);
    g.n1->set_time(0);

    // f1 pushes 1 * 1 in its queue
    g.g.state();
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(1);
    g.n2->set_time(0);

    // f1(0) = 1
    // f1(1) = 2
    // f2(f1(0), 0) = [1, 10]
    g.g.state();
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 1, 10 * 1}));

    g.g.disable(g.n1);
    g.g.enable(g.n2);
    g.n1->set_time(2);
    g.n2->set_time(1);

    // f2(f1(1), 1) = [2, 20]
    g.g.state(); // f2 o f1(t-1)
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{1 * 2, 10 * 2}));
    // 10 * 1 is not here because we start from {1 * 1} from the point of view of f1(t-1)

  }

  void reduced_implicit_relationship()
  {

  }

  void reduced_explicit_relationship()
  {
    using namespace ossia;
    TestUtils test;

    // Two ways to envision reduction : reduce at the input of an outlet, or reduce the execution of all nodes that did take
    // the same input at this tick. but would not always make sense.
    three_outputs_one_input_explicit_graph g(test, reduction_connection{});
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.enable(g.n2);
    g.g.enable(g.n3);
    g.g.enable(g.nin);

    g.n1->set_time(0);
    g.n2->set_time(0);
    g.n3->set_time(0);
    g.nin->set_time(0);

    g.g.state();

    // The reduction operation for values just take the last.

  }

  void replaced_implicit_relationship()
  {
    using namespace ossia;
    TestUtils test;

    three_outputs_one_input_explicit_graph g(test, replacing_connection{});
    QCOMPARE(test.tuple_addr->cloneValue(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(g.n1);
    g.g.enable(g.n2);
    g.g.enable(g.n3);
    g.g.enable(g.nin);

    g.n1->set_time(0);
    g.n2->set_time(0);
    g.n3->set_time(0);
    g.nin->set_time(0);

    g.g.state();

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
