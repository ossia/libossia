#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

void constraint_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> element)
{
    ;
}

void event_callback(TimeEvent::Status newStatus)
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
        auto start_node = TimeNode::create();
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), &event_callback));

        auto end_node = TimeNode::create();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &event_callback));

        auto constraint = TimeConstraint::create(&constraint_callback, start_event, end_event, 1000.);
        QVERIFY(constraint != nullptr);

        QVERIFY(constraint->getDuration() == 1000.);
        QVERIFY(constraint->getGranularity() == 1.);
        QVERIFY(constraint->getOffset() == 0.);
        QVERIFY(constraint->getSpeed() == 1.);
        QVERIFY(constraint->getDurationMin() == 0.);
        QVERIFY(constraint->getDurationMax() == Infinite);

        constraint->setDuration(2000.);
        constraint->setGranularity(50.);
        constraint->setOffset(500.);
        constraint->setSpeed(2.);
        constraint->setDurationMin(1000.);
        constraint->setDurationMax(3000.);

        QVERIFY(constraint->getDuration() == 2000.);
        QVERIFY(constraint->getGranularity() == 50.);
        QVERIFY(constraint->getOffset() == 500.);
        QVERIFY(constraint->getSpeed() == 2.);
        QVERIFY(constraint->getDurationMin() == 1000.);
        QVERIFY(constraint->getDurationMax() == 3000.);

        QVERIFY(constraint->getRunning() == false);
        QVERIFY(constraint->getPosition() == 0.25);
        QVERIFY(constraint->getDate() == 500.);

        QVERIFY(constraint->getStartEvent() == start_event);
        QVERIFY(constraint->getEndEvent() == end_event);

        //! \todo test clone()
    }
    
    /*! test edition functions */
    void test_edition()
    {
        auto start_node = TimeNode::create();
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), &event_callback));

        auto end_node = TimeNode::create();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &event_callback));

        auto constraint = TimeConstraint::create(&constraint_callback, start_event, end_event, 1000.);
        auto scenario = Scenario::create();

        constraint->addTimeProcess(scenario);
        QVERIFY(constraint->timeProcesses().size() == 1);
        QVERIFY(constraint->getStartEvent()->getState()->stateElements().front() == scenario->getStartState());
        QVERIFY(constraint->getEndEvent()->getState()->stateElements().front() == scenario->getEndState());

        constraint->removeTimeProcess(scenario);
        QVERIFY(constraint->timeProcesses().size() == 0);
    }

    /*! test execution functions */
    void test_execution()
    {
        //! \todo test play()
        //! \todo test stop()
        //! \todo test pause()
        //! \todo test resume()
    }
};

QTEST_APPLESS_MAIN(TimeConstraintTest)

#include "TimeConstraintTest.moc"
