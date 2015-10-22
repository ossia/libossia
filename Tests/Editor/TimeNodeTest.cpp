#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

void event_callback(TimeEvent::Status newStatus)
{
    ;
}

class TimeNodeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto node = TimeNode::create();
        QVERIFY(node != nullptr);
        
        TimeValue date = node->getDate();
        QVERIFY(date == Zero);

        QVERIFY(*node->getExpression() == *ExpressionFalse);

        auto expression = Expression::create();
        node->setExpression(expression);

        QVERIFY(*node->getExpression() == *expression);
        
        TimeValue simultaneity_margin = node->getSimultaneityMargin();
        QVERIFY(simultaneity_margin == Zero);
        
        node->setSimultaneityMargin(50.);
        simultaneity_margin = node->getSimultaneityMargin();
        QVERIFY(simultaneity_margin == TimeValue(50.));
        
        QVERIFY(node->timeEvents().size() == 0);

        //! \todo test clone()
    }
    
    /*! test edition functions */
    void test_edition()
    {
        auto node = TimeNode::create();
        QVERIFY(node != nullptr);
        
        auto event = *(node->emplace(node->timeEvents().begin(), &event_callback));
        QVERIFY(event != nullptr);
        
        QVERIFY(node->timeEvents().size() == 1);
    }
};

QTEST_APPLESS_MAIN(TimeNodeTest)

#include "TimeNodeTest.moc"
