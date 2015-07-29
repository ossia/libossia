#include <QtTest>
#include "../ForwardDeclaration.h"
#include <functional>
#include <iostream>

using namespace OSSIA;
using namespace std::placeholders;

class ScenarioTest : public QObject
{
    Q_OBJECT

    void main_constraint_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> element)
    {
        std::cout << "Main Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void first_constraint_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> element)
    {
        std::cout << "First Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void second_constraint_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> element)
    {
        std::cout << "Second Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void event_callback(TimeEvent::Status newStatus)
    {
        switch (newStatus)
        {
            case TimeEvent::Status::NONE:
            {
                std::cout << "Event NONE" << std::endl;
                break;
            }
            case TimeEvent::Status::PENDING:
            {
                std::cout << "Event PENDING" << std::endl;
                break;
            }
            case TimeEvent::Status::HAPPENED:
            {
                std::cout << "Event HAPPENED" << std::endl;
                break;
            }
            case TimeEvent::Status::DISPOSED:
            {
                std::cout << "Event DISPOSED" << std::endl;
                break;
            }
        }
    }

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto scenario = Scenario::create();
        QVERIFY(scenario != nullptr);
        
        QVERIFY(scenario->getStartState() != nullptr);
        QVERIFY(scenario->getEndState() != nullptr);
        QVERIFY(scenario->getParentTimeConstraint() == nullptr);
        
        QVERIFY(scenario->getStartNode() != nullptr);
        QVERIFY(scenario->getEndNode() != nullptr);
        QVERIFY(scenario->isKiller() == false);
        
        scenario->setKiller(true);
        QVERIFY(scenario->isKiller() == true);

        QVERIFY(scenario->timeNodes().size() == 2);
        QVERIFY(scenario->timeConstraints().size() == 0);

        QVERIFY(scenario->getStartNode()->getDate() == 0.);
        QVERIFY(scenario->getEndNode()->getDate() == Infinite);
    }
    
    /*! test edition functions */
    void test_edition()
    {
        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

        auto scenario = Scenario::create();

        auto start_node = scenario->getStartNode();
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), e_callback));

        auto end_node = TimeNode::create();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), e_callback));

        auto constraint = TimeConstraint::create(mc_callback, start_event, end_event, 1000.);

        scenario->addTimeConstraint(constraint);
        QVERIFY(scenario->timeConstraints().size() == 1);
        QVERIFY(scenario->timeNodes().size() == 3);

        scenario->removeTimeConstraint(constraint);
        QVERIFY(scenario->timeConstraints().size() == 0);
        QVERIFY(scenario->timeNodes().size() == 3);

        auto lonely_node = TimeNode::create();

        scenario->addTimeNode(lonely_node);
        QVERIFY(scenario->timeNodes().size() == 4);

        scenario->removeTimeNode(lonely_node);
        QVERIFY(scenario->timeNodes().size() == 3);

        //! \todo how to verify something here ?
        auto scenario_copy = scenario->clone();
    }

    /*! test execution functions */
    //! \todo maybe a way to tet many scenario would be to load them from a files
    void test_execution()
    {
        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto fc_callback = std::bind(&ScenarioTest::first_constraint_callback, this, _1, _2, _3);
        auto sc_callback = std::bind(&ScenarioTest::second_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

        auto main_start_node = TimeNode::create();
        auto main_end_node = TimeNode::create();
        auto main_start_event = *(main_start_node->emplace(main_start_node->timeEvents().begin(), e_callback));
        auto main_end_event = *(main_end_node->emplace(main_end_node->timeEvents().begin(), e_callback));
        auto main_constraint = TimeConstraint::create(mc_callback, main_start_event, main_end_event, 5000.);
        auto main_scenario = Scenario::create();

        main_constraint->addTimeProcess(main_scenario);

        auto scenario_start_node = main_scenario->getStartNode();

        auto first_end_node = TimeNode::create();
        auto first_start_event = *(scenario_start_node->emplace(scenario_start_node->timeEvents().begin(), e_callback));
        auto first_end_event = *(first_end_node->emplace(first_end_node->timeEvents().begin(), e_callback));
        auto first_constraint = TimeConstraint::create(fc_callback, first_start_event, first_end_event, 1500., 1500., 1500.);

        main_scenario->addTimeConstraint(first_constraint);

        auto second_end_node = TimeNode::create();
        auto second_end_event = *(second_end_node->emplace(second_end_node->timeEvents().begin(), e_callback));
        auto second_constraint = TimeConstraint::create(sc_callback, first_end_event, second_end_event, 2000., 2000., 2000.);

        main_scenario->addTimeConstraint(second_constraint);

        main_constraint->setSpeed(1.);
        main_constraint->setGranularity(50.);
        first_constraint->setSpeed(1.);
        first_constraint->setGranularity(250.);
        second_constraint->setSpeed(1.);
        second_constraint->setGranularity(250.);

        main_constraint->start();

        while (main_constraint->getRunning())
            ;
    }
};

QTEST_APPLESS_MAIN(ScenarioTest)

#include "ScenarioTest.moc"
