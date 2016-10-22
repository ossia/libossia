#include "TestUtils.hpp"

#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

void constraint_callback(time_value position, time_value date, const state& element)
{
    ;
}

void event_callback(time_event::Status newStatus)
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
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), &event_callback));

        auto end_node = std::make_shared<time_node>();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &event_callback));

        auto constraint = time_constraint::create(&constraint_callback, *start_event, *end_event, 1000._tv);
        QVERIFY(constraint != nullptr);

        QVERIFY(constraint->getGranularity() == 1._tv);
        QVERIFY(constraint->getOffset() == 0._tv);
        QVERIFY(constraint->getSpeed() == 1._tv);
        QVERIFY(constraint->getDurationNominal() == 1000._tv);
        QVERIFY(constraint->getDurationMin() == 0._tv);
        QVERIFY(constraint->getDurationMax() == Infinite);

        constraint->setGranularity(50._tv);
        constraint->setSpeed(2._tv);
        constraint->setDurationNominal(2000._tv);
        constraint->setDurationMin(1000._tv);
        constraint->setDurationMax(3000._tv);
        auto state = constraint->offset(500._tv);

        QVERIFY(constraint->getGranularity() == 50._tv);
        QVERIFY(constraint->getSpeed() == 2._tv);
        QVERIFY(constraint->getDurationNominal() == 2000._tv);
        QVERIFY(constraint->getDurationMin() == 1000._tv);
        QVERIFY(constraint->getDurationMax() == 3000._tv);
        QVERIFY(constraint->getOffset() == 500._tv);
        QVERIFY(state.size() == 0);

        QCOMPARE(constraint->getPosition(), 0.25_tv);
        QVERIFY(constraint->getRunning() == false);
        QVERIFY(constraint->getDate() == 500._tv);

        QVERIFY(&constraint->getStartEvent() == start_event.get());
        QVERIFY(&constraint->getEndEvent() == end_event.get());

        //! \todo test clone()
    }

    /*! test edition functions */
    void test_edition()
    {
        auto start_node = std::make_shared<time_node>();
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), &event_callback));

        auto end_node = std::make_shared<time_node>();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &event_callback));

        auto constraint = time_constraint::create(&constraint_callback, *start_event, *end_event, 1000._tv);
        auto scenar = std::make_unique<scenario>();

        auto scenar_ptr = scenar.get();
        constraint->addTimeProcess(std::move(scenar));
        QVERIFY(constraint->timeProcesses().size() == 1);

        constraint->removeTimeProcess(scenar_ptr);
        QVERIFY(constraint->timeProcesses().size() == 0);
    }

    /*! test execution functions */
    void test_execution()
    {
      TestUtils t;
      auto start_node = std::make_shared<time_node>();
      auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), &event_callback));

      auto end_node = std::make_shared<time_node>();
      auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &event_callback));

      auto constraint = time_constraint::create(&constraint_callback, *start_event, *end_event, 1000._tv);

      auto s = new scenario;

      constraint->addTimeProcess(std::unique_ptr<ossia::time_process>(s));
      auto a = new automation{*t.float_addr, std::make_shared<curve<double, float>>()};
      constraint->addTimeProcess(std::unique_ptr<ossia::time_process>(a));
      auto m = new mapper{*t.float_addr, *t.float_addr, std::make_shared<curve<float, float>>()};
      constraint->addTimeProcess(std::unique_ptr<ossia::time_process>(m));
      auto l = new loop{10._tv, {}, {}, {}};
      constraint->addTimeProcess(std::unique_ptr<ossia::time_process>(l));

      constraint->start();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      constraint->pause();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      constraint->resume();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      constraint->stop();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      QVERIFY_EXCEPTION_THROWN(constraint->state(), execution_error);
      QVERIFY_EXCEPTION_THROWN(static_cast<time_process*>(a)->state(), execution_error);
      QVERIFY_EXCEPTION_THROWN(static_cast<time_process*>(s)->state(), execution_error);
      QVERIFY_EXCEPTION_THROWN(static_cast<time_process*>(m)->state(), execution_error);
      QVERIFY_EXCEPTION_THROWN(static_cast<time_process*>(l)->state(), execution_error);

    }
};

QTEST_APPLESS_MAIN(TimeConstraintTest)

#include "TimeConstraintTest.moc"
