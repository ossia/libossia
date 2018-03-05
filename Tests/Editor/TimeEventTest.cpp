// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

void interval_callback(double position, ossia::time_value date)
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
        auto node = std::make_shared<time_sync>();
        auto event = *(node->emplace(node->get_time_events().begin(), &event_callback));
        QVERIFY(event != nullptr);

        QVERIFY(&event->get_time_sync() == node.get());
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
        auto nodeA = std::make_shared<time_sync>();
        auto eventA = *(nodeA->emplace(nodeA->get_time_events().begin(), &event_callback));

        auto nodeB = std::make_shared<time_sync>();
        auto eventB = *(nodeB->emplace(nodeB->get_time_events().begin(), &event_callback));

        auto nodeC = std::make_shared<time_sync>();
        auto eventC = *(nodeC->emplace(nodeC->get_time_events().begin(), &event_callback));

        auto interval1 = time_interval::create({[] (auto&&... args) { interval_callback(args...); }}, *eventA, *eventB, 1000._tv);
        auto interval2 = time_interval::create({[] (auto&&... args) { interval_callback(args...); }}, *eventB, *eventC, 1000._tv);
        auto interval3 = time_interval::create({[] (auto&&... args) { interval_callback(args...); }}, *eventA, *eventC, 2000._tv);

        QVERIFY(eventA->previous_time_intervals().size() == 0);
        QVERIFY(eventA->next_time_intervals().size() == 2);

        QVERIFY(eventB->previous_time_intervals().size() == 1);
        QVERIFY(eventB->next_time_intervals().size() == 1);

        QVERIFY(eventC->previous_time_intervals().size() == 2);
        QVERIFY(eventC->next_time_intervals().size() == 0);
    }
};

QTEST_APPLESS_MAIN(TimeEventTest)

#include "TimeEventTest.moc"
