#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class LoopTest : public QObject
{
    Q_OBJECT

    void constraint_callback(time_value position, time_value date, const state& element)
    {
        element.launch();
    }

    void event_callback(time_event::Status newStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto constraint_callback = std::bind(&LoopTest::constraint_callback, this, _1, _2, _3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, _1);

        loop l(25., constraint_callback, event_callback, event_callback);

        QVERIFY(l.parent() == nullptr);

        QVERIFY(l.getPatternTimeConstraint() != nullptr);
        QVERIFY(l.getPatternStartTimeNode() != nullptr);
        QVERIFY(l.getPatternEndTimeNode() != nullptr);

        //! \todo test clone()
    }

    /*! test execution functions */
    //! \todo test state()
    void test_execution()
    {
        auto constraint_callback = std::bind(&LoopTest::constraint_callback, this, _1, _2, _3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, _1);

        auto start_node = std::make_shared<time_node>();
        auto end_node = std::make_shared<time_node>();

        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));

        auto constraint = time_constraint::create(constraint_callback, *start_event, *end_event, 100., 100., 100.);

        constraint->addTimeProcess(
              std::make_unique<loop>(25., constraint_callback, event_callback, event_callback));

        constraint->start();

        while (constraint->getRunning())
            ;
    }
};

QTEST_APPLESS_MAIN(LoopTest)

#include "LoopTest.moc"
