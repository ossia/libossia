// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

void constraint_callback(double position, time_value date, const ossia::state_element& element)
{
    ;
}

void event_callback(time_event::status newStatus)
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
        auto node = std::make_shared<time_node>();
        auto event = *(node->emplace(node->get_time_events().begin(), &event_callback));
        QVERIFY(event != nullptr);

        QVERIFY(&event->get_time_node() == node.get());
        QVERIFY(event->get_expression() == expressions::expression_true());
        QVERIFY(event->get_status() == time_event::status::NONE);

        auto event_with_expression = *(node->emplace(node->get_time_events().begin(), &event_callback, expressions::make_expression_false()));
        QVERIFY(event_with_expression != nullptr);

        QVERIFY(event_with_expression->get_expression() == expressions::expression_false());

        auto expression = expressions::make_expression_true();
        auto& expr_ref = *expression;
        event_with_expression->set_expression(std::move(expression));
        QVERIFY(event_with_expression->get_expression() == expr_ref);

        //! \todo test clone()
    }

    /*! test edition functions */
    void test_edition()
    {
        auto nodeA = std::make_shared<time_node>();
        auto eventA = *(nodeA->emplace(nodeA->get_time_events().begin(), &event_callback));

        auto nodeB = std::make_shared<time_node>();
        auto eventB = *(nodeB->emplace(nodeB->get_time_events().begin(), &event_callback));

        auto nodeC = std::make_shared<time_node>();
        auto eventC = *(nodeC->emplace(nodeC->get_time_events().begin(), &event_callback));

        auto constraint1 = time_constraint::create(&constraint_callback, *eventA, *eventB, 1000._tv);
        auto constraint2 = time_constraint::create(&constraint_callback, *eventB, *eventC, 1000._tv);
        auto constraint3 = time_constraint::create(&constraint_callback, *eventA, *eventC, 2000._tv);

        QVERIFY(eventA->previous_time_constraints().size() == 0);
        QVERIFY(eventA->next_time_constraints().size() == 2);

        QVERIFY(eventB->previous_time_constraints().size() == 1);
        QVERIFY(eventB->next_time_constraints().size() == 1);

        QVERIFY(eventC->previous_time_constraints().size() == 2);
        QVERIFY(eventC->next_time_constraints().size() == 0);
    }
};

QTEST_APPLESS_MAIN(TimeEventTest)

#include "TimeEventTest.moc"
