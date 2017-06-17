#include "TestUtils.hpp"

#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

void constraint_callback(double position, ossia::time_value date, const state_element& element)
{
    ;
}

void event_callback(time_event::status newStatus)
{
    ;
}

class TimeConstraintTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto start_node = std::make_shared<time_node>();
        auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), &event_callback));

        auto end_node = std::make_shared<time_node>();
        auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), &event_callback));

        auto constraint = time_constraint::create(&constraint_callback, *start_event, *end_event, 1000._tv);
        ossia::clock c{*constraint};
        QVERIFY(constraint != nullptr);

        QVERIFY(c.get_granularity() == 1._tv);
        QVERIFY(constraint->get_offset() == 0._tv);
        QVERIFY(constraint->get_speed() == 1._tv);
        QVERIFY(constraint->get_nominal_duration() == 1000._tv);
        QVERIFY(constraint->get_min_duration() == 0._tv);
        QVERIFY(constraint->get_max_duration() == Infinite);

        using namespace std::literals;

        c.set_granularity(50ms);
        constraint->set_speed(2._tv);
        constraint->set_nominal_duration(2000._tv);
        constraint->set_min_duration(1000._tv);
        constraint->set_max_duration(3000._tv);
        auto state = constraint->offset(500._tv);

        QVERIFY(c.get_granularity() == 50000._tv);
        QVERIFY(constraint->get_speed() == 2._tv);
        QVERIFY(constraint->get_nominal_duration() == 2000._tv);
        QVERIFY(constraint->get_min_duration() == 1000._tv);
        QVERIFY(constraint->get_max_duration() == 3000._tv);
        QVERIFY(constraint->get_offset() == 500._tv);
        QVERIFY(!state);

        QCOMPARE(constraint->get_position(), 0.25);
        QVERIFY(c.running() == false);
        QVERIFY(constraint->get_date() == 500._tv);

        QVERIFY(&constraint->get_start_event() == start_event.get());
        QVERIFY(&constraint->get_end_event() == end_event.get());

        //! \todo test clone()
    }

    /*! test edition functions */
    void test_edition()
    {
        auto start_node = std::make_shared<time_node>();
        auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), &event_callback));

        auto end_node = std::make_shared<time_node>();
        auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), &event_callback));

        auto constraint = time_constraint::create(&constraint_callback, *start_event, *end_event, 1000._tv);
        auto scenar = std::make_unique<scenario>();

        auto scenar_ptr = scenar.get();
        constraint->add_time_process(std::move(scenar));
        QVERIFY(constraint->get_time_processes().size() == 1);

        constraint->remove_time_process(scenar_ptr);
        QVERIFY(constraint->get_time_processes().size() == 0);
    }

    /*! test execution functions */
    void test_execution()
    {
      TestUtils t;
      auto start_node = std::make_shared<time_node>();
      auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), &event_callback));

      auto end_node = std::make_shared<time_node>();
      auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), &event_callback));

      auto constraint = time_constraint::create(&constraint_callback, *start_event, *end_event, 1000._tv);

      auto s = new scenario;

      constraint->add_time_process(std::unique_ptr<ossia::time_process>(s));
      auto a = new automation{*t.float_addr, std::make_shared<curve<double, float>>()};
      constraint->add_time_process(std::unique_ptr<ossia::time_process>(a));
      auto m = new mapper{*t.float_addr, *t.float_addr, std::make_shared<curve<float, float>>()};
      constraint->add_time_process(std::unique_ptr<ossia::time_process>(m));
      auto l = new loop{10._tv, {}, {}, {}};
      constraint->add_time_process(std::unique_ptr<ossia::time_process>(l));

      constraint->start();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      constraint->pause();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      constraint->resume();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      constraint->stop();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
};

QTEST_APPLESS_MAIN(TimeConstraintTest)

#include "TimeConstraintTest.moc"
