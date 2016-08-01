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

    void main_constraint_callback(time_value position, time_value date, const state& element)
    {
        std::cout << "Main Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void first_constraint_callback(time_value position, time_value date, const state& element)
    {
        std::cout << "First Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void second_constraint_callback(time_value position, time_value date, const state& element)
    {
        std::cout << "Second Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void event_callback(time_event::Status newStatus)
    {
        time_value date = main_constraint->getDate();

        switch (newStatus)
        {
            case time_event::Status::NONE:
            {
                std::cout << "Event NONE" << std::endl;
                break;
            }
            case time_event::Status::PENDING:
            {
                std::cout << "Event PENDING at " << double(date) << " ms" << std::endl;
                break;
            }
            case time_event::Status::HAPPENED:
            {
                std::cout << "Event HAPPENED at " << double(date) << " ms" << std::endl;
                events_date.push_back(date);
                break;
            }
            case time_event::Status::DISPOSED:
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
        auto scenario = scenario::create();
        QVERIFY(scenario != nullptr);

        QVERIFY(scenario->parent == nullptr);

        QVERIFY(scenario->getStartTimeNode() != nullptr);

        QVERIFY(scenario->timeNodes().size() == 1);
        QVERIFY(scenario->timeConstraints().size() == 0);

        QVERIFY(scenario->getStartTimeNode()->getDate() == 0.);

        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);
        auto start_event = *(scenario->getStartTimeNode()->emplace(
                               scenario->getStartTimeNode()->timeEvents().begin(),
                               e_callback));

        auto end_node = time_node::create();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), e_callback));
        auto constraint = time_constraint::create(mc_callback, start_event, end_event, 1000., 1000., 1000.);

        QVERIFY(end_node->getDate() == 1000.);
    }

    /*! test edition functions */
    void test_edition()
    {
        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

        auto scenario = scenario::create();

        auto start_node = scenario->getStartTimeNode();
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), e_callback));

        auto end_node = time_node::create();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), e_callback));

        auto constraint = time_constraint::create(mc_callback, start_event, end_event, 1000., 1000., 1000.);

        scenario->addTimeConstraint(constraint);
        QVERIFY(scenario->timeConstraints().size() == 1);
        QVERIFY(scenario->timeNodes().size() == 2);

        scenario->removeTimeConstraint(constraint);
        QVERIFY(scenario->timeConstraints().size() == 0);
        QVERIFY(scenario->timeNodes().size() == 2);

        auto lonely_node = time_node::create();

        scenario->addTimeNode(lonely_node);
        QVERIFY(scenario->timeNodes().size() == 3);

        scenario->removeTimeNode(lonely_node);
        QVERIFY(scenario->timeNodes().size() == 2);
    }

    /*! test execution functions */
    //! \todo maybe a way to test many scenario would be to load them from a files
    void test_execution()
    {
        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto fc_callback = std::bind(&ScenarioTest::first_constraint_callback, this, _1, _2, _3);
        auto sc_callback = std::bind(&ScenarioTest::second_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

        auto main_start_node = time_node::create();
        auto main_end_node = time_node::create();
        auto main_start_event = *(main_start_node->emplace(main_start_node->timeEvents().begin(), e_callback));
        auto main_end_event = *(main_end_node->emplace(main_end_node->timeEvents().begin(), e_callback));
        main_constraint = time_constraint::create(mc_callback, main_start_event, main_end_event, 5000., 5000, 5000.);

        auto main_scenario = scenario::create();

        main_constraint->addTimeProcess(main_scenario);

        auto scenario_start_node = main_scenario->getStartTimeNode();

        auto first_end_node = time_node::create();
        auto first_start_event = *(scenario_start_node->emplace(scenario_start_node->timeEvents().begin(), e_callback));
        auto first_end_event = *(first_end_node->emplace(first_end_node->timeEvents().begin(), e_callback));
        auto first_constraint = time_constraint::create(fc_callback, first_start_event, first_end_event, 1500., 1500., 1500.);

        main_scenario->addTimeConstraint(first_constraint);

        auto second_end_node = time_node::create();
        auto second_end_event = *(second_end_node->emplace(second_end_node->timeEvents().begin(), e_callback));
        auto second_constraint = time_constraint::create(sc_callback, first_end_event, second_end_event, 2000., 2000., 2000.);

        main_scenario->addTimeConstraint(second_constraint);

        main_constraint->setSpeed(1.);
        main_constraint->setGranularity(50.);
        first_constraint->setSpeed(1.);
        first_constraint->setGranularity(25.);
        second_constraint->setSpeed(1.);
        second_constraint->setGranularity(25.);

        events_date.clear();
        main_constraint->start();

        while (main_constraint->getRunning())
            ;

        // check TimeEvents date
        QCOMPARE(events_date.size(), 3ul);
        QVERIFY(events_date[0] == Zero);
        QVERIFY(events_date[1] >= first_end_node->getDate());
        // todo QVERIFY(events_date[1] < (first_end_node->getDate() + main_constraint->getGranularity()));
        QVERIFY(events_date[2] >= first_end_node->getDate());
        // todo QVERIFY(events_date[2] < first_end_node->getDate() + main_constraint->getGranularity());
    }
};

QTEST_APPLESS_MAIN(ScenarioTest)

#include "ScenarioTest.moc"
