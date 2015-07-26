#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

void constraint_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<State> state)
{
    ;
}

void event_callback(TimeEvent::Status newStatus)
{
    ;
}

class TimeEventTest : public QObject
{
    Q_OBJECT
    
private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto node = TimeNode::create();
        auto event = *(node->emplace(node->timeEvents().begin(), &event_callback));
        QVERIFY(event != nullptr);
        
        QVERIFY(event->getTimeNode() == node);
        QVERIFY(event->getState() != nullptr);
        QVERIFY(event->getExpression() == nullptr);
        QVERIFY(event->getStatus() == TimeEvent::Status::NONE);

        auto event_with_expression = *(node->emplace(node->timeEvents().begin(), &event_callback, ExpressionTrue));
        QVERIFY(event_with_expression != nullptr);

        QVERIFY(event_with_expression->getExpression() == ExpressionTrue);

        //! \todo test clone()
    }
    
    /*! test edition functions */
    void test_edition()
    {
        auto nodeA = TimeNode::create();
        auto eventA = *(nodeA->emplace(nodeA->timeEvents().begin(), &event_callback));
        
        auto nodeB = TimeNode::create();
        auto eventB = *(nodeB->emplace(nodeB->timeEvents().begin(), &event_callback));
        
        auto nodeC = TimeNode::create();
        auto eventC = *(nodeC->emplace(nodeC->timeEvents().begin(), &event_callback));
        
        auto constraint1 = TimeConstraint::create(&constraint_callback, eventA, eventB, 1000.);
        auto constraint2 = TimeConstraint::create(&constraint_callback, eventB, eventC, 1000.);
        auto constraint3 = TimeConstraint::create(&constraint_callback, eventA, eventC, 2000.);
        
        QVERIFY(eventA->previousTimeConstraints().size() == 0);
        QVERIFY(eventA->nextTimeConstraints().size() == 2);
        
        QVERIFY(eventB->previousTimeConstraints().size() == 1);
        QVERIFY(eventB->nextTimeConstraints().size() == 1);
        
        QVERIFY(eventC->previousTimeConstraints().size() == 2);
        QVERIFY(eventC->nextTimeConstraints().size() == 0);
    }
};

QTEST_APPLESS_MAIN(TimeEventTest)

#include "TimeEventTest.moc"
