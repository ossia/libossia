// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest/QTest>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/base/parameter.hpp>
#include <Network/TestUtils.hpp>

namespace ossia
{
class node_mock : public graph_node {

public:
  node_mock(inlets in, outlets out)
  {
    m_inlets = std::move(in);
    m_outlets = std::move(out);
  }

  std::function<void(execution_state& e)> fun;
  void run(execution_state& e) override
  {
    if(fun)
      fun(e);
  }
};


template<typename T>
auto pop_front(std::vector<T>& vec)
{
  if(!vec.empty())
  {
    auto val = vec.front();
    vec.erase(vec.begin());
    return val;
  }
  else
  {
    throw std::runtime_error("invalid pop");
  }
}

template<typename T, std::size_t N>
auto pop_front(chobo::small_vector<T, N>& vec)
{
  if(!vec.empty())
  {
    auto val = vec.front();
    vec.erase(vec.begin());
    return val;
  }
  else
  {
    throw std::runtime_error("invalid pop");
  }
}

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


struct debug_mock
{
  debug_mock(int f, std::weak_ptr<node_mock> p): factor{f}, node{p} { }
  const int factor = 1;
  std::weak_ptr<node_mock> node;
  void operator()(const execution_state& )
  {
    if(auto n = node.lock())
    {
      qDebug() << factor << n->time();
      messages.emplace_back(factor, n->time());
    }
  }
  static std::vector<std::pair<int, int>> messages;
};
std::vector<std::pair<int, int>> debug_mock::messages;
struct execution_mock
{
  execution_mock(int f, std::weak_ptr<node_mock> p): factor{f}, node{p} { }
  const int factor = 1;
  std::weak_ptr<node_mock> node;
  void operator()(const execution_state& )
  {
    if(auto n = node.lock())
    {
      auto elt = pop_front(n->inputs()[0]->data.target<value_port>()->data);
      if(auto val = elt.target<std::vector<ossia::value>>())
      {
        auto v = *val;
        v.push_back(factor * (1+int(n->time())));
        n->outputs()[0]->data.target<value_port>()->data.push_back(std::move(v));
      }
    }
  }
};

}

struct simple_explicit_graph
{
  ossia::graph g;
  ossia::node_mock* n1, *n2;
  simple_explicit_graph(ossia::TestDevice& test, ossia::connection c)
  {
    using namespace ossia;
    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>(*test.tuple_addr);
    auto n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>(*test.tuple_addr);
    auto n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    qDebug() << "N1:" << (void*) n1.get() << "N2: " << (void*) n2.get();

    g.add_node(n1);
    g.add_node(n2);
    g.connect(make_edge(c, n1_out, n2_in, n1, n2));

    this->n1 = n1.get();
    this->n2 = n2.get();
  }
};

struct simple_implicit_graph
{
  ossia::graph g;
  ossia::node_mock* n1, *n2;
  simple_implicit_graph(ossia::TestDevice& test, ossia::connection c)
  {
    using namespace ossia;
    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>(*test.tuple_addr);
    auto n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>(*test.tuple_addr);
    auto n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    g.add_node(n1);
    g.add_node(n2);

    this->n1 = n1.get();
    this->n2 = n2.get();
  }
};

struct no_parameter_explicit_graph
{
  ossia::graph g;
  ossia::node_mock* n1, *n2;
  no_parameter_explicit_graph(ossia::TestDevice& test, ossia::connection c)
  {
    using namespace ossia;
    auto n1_out = make_outlet<value_port>();
    auto n1 = std::make_shared<node_mock>(inlets{}, outlets{n1_out});
    n1->fun = debug_mock{1, n1};

    auto n2_in = make_inlet<value_port>();
    auto n2_out = make_outlet<value_port>();
    auto n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = debug_mock{10, n2};

    g.add_node(n1);
    g.add_node(n2);
    g.connect(make_edge(c, n1_out, n2_in, n1, n2));

    this->n1 = n1.get();
    this->n2 = n2.get();
  }
};

struct three_outputs_one_input_explicit_graph
{
  ossia::graph g;
  ossia::node_mock* n1, *n2, *n3, *nin;
  three_outputs_one_input_explicit_graph(ossia::TestDevice& test, ossia::connection c)
  {
    using namespace ossia;

    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>();
    auto n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>();
    auto n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    auto n3_in = make_inlet<value_port>(*test.tuple_addr);
    auto n3_out = make_outlet<value_port>();
    auto n3 = std::make_shared<node_mock>(inlets{n3_in}, outlets{n3_out});
    n3->fun = execution_mock{100, n3};

    auto nin_in = make_inlet<value_port>();
    auto nin_out = make_outlet<value_port>(*test.tuple_addr);
    auto nin = std::make_shared<node_mock>(inlets{nin_in}, outlets{nin_out});
    nin->fun = execution_mock{1000, nin};

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);
    g.add_node(nin);

