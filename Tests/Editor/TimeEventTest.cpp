#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

void constraint_callback(time_value position, time_value date, const ossia::state& element)
{
    ;
}

void event_callback(time_event::Status newStatus)
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
        auto node = time_node::create();
        auto event = *(node->emplace(node->timeEvents().begin(), &event_callback));
        QVERIFY(event != nullptr);

        QVERIFY(event->getTimeNode() == node);
        QVERIFY(event->getExpression() == expressions::expression_true);
        QVERIFY(event->getStatus() == time_event::Status::NONE);

        auto event_with_expression = *(node->emplace(node->timeEvents().begin(), &event_callback, expressions::make_expression_false()));
        QVERIFY(event_with_expression != nullptr);

        QVERIFY(event_with_expression->getExpression() == expressions::expression_false);

        auto expression = expressions::make_expression_true();
        auto& expr_ref = *expression;
        event_with_expression->setExpression(std::move(expression));
        QVERIFY(event_with_expression->getExpression() == expr_ref);

        //! \todo test clone()
    }

    /*! test edition functions */
    void test_edition()
    {
        auto nodeA = time_node::create();
        auto eventA = *(nodeA->emplace(nodeA->timeEvents().begin(), &event_callback));

        auto nodeB = time_node::create();
        auto eventB = *(nodeB->emplace(nodeB->timeEvents().begin(), &event_callback));

        auto nodeC = time_node::create();
        auto eventC = *(nodeC->emplace(nodeC->timeEvents().begin(), &event_callback));

        auto constraint1 = time_constraint::create(&constraint_callback, eventA, eventB, 1000.);
        auto constraint2 = time_constraint::create(&constraint_callback, eventB, eventC, 1000.);
        auto constraint3 = time_constraint::create(&constraint_callback, eventA, eventC, 2000.);

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
