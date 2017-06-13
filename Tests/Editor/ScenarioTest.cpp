#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class ScenarioTest : public QObject
{
  Q_OBJECT

  std::shared_ptr<time_constraint> main_constraint;
  std::vector<time_value> events_date;

  void main_constraint_callback(ossia::time_value position, time_value date, const state_element& element)
  {
    std::cout << "Main Constraint : " << double(position) << ", " << double(date) << std::endl;
  }

  void first_constraint_callback(ossia::time_value position, time_value date, const state_element& element)
  {
    std::cout << "First Constraint : " << double(position) << ", " << double(date) << std::endl;
  }

  void second_constraint_callback(ossia::time_value position, time_value date, const state_element& element)
  {
    std::cout << "Second Constraint : " << double(position) << ", " << double(date) << std::endl;
  }

  void event_callback(time_event::status newStatus)
  {
    time_value date = main_constraint->get_date();

    switch (newStatus)
    {
      case time_event::status::NONE:
      {
        std::cout << "Event NONE" << std::endl;
        break;
      }
      case time_event::status::PENDING:
      {
        std::cout << "Event PENDING at " << double(date) << " ms" << std::endl;
        break;
      }
      case time_event::status::HAPPENED:
      {
        std::cout << "Event HAPPENED at " << double(date) << " ms" << std::endl;
        events_date.push_back(date);
        break;
      }
      case time_event::status::DISPOSED:
      {
        std::cout << "Event DISPOSED at " << double(date) << " ms" << std::endl;
        break;
      }
    }
  }

private Q_SLOTS:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    auto scenar = std::make_shared<scenario>();
    QVERIFY(scenar != nullptr);

    QVERIFY(scenar->parent() == nullptr);

    QVERIFY(scenar->get_start_time_node() != nullptr);

    QVERIFY(scenar->get_time_nodes().size() == 1);
    QVERIFY(scenar->get_time_constraints().size() == 0);

    QVERIFY(scenar->get_start_time_node()->get_date() == 0.);

    auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
    auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);
    auto start_event = *(scenar->get_start_time_node()->emplace(
                           scenar->get_start_time_node()->get_time_events().begin(),
                           e_callback));

    auto end_node = std::make_shared<time_node>();
    auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), e_callback));
    auto constraint = time_constraint::create(mc_callback, *start_event, *end_event, 1000._tv, 1000._tv, 1000._tv);

    QVERIFY(end_node->get_date() == 1000._tv);
  }

  /*! test edition functions */
  void test_edition()
  {
    auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
    auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

    auto scenar = std::make_shared<scenario>();

    auto start_node = scenar->get_start_time_node();
    auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), e_callback));

    auto end_node = std::make_shared<time_node>();
    auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), e_callback));

    auto constraint = time_constraint::create(mc_callback, *start_event, *end_event, 1000._tv, 1000._tv, 1000._tv);

    scenar->add_time_constraint(constraint);
    QVERIFY(scenar->get_time_constraints().size() == 1);
    QVERIFY(scenar->get_time_nodes().size() == 2);

    scenar->remove_time_constraint(constraint);
    QVERIFY(scenar->get_time_constraints().size() == 0);
    QVERIFY(scenar->get_time_nodes().size() == 2);

    auto lonely_node = std::make_shared<time_node>();

    scenar->add_time_node(lonely_node);
    QVERIFY(scenar->get_time_nodes().size() == 3);

    scenar->remove_time_node(lonely_node);
    QVERIFY(scenar->get_time_nodes().size() == 2);
  }

  /*! test execution functions */
  //! \todo maybe a way to test many scenario would be to load them from a files
  void test_execution()
  {
    using namespace ossia;
    auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
    auto fc_callback = std::bind(&ScenarioTest::first_constraint_callback, this, _1, _2, _3);
    auto sc_callback = std::bind(&ScenarioTest::second_constraint_callback, this, _1, _2, _3);
    auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

    auto main_start_node = std::make_shared<time_node>();
    auto main_end_node = std::make_shared<time_node>();
    auto main_start_event = *(main_start_node->emplace(main_start_node->get_time_events().begin(), e_callback));
    auto main_end_event = *(main_end_node->emplace(main_end_node->get_time_events().begin(), e_callback));
    main_constraint = time_constraint::create(mc_callback, *main_start_event, *main_end_event, 5000._tv, 5000._tv, 5000._tv);

    auto main_scenario = std::make_unique<scenario>();

    auto scenario_start_node = main_scenario->get_start_time_node();

    auto first_end_node = std::make_shared<time_node>();
    auto first_start_event = *(scenario_start_node->emplace(scenario_start_node->get_time_events().begin(), e_callback));
    auto first_end_event = *(first_end_node->emplace(first_end_node->get_time_events().begin(), e_callback));
    auto first_constraint = time_constraint::create(fc_callback, *first_start_event, *first_end_event, 1500._tv, 1500._tv, 1500._tv);

    main_scenario->add_time_constraint(first_constraint);

    auto second_end_node = std::make_shared<time_node>();
    auto second_end_event = *(second_end_node->emplace(second_end_node->get_time_events().begin(), e_callback));
    auto second_constraint = time_constraint::create(sc_callback, *first_end_event, *second_end_event, 2000._tv, 2000._tv, 2000._tv);

    main_scenario->add_time_constraint(second_constraint);

    main_constraint->add_time_process(std::move(main_scenario));

    main_constraint->set_speed(1._tv);
    main_constraint->set_granularity(50._tv);
    first_constraint->set_speed(1._tv);
    first_constraint->set_granularity(25._tv);
    second_constraint->set_speed(1._tv);
    second_constraint->set_granularity(25._tv);

    events_date.clear();
    main_constraint->start();

    while (main_constraint->running())
      ;

    // check TimeEvents date
    QCOMPARE((int)events_date.size(), 3);
    QVERIFY(events_date[0] == Zero);
    QVERIFY(events_date[1] >= first_end_node->get_date());
    // todo QVERIFY(events_date[1] < (first_end_node->get_date() + main_constraint->getGranularity()));
    QVERIFY(events_date[2] >= first_end_node->get_date());
    // todo QVERIFY(events_date[2] < first_end_node->get_date() + main_constraint->getGranularity());
  }
};

QTEST_APPLESS_MAIN(ScenarioTest)

#include "ScenarioTest.moc"