    g.connect(make_edge(c, n1_out, nin_in, n1, nin));
    g.connect(make_edge(c, n2_out, nin_in, n2, nin));
    g.connect(make_edge(c, n3_out, nin_in, n3, nin));

    g.connect(make_edge(dependency_connection{}, outlet_ptr{}, inlet_ptr{}, n1, n2));
    g.connect(make_edge(dependency_connection{}, outlet_ptr{}, inlet_ptr{}, n2, n3));

    this->n1 = n1.get();
    this->n2 = n2.get();
    this->n3 = n3.get();
    this->nin = nin.get();
  }
};


struct three_serial_nodes_explicit_graph
{
  ossia::graph g;
  ossia::node_mock* n1, *n2, *n3;
  three_serial_nodes_explicit_graph(ossia::TestDevice& test, ossia::connection c)
  {
    using namespace ossia;

    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>();
    auto n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>();
    auto n2_out = make_outlet<value_port>();
    auto n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    auto n3_in = make_inlet<value_port>();
    auto n3_out = make_outlet<value_port>(*test.tuple_addr);
    auto n3 = std::make_shared<node_mock>(inlets{n3_in}, outlets{n3_out});
    n3->fun = execution_mock{100, n3};

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);

    g.connect(make_edge(c, n1_out, n2_in, n1, n2));
    g.connect(make_edge(c, n2_out, n3_in, n2, n3));

    this->n1 = n1.get();
    this->n2 = n2.get();
    this->n3 = n3.get();
  }
};


struct three_serial_nodes_implicit_graph
{
  ossia::graph g;
  ossia::node_mock* n1, *n2, *n3;
  three_serial_nodes_implicit_graph(ossia::TestDevice& test, ossia::connection c)
  {
    using namespace ossia;

    auto n1_in = make_inlet<value_port>(*test.tuple_addr);
    auto n1_out = make_outlet<value_port>(*test.tuple_addr);
    auto n1 = std::make_shared<node_mock>(inlets{n1_in}, outlets{n1_out});
    n1->fun = execution_mock{1, n1};

    auto n2_in = make_inlet<value_port>(*test.tuple_addr);
    auto n2_out = make_outlet<value_port>(*test.tuple_addr);
    auto n2 = std::make_shared<node_mock>(inlets{n2_in}, outlets{n2_out});
    n2->fun = execution_mock{10, n2};

    auto n3_in = make_inlet<value_port>(*test.tuple_addr);
    auto n3_out = make_outlet<value_port>(*test.tuple_addr);
    auto n3 = std::make_shared<node_mock>(inlets{n3_in}, outlets{n3_out});
    n3->fun = execution_mock{100, n3};

    g.add_node(n1);
    g.add_node(n2);
    g.add_node(n3);

    this->n1 = n1.get();
    this->n2 = n2.get();
    this->n3 = n3.get();
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

    auto c1 = make_edge(connection{immediate_glutton_connection{}}, n1->outputs()[0], n2->inputs()[0], n1, n2);
    auto c2 = make_edge(connection{immediate_glutton_connection{}}, n1->outputs()[0], n3->inputs()[0], n1, n3);
    auto c3 = make_edge(connection{immediate_glutton_connection{}}, n2->outputs()[0], n4->inputs()[0], n2, n4);
    auto c4 = make_edge(connection{immediate_glutton_connection{}}, n3->outputs()[0], n4->inputs()[1], n3, n4);

    auto c5 = make_edge(connection{immediate_glutton_connection{}}, n4->outputs()[0], n5->inputs()[0], n4, n5);
    auto c6 = make_edge(connection{immediate_glutton_connection{}}, n4->outputs()[1], n5->inputs()[1], n4, n5);

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

    g.connect(make_edge(connection{immediate_strict_connection{}}, n1->outputs()[0], n2->inputs()[0], n1, n2));
    g.connect(make_edge(connection{immediate_strict_connection{}}, n1->outputs()[0], n3->inputs()[0], n1, n3));
    g.connect(make_edge(connection{immediate_strict_connection{}}, n2->outputs()[0], n4->inputs()[0], n2, n4));
    g.connect(make_edge(connection{immediate_strict_connection{}}, n3->outputs()[0], n4->inputs()[1], n3, n4));

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
      set<graph_node*> expected{n2.get(), n3.get()};
      QCOMPARE(res, expected);
    }

