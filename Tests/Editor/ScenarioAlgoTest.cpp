#include <QtTest>

#include "TestUtils.hpp"

namespace ossia {
char* toString(const ossia::time_value& val)
{
  return QTest::toString(QByteArray::number((qint64)val.impl));
}
}

class ScenarioAlgoTest : public QObject
{
  Q_OBJECT

    auto create_event(ossia::scenario& s)
    {
      auto en = std::make_shared<ossia::time_node>();
      en->set_expression(ossia::expressions::make_expression_true());
      auto ee = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *en, ossia::expressions::make_expression_true());
      en->insert(en->get_time_events().end(), ee);
      s.add_time_node(std::move(en));
      return ee;
    }

    auto start_event(ossia::scenario& s)
    {
      auto sn = s.get_start_time_node();
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

      std::shared_ptr<time_constraint> c = time_constraint::create([] (auto&&...) {}, *se, *ee, 3000_tv, 3000_tv, 3000_tv);
      s.scenario->add_time_constraint(c);

      s.constraint->start();
      QVERIFY(se->get_status() == time_event::status::HAPPENED);
      QCOMPARE(c->get_date(), 0_tv);

      s.constraint->tick(1000_tv);
      QCOMPARE(c->get_date(), 1000_tv);
      s.constraint->tick(1000_tv);
      QCOMPARE(c->get_date(), 2000_tv);
      s.constraint->tick(999_tv);
      QCOMPARE(c->get_date(), 2999_tv);
      s.constraint->tick(1_tv); // The constraint is stopped

      QCOMPARE(c->get_date(), 0_tv);
      QCOMPARE(c->get_end_event().get_status(), time_event::status::HAPPENED);
      s.constraint->tick(1000_tv);
      QCOMPARE(c->get_date(), 0_tv);
      QCOMPARE(c->get_end_event().get_status(), time_event::status::HAPPENED);
      s.constraint->tick(1000_tv);
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


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 3000_tv, 3000_tv, 3000_tv);
      std::shared_ptr<time_constraint> c1 = time_constraint::create([] (auto&&...) {}, *e1, *e2, 3000_tv, 3000_tv, 3000_tv);

      s.scenario->add_time_constraint(c0);
      s.scenario->add_time_constraint(c1);

      s.constraint->start();
      s.constraint->tick(500_tv);
      QCOMPARE(c0->get_date(), 500_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.constraint->tick(3000_tv);

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


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 1_tv, 1_tv, 1_tv);
      std::shared_ptr<time_constraint> c1 = time_constraint::create([] (auto&&...) {}, *e1, *e2, 1_tv, 1_tv, 1_tv);
      std::shared_ptr<time_constraint> c2 = time_constraint::create([] (auto&&...) {}, *e2, *e3, 1_tv, 1_tv, 1_tv);
      std::shared_ptr<time_constraint> c3 = time_constraint::create([] (auto&&...) {}, *e3, *e4, 10_tv, 10_tv, 10_tv);

      s.scenario->add_time_constraint(c0);
      s.scenario->add_time_constraint(c1);
      s.scenario->add_time_constraint(c2);
      s.scenario->add_time_constraint(c3);

      std::cerr << c0.get() << " " << c1.get() << " " << c2.get() << " " << c3.get() << std::endl;
      s.constraint->start();
      s.constraint->tick(5_tv);
      std::cerr << e0->get_status() << " "
                << e1->get_status() << " "
                << e2->get_status() << " "
                << e3->get_status() << std::endl;

      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      QCOMPARE(c2->get_date(), 0_tv);
      QCOMPARE(c3->get_date(), 2_tv);
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


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
      std::shared_ptr<time_constraint> c1 = time_constraint::create([] (auto&&...) {}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

      s.scenario->add_time_constraint(c0);
      s.scenario->add_time_constraint(c1);

      s.constraint->start();
      s.constraint->tick(1000_tv);
      QCOMPARE(c0->get_date(), 1000_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.constraint->tick(1500_tv); // Get into the min; the node is triggered
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0_tv);

      // whole tick goes in the next constraint
      s.constraint->tick(5000_tv);

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
      e1->get_time_node().set_expression(ossia::expressions::make_expression_false());


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
      std::shared_ptr<time_constraint> c1 = time_constraint::create([] (auto&&...) {}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

      scenario.add_time_constraint(c0);
      scenario.add_time_constraint(c1);

      s.constraint->start();
      s.constraint->tick(3000_tv);
      QCOMPARE(c0->get_date(), 3000_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.constraint->tick(1500_tv); // Go past the max
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 500_tv);
    }

    void test_autom()
    {
      std::cerr << "\n\ntest_autom\n";
      using namespace ossia;
      root_scenario s;
      TestUtils utils;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);

      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 2_tv, 2_tv, 2_tv);
      s.scenario->add_time_constraint(c0);

      std::shared_ptr<ossia::automation> proc = std::make_shared<ossia::automation>();
      proc->set_destination(*utils.float_addr);

      auto crv = std::make_shared<curve<double, float>>();
      curve_segment_linear<float> linearSegment;

      crv->set_x0(0.);
      crv->set_y0(0.);
      crv->add_point(linearSegment, 1., 1.);

      proc->set_behavior(crv);
      c0->add_time_process(proc);

      s.constraint->set_callback([] (double, time_value, const state_element& s) {
        ossia::print(std::cerr, s);
      });
      s.constraint->start();
      s.constraint->tick(1000_tv);
      s.constraint->tick(999_tv);
      s.constraint->tick(1_tv);
    }

    void test_autom_and_state()
    {
      std::cerr << "\n\ntest_autom_and_state\n";
      using namespace ossia;
      root_scenario s;
      TestUtils utils;

      ossia::scenario& scenario = *s.scenario;
      std::shared_ptr<time_event> e0 = start_event(scenario);
      std::shared_ptr<time_event> e1 = create_event(scenario);
      e0->add_state(ossia::message{*utils.float_addr, ossia::value{36.}});
      e1->add_state(ossia::message{*utils.float_addr, ossia::value{24.}});


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 2_tv, 2_tv, 2_tv);
      s.scenario->add_time_constraint(c0);

      std::shared_ptr<ossia::automation> proc = std::make_shared<ossia::automation>();
      proc->set_destination(*utils.float_addr);

      auto crv = std::make_shared<curve<double, float>>();
      curve_segment_linear<float> linearSegment;

      crv->set_x0(0.);
      crv->set_y0(0.);
      crv->add_point(linearSegment, 1., 1.);

      proc->set_behavior(crv);
      c0->add_time_process(proc);

      s.constraint->set_callback([] (double, time_value, const state_element& s) {
        ossia::print(std::cerr, s);
      });
      s.constraint->start();
      s.constraint->tick(1000_tv);
      s.constraint->tick(999_tv);
      s.constraint->tick(1_tv);

    }
};
QTEST_APPLESS_MAIN(ScenarioAlgoTest)

#include "ScenarioAlgoTest.moc"

