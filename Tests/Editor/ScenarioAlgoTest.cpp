// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>

#include <ossia/dataflow/node_process.hpp>

#include "TestUtils.hpp"

namespace ossia
{
class message_node : public ossia::graph_node
{
  public:
    message_node()
    {

    }

    void run(ossia::token_request, ossia::execution_state& e) override
    {
      for(auto& msg : data)
      {
        e.insert(&msg.dest.address(), ossia::tvalue{ msg.message_value, msg.dest.index, msg.dest.unit });
      }
    }

    std::vector<ossia::message> data;
};

class percentage_node : public ossia::graph_node
{
  public:
    percentage_node(ossia::destination d)
    {
      outputs().push_back(ossia::make_outlet<ossia::value_port>(&d.address()));
    }

    void run(ossia::token_request tok, ossia::execution_state& e) override
    {
      outputs().back()->data.target<ossia::value_port>()->add_value(ossia::tvalue{(float)tok.position});
    }
};
}

namespace ossia {
}

class ScenarioAlgoTest : public QObject
{
  Q_OBJECT

    auto create_event(ossia::scenario& s)
    {
      auto en = std::make_shared<ossia::time_sync>();
      en->set_expression(ossia::expressions::make_expression_true());
      auto ee = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *en, ossia::expressions::make_expression_true());
      en->insert(en->get_time_events().end(), ee);
      s.add_time_sync(std::move(en));
      return ee;
    }

    auto start_event(ossia::scenario& s)
    {
      auto sn = s.get_start_time_sync();
      return *sn->get_time_events().begin();
    }

  private slots:
    void test_exec_simple()
    {
      using namespace ossia;
      root_scenario s;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> se = start_event(scenario);
      std::shared_ptr<time_event> ee = create_event(scenario);

      std::shared_ptr<time_interval> c = time_interval::create([] (auto&&...) {}, *se, *ee, 3000_tv, 3000_tv, 3000_tv);
      s.scenario->add_time_interval(c);

      s.interval->start_and_tick();
      QVERIFY(se->get_status() == time_event::status::HAPPENED);
      QCOMPARE(c->get_date(), 0_tv);

      s.interval->tick(1000_tv);
      QCOMPARE(c->get_date(), 1000_tv);
      s.interval->tick(1000_tv);
      QCOMPARE(c->get_date(), 2000_tv);
      s.interval->tick(999_tv);
      QCOMPARE(c->get_date(), 2999_tv);
      s.interval->tick(1_tv); // The interval is stopped

      QCOMPARE(c->get_date(), 0_tv);
      QCOMPARE(c->get_end_event().get_status(), time_event::status::HAPPENED);
      s.interval->tick(1000_tv);
      QCOMPARE(c->get_date(), 0_tv);
      QCOMPARE(c->get_end_event().get_status(), time_event::status::HAPPENED);
      s.interval->tick(1000_tv);
      QCOMPARE(c->get_date(), 0_tv);
      QCOMPARE(c->get_end_event().get_status(), time_event::status::HAPPENED);
    }

    void test_exec_chain_long()
    {
      using namespace ossia;
      root_scenario s;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 3000_tv, 3000_tv, 3000_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 3000_tv, 3000_tv, 3000_tv);

      s.scenario->add_time_interval(c0);
      s.scenario->add_time_interval(c1);

      s.interval->start_and_tick();
      s.interval->tick(500_tv);
      QCOMPARE(c0->get_date(), 500_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.interval->tick(3000_tv);

      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 500_tv);
    }

    void test_exec_chain_multi()
    {
      std::cerr << "\n\ntest_exec_chain_multi\n";
      using namespace ossia;

      root_scenario s;
      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);
      std::shared_ptr<time_event> e3 = create_event(scenario);
      std::shared_ptr<time_event> e4 = create_event(scenario);


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 1_tv, 1_tv, 1_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 1_tv, 1_tv, 1_tv);
      std::shared_ptr<time_interval> c2 = time_interval::create([] (auto&&...) {}, *e2, *e3, 1_tv, 1_tv, 1_tv);
      std::shared_ptr<time_interval> c3 = time_interval::create([] (auto&&...) {}, *e3, *e4, 10_tv, 10_tv, 10_tv);

      s.scenario->add_time_interval(c0);
      s.scenario->add_time_interval(c1);
      s.scenario->add_time_interval(c2);
      s.scenario->add_time_interval(c3);

      std::cerr << c0.get() << " " << c1.get() << " " << c2.get() << " " << c3.get() << std::endl;
      s.interval->start_and_tick();
      s.interval->tick(5_tv);
      std::cerr << e0->get_status() << " "
                << e1->get_status() << " "
                << e2->get_status() << " "
                << e3->get_status() << std::endl;

      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      QCOMPARE(c2->get_date(), 0_tv);
      QCOMPARE(c3->get_date(), 2_tv);
    }

    void test_exec_chain_multi_infinite()
    {
      std::cerr << "\n\ntest_exec_chain_multi\n";
      using namespace ossia;

      root_scenario s;
      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario); e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
      std::shared_ptr<time_event> e2 = create_event(scenario); e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());
      std::shared_ptr<time_event> e3 = create_event(scenario); e3->get_time_sync().set_expression(ossia::expressions::make_expression_false());
      std::shared_ptr<time_event> e4 = create_event(scenario); e4->get_time_sync().set_expression(ossia::expressions::make_expression_false());


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 1_tv, 0_tv, ossia::Infinite);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 1_tv, 0_tv, ossia::Infinite);
      std::shared_ptr<time_interval> c2 = time_interval::create([] (auto&&...) {}, *e2, *e3, 1_tv, 0_tv, ossia::Infinite);
      std::shared_ptr<time_interval> c3 = time_interval::create([] (auto&&...) {}, *e3, *e4, 1_tv, 0_tv, ossia::Infinite);

      s.scenario->add_time_interval(c0);
      s.scenario->add_time_interval(c1);
      s.scenario->add_time_interval(c2);
      s.scenario->add_time_interval(c3);

      std::cerr << c0.get() << " " << c1.get() << " " << c2.get() << " " << c3.get() << std::endl;
      s.interval->start_and_tick();
      {
        s.interval->tick(5_tv);
        qDebug() << e1->get_status() << " "
                 << e2->get_status() << " "
                 << e3->get_status() << " "
                 << e4->get_status();

        QCOMPARE(c0->get_date(), 5_tv);
        QCOMPARE(e1->get_status(), time_event::status::PENDING);

        QCOMPARE(c1->get_date(), 0_tv);
        QCOMPARE(e2->get_status(), time_event::status::NONE);

        QCOMPARE(c2->get_date(), 0_tv);
        QCOMPARE(e3->get_status(), time_event::status::NONE);

        QCOMPARE(c3->get_date(), 0_tv);
        QCOMPARE(e4->get_status(), time_event::status::NONE);
      }

      e1->get_time_sync().set_expression(ossia::expressions::make_expression_true());

      // The "interactivity" tick introduced
      {
        s.interval->tick(5_tv);
        qDebug() << e1->get_status() << " "
                 << e2->get_status() << " "
                 << e3->get_status() << " "
                 << e4->get_status();

        QCOMPARE(c0->get_date(), 0_tv);
        QCOMPARE(e1->get_status(), time_event::status::HAPPENED);

        QCOMPARE(c1->get_date(), 0_tv);
        QCOMPARE(e2->get_status(), time_event::status::NONE);

        QCOMPARE(c2->get_date(), 0_tv);
        QCOMPARE(e3->get_status(), time_event::status::NONE);

        QCOMPARE(c3->get_date(), 0_tv);
        QCOMPARE(e4->get_status(), time_event::status::NONE);
      }

      {
        s.interval->tick(5_tv);
        qDebug() << e1->get_status() << " "
                 << e2->get_status() << " "
                 << e3->get_status() << " "
                 << e4->get_status();

        QCOMPARE(c0->get_date(), 0_tv);
        QCOMPARE(e1->get_status(), time_event::status::HAPPENED);

        QCOMPARE(c1->get_date(), 5_tv);
        QCOMPARE(e2->get_status(), time_event::status::PENDING);

        QCOMPARE(c2->get_date(), 0_tv);
        QCOMPARE(e3->get_status(), time_event::status::NONE);

        QCOMPARE(c3->get_date(), 0_tv);
        QCOMPARE(e4->get_status(), time_event::status::NONE);
      }
    }


    void test_exec_two_branch_infinite()
    {
      std::cerr << "\n\ntest_exec_chain_multi\n";
      using namespace ossia;

      root_scenario s;
      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario); e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
      std::shared_ptr<time_event> e2 = create_event(scenario); e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());
      std::shared_ptr<time_event> e3 = create_event(scenario); e3->get_time_sync().set_expression(ossia::expressions::make_expression_false());

      /*           c0
       * e0 - - - - - - - - - - e2 - )
       * |    c1         c2     |      c3
       * | - - - - e1 - - - - - | ------------ e3
       */

      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e2, 20_tv, 0_tv, 25_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e0, *e1, 15_tv, 0_tv, ossia::Infinite);
      std::shared_ptr<time_interval> c2 = time_interval::create([] (auto&&...) {}, *e1, *e2, 5_tv, 0_tv, ossia::Infinite);
      std::shared_ptr<time_interval> c3 = time_interval::create([] (auto&&...) {}, *e2, *e3, 100_tv, 100_tv, 100_tv);

      s.scenario->add_time_interval(c0);
      s.scenario->add_time_interval(c1);
      s.scenario->add_time_interval(c2);
      s.scenario->add_time_interval(c3);

      s.interval->start_and_tick();
      {
        s.interval->tick(5_tv);
        qDebug() << e1->get_status() << " "
                 << e2->get_status() << " "
                 << e3->get_status();

        QCOMPARE(c0->get_date(), 5_tv);
        QCOMPARE(e1->get_status(), time_event::status::PENDING);

        QCOMPARE(c1->get_date(), 5_tv);
        QCOMPARE(e2->get_status(), time_event::status::NONE);

        QCOMPARE(c2->get_date(), 0_tv);
        QCOMPARE(e3->get_status(), time_event::status::NONE);

        QCOMPARE(c3->get_date(), 0_tv);
      }

      {
        s.interval->tick(30_tv);
        qDebug() << e1->get_status() << " "
                 << e2->get_status() << " "
                 << e3->get_status();

        QCOMPARE(c0->get_date(), 25_tv);
        QCOMPARE(e1->get_status(), time_event::status::PENDING);

        QCOMPARE(c1->get_date(), 35_tv);
        QCOMPARE(e2->get_status(), time_event::status::NONE);

        QCOMPARE(c2->get_date(), 0_tv);
        QCOMPARE(e3->get_status(), time_event::status::NONE);

        QCOMPARE(c3->get_date(), 0_tv);
      }

      e1->get_time_sync().set_expression(ossia::expressions::make_expression_true());

      {
        s.interval->tick(2_tv);
        qDebug() << e1->get_status() << " "
                 << e2->get_status() << " "
                 << e3->get_status();

        QCOMPARE(c0->get_date(), 0_tv);
        QCOMPARE(e1->get_status(), time_event::status::HAPPENED);

        QCOMPARE(c1->get_date(), 0_tv);
        QCOMPARE(e2->get_status(), time_event::status::HAPPENED);

        QCOMPARE(c2->get_date(), 0_tv);
        QCOMPARE(e3->get_status(), time_event::status::NONE);

        QCOMPARE(c3->get_date(), 2_tv);
      }

      {
        s.interval->tick(2_tv);
        qDebug() << e1->get_status() << " "
                 << e2->get_status() << " "
                 << e3->get_status();

        QCOMPARE(c0->get_date(), 0_tv);
        QCOMPARE(e1->get_status(), time_event::status::HAPPENED);

        QCOMPARE(c1->get_date(), 0_tv);
        QCOMPARE(e2->get_status(), time_event::status::HAPPENED);

        QCOMPARE(c2->get_date(), 0_tv);
        QCOMPARE(e3->get_status(), time_event::status::NONE);

        QCOMPARE(c3->get_date(), 4_tv);
      }

    }

    void test_min()
    {
      std::cerr << "\n\ntest_min\n";
      using namespace ossia;
      root_scenario s;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

      s.scenario->add_time_interval(c0);
      s.scenario->add_time_interval(c1);

      s.interval->start_and_tick();
      s.interval->tick(1000_tv);
      QCOMPARE(c0->get_date(), 1000_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.interval->tick(1500_tv); // Get into the min; the node is triggered
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0_tv);

      // whole tick goes in the next interval
      s.interval->tick(5000_tv);

      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 5000_tv);
    }

    void test_max()
    {
      std::cerr << "\n\ntest_max\n";
      using namespace ossia;
      root_scenario s;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);
      e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

      scenario.add_time_interval(c0);
      scenario.add_time_interval(c1);

      s.interval->start_and_tick();
      s.interval->tick(3000_tv);
      QCOMPARE(c0->get_date(), 3000_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.interval->tick(1500_tv); // Go past the max
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 500_tv);
    }



    void test_inter_tick()
    {
      std::cerr << "\n\ntest_inter_tick\n";
      using namespace ossia;
      root_scenario s;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);
      std::shared_ptr<time_event> e3 = create_event(scenario);
      e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 3000_tv, 3000_tv, 3000_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 500_tv, 500_tv, 500_tv);
      std::shared_ptr<time_interval> c2 = time_interval::create([] (auto&&...) {}, *e2, *e3, 3000_tv, 3000_tv, 3000_tv);

      scenario.add_time_interval(c0);
      scenario.add_time_interval(c1);
      scenario.add_time_interval(c2);

      s.interval->start_and_tick();
      s.interval->tick(1000_tv);
      QCOMPARE(c0->get_date(), 1000_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      QCOMPARE(c2->get_date(), 0_tv);
      s.interval->tick(3000_tv); // Go past the max and way into c2
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      QCOMPARE(c2->get_date(), 500_tv);
    }

    void test_unconnected()
    {
      std::cerr << "\n\ntest_unconnected\n";

      root_scenario s;
      using namespace ossia;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = create_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);

      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 2_tv, 2_tv, 2_tv);
      s.scenario->add_time_interval(c0);

      QCOMPARE(c0->get_position(), 0.);
      QVERIFY(e0->get_status() == time_event::status::NONE);
      QVERIFY(e1->get_status() == time_event::status::NONE);

      s.interval->start_and_tick();

      QCOMPARE(c0->get_position(), 0.);
      QVERIFY(e0->get_status() == time_event::status::NONE);
      QVERIFY(e1->get_status() == time_event::status::NONE);

      s.interval->tick(1_tv);

      QCOMPARE(c0->get_position(), 0.);
      QVERIFY(e0->get_status() == time_event::status::NONE);
      QVERIFY(e1->get_status() == time_event::status::NONE);

      s.interval->tick(1_tv);

      QCOMPARE(c0->get_position(), 0.);
      QVERIFY(e0->get_status() == time_event::status::NONE);
      QVERIFY(e1->get_status() == time_event::status::NONE);
    }


    void test_simulated_state()
    {
      std::cerr << "\n\test_simulated_state\n";
      using namespace ossia;
      root_scenario s;
      TestDevice utils;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);
      std::shared_ptr<time_event> e3 = create_event(scenario);

      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 2_tv, 2_tv, 2_tv);
      s.scenario->add_time_interval(c0);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 0_tv, 0_tv, 0_tv);
      s.scenario->add_time_interval(c1);
      std::shared_ptr<time_interval> c2 = time_interval::create([] (auto&&...) {}, *e2, *e3, 20_tv, 20_tv, 20_tv);
      s.scenario->add_time_interval(c2);

      auto msg_node = std::make_shared<message_node>();
      msg_node->data.push_back({*utils.float_addr, ossia::value(1.234)});

      auto msg_proc = std::make_shared<ossia::node_process>(msg_node);

      c1->add_time_process(msg_proc);
      s.interval->start_and_tick();
      s.interval->tick(10_tv);

      for(auto tr : msg_node->requested_tokens) qDebug() << tr;
      QCOMPARE((int)msg_node->requested_tokens.size(), (int) 1);
      auto t0 = msg_node->requested_tokens[0];
      auto expected = token_request{0_tv, 0., 2_tv};
      QCOMPARE(t0, expected);

      QVERIFY(utils.float_addr->value() == ossia::value(float(0.)));

      ossia::graph g;
      g.add_node(msg_node);
      g.state();

      QVERIFY(utils.float_addr->value() == ossia::value(float(1.234)));
      QCOMPARE(c2->get_date(), 8_tv);
    }

    void test_trigger()
    {
      std::cerr << "\n\ntest_trigger\n";
      using namespace ossia;
      root_scenario s;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);
      e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(1, ossia::expressions::comparator::EQUAL, 1));


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 30_tv, 20_tv, 40_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

      scenario.add_time_interval(c0);
      scenario.add_time_interval(c1);

      s.interval->start_and_tick();
      s.interval->tick(15_tv);
      QCOMPARE(c0->get_date(), 15_tv);
      QCOMPARE(c1->get_date(), 0_tv);

      s.interval->tick(15_tv);
      qDebug() << c0->get_date() << c1->get_date();
      QCOMPARE(c0->get_date(), 0_tv); // executed, we go on to the next one
      QCOMPARE(c1->get_date(), 0_tv);
      QVERIFY(e1->get_status() == ossia::time_event::status::HAPPENED);

      s.interval->tick(15_tv);
      qDebug() << c0->get_date() << c1->get_date();
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 15_tv);

    }
    void test_speed()
    {

    }


    void test_tokens()
    {
      std::cerr << "\n\ntest_tokens\n";
      using namespace ossia;
      root_scenario s;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      std::shared_ptr<time_event> e2 = create_event(scenario);
      e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 300_tv, 300_tv, 300_tv);
      std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 500_tv, 500_tv, 500_tv);

      scenario.add_time_interval(c0);
      scenario.add_time_interval(c1);
      s.interval->start();
      s.interval->tick(700_tv);

      chobo::small_vector<token_request, 4> expected0{{0_tv, 0., 0_tv}, {300_tv, 1., 0_tv}};
      QVERIFY(c0->node->requested_tokens == expected0);
      qDebug() << c1->node->requested_tokens.size();
      qDebug() << c1->node->requested_tokens[0];
      QVERIFY(c1->node->requested_tokens.size() == 2);
      qDebug() << c1->node->requested_tokens[1];
      chobo::small_vector<token_request, 4> expected1{{0_tv, 0., 300_tv}, {400_tv, 4./5., 300_tv}};
      QVERIFY(c1->node->requested_tokens == expected1);

    }

    void test_autom()
    {
//      std::cerr << "\n\ntest_autom\n";
//      using namespace ossia;
//      root_scenario s;
//      TestDevice utils;

//      ossia::scenario& scenario = *s.scenario;
//      std::shared_ptr<time_event> e0 = start_event(scenario);
//      std::shared_ptr<time_event> e1 = create_event(scenario);

//      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 2_tv, 2_tv, 2_tv);
//      s.scenario->add_time_interval(c0);

//      std::shared_ptr<ossia::automation> proc = std::make_shared<ossia::automation>();
//      proc->set_destination(*utils.float_addr);

//      auto crv = std::make_shared<curve<double, float>>();
//      curve_segment_linear<float> linearSegment;

//      crv->set_x0(0.);
//      crv->set_y0(0.);
//      crv->add_point(linearSegment, 1., 1.);

//      proc->set_behavior(crv);
//      c0->add_time_process(proc);

//      s.interval->start_and_tick();
//      s.interval->tick(1000_tv);
//      s.interval->tick(999_tv);
//      s.interval->tick(1_tv);
    }

    void test_autom_and_state()
    {
//      std::cerr << "\n\ntest_autom_and_state\n";
//      using namespace ossia;
//      root_scenario s;
//      TestDevice utils;

//      ossia::scenario& scenario = *s.scenario;
//      std::shared_ptr<time_event> e0 = start_event(scenario);
//      std::shared_ptr<time_event> e1 = create_event(scenario);
//      e0->add_state(ossia::message{*utils.float_addr, ossia::value{36.}});
//      e1->add_state(ossia::message{*utils.float_addr, ossia::value{24.}});


//      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 2_tv, 2_tv, 2_tv);
//      s.scenario->add_time_interval(c0);

//      std::shared_ptr<ossia::automation> proc = std::make_shared<ossia::automation>();
//      proc->set_destination(*utils.float_addr);

//      auto crv = std::make_shared<curve<double, float>>();
//      curve_segment_linear<float> linearSegment;

//      crv->set_x0(0.);
//      crv->set_y0(0.);
//      crv->add_point(linearSegment, 1., 1.);

//      proc->set_behavior(crv);
//      c0->add_time_process(proc);

//      s.interval->start_and_tick();
//      s.interval->tick(1000_tv);
//      s.interval->tick(999_tv);
//      s.interval->tick(1_tv);
    }


    void test_percentage()
    {
      std::cerr << "\n\ntest_percentage\n";
      using namespace ossia;
      root_scenario s;
      TestDevice utils;
      ossia::graph g;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);

      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 2_tv, 2_tv, 2_tv);
      s.scenario->add_time_interval(c0);

      auto node = std::make_shared<percentage_node>(*utils.float_addr);
      auto proc = std::make_shared<ossia::node_process>(node);
      c0->add_time_process(proc);
      g.add_node(node);


      s.interval->start_and_tick();
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.size() == 1);
        QVERIFY(s.valueState.begin()->second.back().value == ossia::value(0.f));
      }

      s.interval->tick(1_tv);
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.size() == 1);
        QVERIFY(s.valueState.begin()->second.back().value == ossia::value(0.5f));
      }

      s.interval->tick(1_tv);
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.size() == 1);
        QVERIFY(s.valueState.begin()->second.back().value == ossia::value(1.f));
      }

      s.interval->tick(1_tv);
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.empty());
      }
    }

    void test_percentage_long()
    {
      std::cerr << "\n\ntest_percentage_long\n";
      using namespace ossia;
      root_scenario s;
      TestDevice utils;
      ossia::graph g;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);

      std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 5_tv, 5_tv, 5_tv);
      s.scenario->add_time_interval(c0);

      auto node = std::make_shared<percentage_node>(*utils.float_addr);
      auto proc = std::make_shared<ossia::node_process>(node);
      c0->add_time_process(proc);
      g.add_node(node);


      s.interval->start_and_tick();
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.size() == 1);
        QVERIFY(s.valueState.begin()->second.back().value == ossia::value(0.f));
      }

      s.interval->tick(3_tv);
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.size() == 1);
        QVERIFY(s.valueState.begin()->second.back().value == ossia::value(3./5.));
      }

      s.interval->tick(7_tv);
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.size() == 1);
        QVERIFY(s.valueState.begin()->second.back().value == ossia::value(1.f));
      }

      s.interval->tick(1_tv);
      {
        ossia::execution_state s;
        g.state(s);
        QVERIFY(s.valueState.empty());
      }
    }

    void test_offset()
    {
//        using namespace ossia;
//        root_scenario s;
//        TestDevice utils;

//        ossia::scenario& scenario = *s.scenario;
//        std::shared_ptr<time_event> e0 = start_event(scenario);
//        std::shared_ptr<time_event> e1 = create_event(scenario);
//        std::shared_ptr<time_event> e2 = create_event(scenario);
//        std::shared_ptr<time_event> e3 = create_event(scenario);

//        e1->add_state(ossia::message{*utils.float_addr, ossia::value{24.}});
//        e2->add_state(ossia::message{*utils.float_addr, ossia::value{31.}});

//        std::shared_ptr<time_interval> c0 = time_interval::create([] (auto&&...) {}, *e0, *e1, 5_tv, 5_tv, 5_tv);
//        s.scenario->add_time_interval(c0);
//        std::shared_ptr<time_interval> c1 = time_interval::create([] (auto&&...) {}, *e1, *e2, 5_tv, 5_tv, 5_tv);
//        s.scenario->add_time_interval(c1);
//        std::shared_ptr<time_interval> c2 = time_interval::create([] (auto&&...) {}, *e2, *e3, 5_tv, 5_tv, 5_tv);
//        s.scenario->add_time_interval(c1);

//        {
//            auto st = s.interval->offset(7_tv);
//            ossia::print(std::cerr, st);

//            ossia::state expected;
//            {
//                ossia::state sub;
//                sub.add(ossia::message{*utils.float_addr, ossia::value{24.}});
//                expected.add(sub);
//            }
//            QVERIFY(st == expected);
//        }

//        {
//            ossia::state st;

//            ossia::state expected;

//            s.interval->start_and_tick(st);
//            ossia::print(std::cerr, st);

//            QVERIFY(st == expected);
//        }

//        s.interval->stop();

//        {
//            auto st = s.interval->offset(13_tv);
//            ossia::print(std::cerr, st);

//            ossia::state expected;
//            {
//                ossia::state sub;
//                sub.add(ossia::message{*utils.float_addr, ossia::value{31.}});
//                expected.add(sub);
//            }
//            QVERIFY(st == expected);
//        }

//        {
//            ossia::state st;

//            ossia::state expected;

//            s.interval->start_and_tick(st);
//            ossia::print(std::cerr, st);

//            QVERIFY(st == expected);
//        }

    }
};
QTEST_APPLESS_MAIN(ScenarioAlgoTest)

#include "ScenarioAlgoTest.moc"