    {
      n1->set_enabled(true);
      n2->set_enabled(false);
      n3->set_enabled(false);
      n4->set_enabled(false);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      set<graph_node*> expected{n1.get()};
      QCOMPARE(res, expected);
    }

    {
      n1->set_enabled(true);
      n2->set_enabled(true);
      n3->set_enabled(false);
      n4->set_enabled(false);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      set<graph_node*> expected{n1.get(), n2.get()};
      QCOMPARE(res, expected);
    }

    {
      n1->set_enabled(false);
      n2->set_enabled(true);
      n3->set_enabled(true);
      n4->set_enabled(true);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      decltype(res) expected{n2.get(), n3.get()};
      QCOMPARE(res, expected);
    }
    {
      n1->set_enabled(false);
      n2->set_enabled(false);
      n3->set_enabled(false);
      n4->set_enabled(true);
      auto res = g.disable_strict_nodes({n1, n2, n3, n4});
      decltype(res) expected{n4.get()};
      QCOMPARE(res, expected);
    }
  }

  void strict_explicit_relationship_simple()
  {
    using namespace ossia;
    TestDevice test;

    // Functional dependency
    simple_explicit_graph g(test, immediate_strict_connection{});
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.disable(*g.n2);
    g.n1->set_date(0, 0);
    g.n2->set_date(0, 0);

    g.g.state(); // nothing
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(1, 0.5);
    g.n2->set_date(1, 0.5);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 2, 10 * 2}));

    g.g.disable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(2, 1);
    g.n2->set_date(2, 1);

    g.g.state(); // nothing
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 2, 10 * 2}));
  }


  void strict_explicit_relationship_serial()
  {
    using namespace ossia;
    TestDevice test;

    // Functional dependency
    three_serial_nodes_explicit_graph g(test, immediate_strict_connection{});
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.g.enable(*g.n3);
    g.n1->set_date(0, 0);
    g.n2->set_date(0, 0);
    g.n3->set_date(0, 0);

    g.g.state(); // nothing
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 10 * 1, 100 * 1}));

  }
  void strict_implicit_relationship()
  {

  }

  void glutton_implicit_relationship()
  {
    using namespace ossia;
    TestDevice test;

    simple_implicit_graph g(test, immediate_glutton_connection{});
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.disable(*g.n2);
    g.n1->set_date(0, 0);
    g.n2->set_date(0, 0);

    g.g.state(); // f1
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(1, 0.5);
    g.n2->set_date(1, 0.5);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2}));

    g.g.disable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(2, 1);
    g.n2->set_date(2, 1);

    g.g.state(); // f2
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2, 10 * 3}));

  }

  void glutton_explicit_relationship()
  {
    // Does it make sense ??
    // Cables : used to reduce "where" things go, so yeah, makes sense
    using namespace ossia;
    TestDevice test;

    simple_explicit_graph g(test, immediate_glutton_connection{});
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.disable(*g.n2);
    g.n1->set_date(0, 0);
    g.n1->set_date(0, 0);

    g.g.state(); // f1
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(1, 0.5);
    g.n2->set_date(1, 0.5);

    g.g.state(); // f2 o f1
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2}));

    g.g.disable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(2, 1);
    g.n2->set_date(2, 1);

    g.g.state(); // f2
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2, 10 * 3}));

  }


  void glutton_explicit_relationship_2()
  {
    // Does it make sense ??
    // Cables : used to reduce "where" things go, so yeah, makes sense
    using namespace ossia;
    TestDevice test;

    no_parameter_explicit_graph g(test, immediate_glutton_connection{});
    debug_mock::messages.clear();

    g.g.enable(*g.n1);
    g.g.disable(*g.n2);
    g.n1->set_date(0, 0);
    g.n1->set_date(0, 0);

    qDebug("Start state");
    g.g.state(); // f1
    qDebug("End state");
    QVERIFY((debug_mock::messages == std::vector<std::pair<int, int>>{{1, 0}}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(1, 0.5);
    g.n2->set_date(1, 0.5);

    qDebug("Start state");
    debug_mock::messages.clear();
    g.g.state(); // f2 o f1
    QVERIFY((debug_mock::messages == std::vector<std::pair<int, int>>{{1, 1}, {10, 1}}));
    qDebug("End state");
    g.g.disable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(2, 1);
    g.n2->set_date(2, 1);

    qDebug("Start state");
    debug_mock::messages.clear();
    g.g.state(); // f2
    QVERIFY((debug_mock::messages == std::vector<std::pair<int, int>>{{10, 2}}));
    qDebug("End state");

  }

  void delayed_relationship()
  {
    using namespace ossia;
    TestDevice test;

    simple_explicit_graph g(test, delayed_strict_connection{});
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.disable(*g.n2);
    g.n1->set_date(0, 0);

    // f1 pushes 1 * 1 in its queue
    g.g.state();
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(1, 0.5);
    g.n2->set_date(0, 0);

    // f1(0) = 1
    // f1(1) = 2
    // f2(f1(0), 0) = [1, 10]
    g.g.state();
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 10 * 1}));

    g.g.disable(*g.n1);
    g.g.enable(*g.n2);
    g.n1->set_date(2, 1);
    g.n2->set_date(1, 1);

    // f2(f1(1), 1) = [2, 20]
    g.g.state(); // f2 o f1(t-1)
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 2, 10 * 2}));
    // 10 * 1 is not here because we start from {1 * 1} from the point of view of f1(t-1)

  }

  void reduced_implicit_relationship()
  {

  }

  void reduced_explicit_relationship()
  {
    /*
    using namespace ossia;
    TestUtils test;

    // Two ways to envision reduction : reduce at the input of an outlet, or reduce the execution of all nodes that did take
    // the same input at this tick. but would not always make sense.
    three_outputs_one_input_explicit_graph g(test, reduction_connection{});
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.g.enable(*g.n3);
    g.g.enable(*g.nin);

    g.n1->set_time(0);
    g.n2->set_time(0);
    g.n3->set_time(0);
    g.nin->set_time(0);

    g.g.state();

    // The reduction operation for values just take the last.
    */

  }

  void replaced_implicit_relationship()
  {
    /*
    using namespace ossia;
    TestUtils test;

    three_outputs_one_input_explicit_graph g(test, replacing_connection{});
    QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

    g.g.enable(*g.n1);
    g.g.enable(*g.n2);
    g.g.enable(*g.n3);
    g.g.enable(*g.nin);

    g.n1->set_time(0);
    g.n2->set_time(0);
    g.n3->set_time(0);
    g.nin->set_time(0);

    g.g.state();
    */
  }

  void replaced_explicit_relationship()
  {

  }


  void temporal_ordering()
  {
    // what if both happen at exact same time.
    // orders should superimperpose themselves so that there is always
    // a "default" ordering (this could be the hierarchical one)
    using namespace ossia;

    {
      TestDevice test;
      simple_implicit_graph g(test, immediate_glutton_connection{});
      g.g.m_ordering = node_ordering::temporal;
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

      g.g.enable(*g.n1);
      g.g.disable(*g.n2);
      g.n1->set_date(0, 0);
      g.n1->temporal_priority = {0};
      g.n2->set_date(0, 0);

      g.g.state(); // f1
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1}));

      g.g.enable(*g.n1);
      g.g.enable(*g.n2);
      g.n1->set_date(1, 0.5);
      g.n2->set_date(1, 0.5);
      g.n2->temporal_priority = {1};

      g.g.state(); // f2 o f1
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2}));

      g.g.enable(*g.n1);
      g.g.enable(*g.n2);
      g.n1->set_date(2, 1);
      g.n2->set_date(2, 1);

      g.g.state(); // f2 o f1
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 1 * 2, 10 * 2, 1 * 3, 10 * 3}));
    }

    {
      TestDevice test;
      simple_implicit_graph g(test, immediate_glutton_connection{});
      g.g.m_ordering = node_ordering::temporal;
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{}));

      g.g.enable(*g.n1);
      g.g.disable(*g.n2);
      g.n1->set_date(0, 0);
      g.n1->temporal_priority = {1};
      g.n2->set_date(0, 0);

      g.g.state(); // f1
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1}));

      g.g.enable(*g.n1);
      g.g.enable(*g.n2);
      g.n1->set_date(1, 0.5);
      g.n2->set_date(1, 0.5);
      g.n2->temporal_priority = {0};

      g.g.state(); // f1 o f2
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 10 * 2, 1 * 2}));

      g.g.enable(*g.n1);
      g.g.enable(*g.n2);
      g.n1->set_date(2, 1);
      g.n2->set_date(2, 1);

      g.g.state(); // f1 o f2
      QCOMPARE(test.tuple_addr->value(), ossia::value(std::vector<ossia::value>{1 * 1, 10 * 2, 1 * 2, 10 * 3, 1 * 3}));
    }

  }

  void hierarchical_ordering()
  {

  }

};

QTEST_APPLESS_MAIN(DataflowTest)

#include "DataflowTest.moc"
