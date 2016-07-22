#include <QtTest>
#include "../ForwardDeclaration.h"
#include <functional>
#include <iostream>

using namespace OSSIA;
using namespace std::placeholders;

class LoopTest : public QObject
{
    Q_OBJECT

    void constraint_callback(TimeValue position, TimeValue date, std::shared_ptr<StateElement> element)
    {
        element->launch();
    }

    void event_callback(TimeEvent::Status newStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto constraint_callback = std::bind(&LoopTest::constraint_callback, this, _1, _2, _3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, _1);
        
        auto loop = Loop::create(25., constraint_callback, event_callback, event_callback);
        QVERIFY(loop != nullptr);

        QVERIFY(loop->parent == nullptr);

        QVERIFY(loop->getPatternTimeConstraint() != nullptr);
        QVERIFY(loop->getPatternStartTimeNode() != nullptr);
        QVERIFY(loop->getPatternEndTimeNode() != nullptr);

        //! \todo test clone()
    }

    /*! test execution functions */
    //! \todo test state()
    void test_execution()
    {
        auto constraint_callback = std::bind(&LoopTest::constraint_callback, this, _1, _2, _3);
        auto event_callback = std::bind(&LoopTest::event_callback, this, _1);
        
        auto start_node = TimeNode::create();
        auto end_node = TimeNode::create();
        
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
        
        auto constraint = TimeConstraint::create(constraint_callback, start_event, end_event, 100., 100., 100.);
        
        auto loop = Loop::create(25., constraint_callback, event_callback, event_callback);

        constraint->addTimeProcess(loop);
        constraint->start();

        while (constraint->getRunning())
            ;
    }
};

QTEST_APPLESS_MAIN(LoopTest)

#include "LoopTest.moc"
