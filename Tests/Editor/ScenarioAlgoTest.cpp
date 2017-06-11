#include <QtTest>

#include "TestUtils.hpp"

namespace ossia {
char* toString(const ossia::time_value& val)
{
  return QTest::toString(QByteArray::number(val.impl));
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

      std::shared_ptr<time_constraint> c = time_constraint::create([] (auto&&...) {}, *se, *ee, 3_tv, 3_tv, 3_tv);
      c->set_drive_mode(clock::drive_mode::EXTERNAL);
      s.scenario->add_time_constraint(c);

      s.constraint->start();
      QVERIFY(se->get_status() == time_event::status::HAPPENED);
      QCOMPARE(c->get_date(), 0_tv);

      s.constraint->tick(1000_tv);
      QCOMPARE(c->get_date(), 1_tv);
      s.constraint->tick(1000_tv);
      QCOMPARE(c->get_date(), 2_tv);
      s.constraint->tick(999_tv);
      QCOMPARE(c->get_date(), 2.999_tv);
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


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 3_tv, 3_tv, 3_tv);
      std::shared_ptr<time_constraint> c1 = time_constraint::create([] (auto&&...) {}, *e1, *e2, 3_tv, 3_tv, 3_tv);

      s.scenario->add_time_constraint(c0);
      s.scenario->add_time_constraint(c1);

      s.constraint->start();
      s.constraint->tick(500_tv);
      QCOMPARE(c0->get_date(), 0.5_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.constraint->tick(3000_tv);

      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0.5_tv);
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
      std::shared_ptr<time_constraint> c3 = time_constraint::create([] (auto&&...) {}, *e3, *e4, 1000_tv, 1000_tv, 1000_tv);

      s.scenario->add_time_constraint(c0);
      s.scenario->add_time_constraint(c1);
      s.scenario->add_time_constraint(c2);
      s.scenario->add_time_constraint(c3);

      std::cerr << c0.get() << " " << c1.get() << " " << c2.get() << " " << c3.get() << std::endl;
      s.constraint->start();
      s.constraint->tick(5000_tv);
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


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 3_tv, 2_tv, 4_tv);
      std::shared_ptr<time_constraint> c1 = time_constraint::create([] (auto&&...) {}, *e1, *e2, 100_tv, 100_tv, 100_tv);

      s.scenario->add_time_constraint(c0);
      s.scenario->add_time_constraint(c1);

      s.constraint->start();
      s.constraint->tick(1000_tv);
      QCOMPARE(c0->get_date(), 1_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.constraint->tick(1500_tv); // Get into the min; the node is triggered
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0_tv);

      // whole tick goes in the next constraint
      s.constraint->tick(5000_tv);

      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 5_tv);
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


      std::shared_ptr<time_constraint> c0 = time_constraint::create([] (auto&&...) {}, *e0, *e1, 3_tv, 2_tv, 4_tv);
      std::shared_ptr<time_constraint> c1 = time_constraint::create([] (auto&&...) {}, *e1, *e2, 100_tv, 100_tv, 100_tv);

      s.scenario->add_time_constraint(c0);
      s.scenario->add_time_constraint(c1);

      s.constraint->start();
      s.constraint->tick(3000_tv);
      QCOMPARE(c0->get_date(), 3_tv);
      QCOMPARE(c1->get_date(), 0_tv);
      s.constraint->tick(1500_tv); // Go past the max
      QCOMPARE(c0->get_date(), 0_tv);
      QCOMPARE(c1->get_date(), 0.5_tv);

    }
};
QTEST_APPLESS_MAIN(ScenarioAlgoTest)

#include "ScenarioAlgoTest.moc"

