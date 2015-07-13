#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

void scenario_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<State> state)
{
    ;
}

void event_callback(TimeEvent::Status newStatus, TimeEvent::Status oldStatus)
{
    ;
}

class ScenarioTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto scenario = Scenario::create(&scenario_callback);
        QVERIFY(scenario != nullptr);
        
        auto start_state = scenario->getStartState();
        QVERIFY(start_state != nullptr);
        
        auto end_state = scenario->getEndState();
        QVERIFY(end_state != nullptr);
        
        auto clock = scenario->getClock();
        QVERIFY(clock != nullptr);
        
        auto parent_constraint = scenario->getParentTimeConstraint();
        QVERIFY(parent_constraint == nullptr);
        
        auto scenario_start_node = scenario->getStartNode();
        QVERIFY(scenario_start_node != nullptr);
        
        auto scenario_end_node = scenario->getEndNode();
        QVERIFY(scenario_end_node != nullptr);
        
        QVERIFY(scenario->isKiller() == false);
        
        scenario->setKiller(true);
        QVERIFY(scenario->isKiller() == true);
    }
    
    /*! test edition functions */
    void test_edition()
    {
        auto scenario = Scenario::create(&scenario_callback);
        
        auto start_node = scenario->getStartNode();
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), &event_callback));
        
        auto end_node = TimeNode::create();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), &event_callback));
        
        auto constraint = TimeConstraint::create(start_event, end_event, 1000.);
        
        //! \todo how to verify something here ?
        scenario->addTimeConstraint(constraint);
        
        //! \todo how to verify something here ?
        scenario->removeTimeConstraint(constraint);

        auto lonely_node = TimeNode::create();
        
        //! \todo how to verify something here ?
        scenario->addTimeNode(lonely_node);
        
        //! \todo how to verify something here ?
        scenario->removeTimeNode(lonely_node);

        //! \todo how to verify something here ?
        auto scenario_copy = scenario->clone();
    }
};

QTEST_APPLESS_MAIN(ScenarioTest)

#include "ScenarioTest.moc"
