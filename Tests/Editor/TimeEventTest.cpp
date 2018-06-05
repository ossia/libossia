// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <QtTest>
#include <iostream>

using namespace ossia;

class TimeEventTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto node = std::make_shared<time_sync>();
        auto event = *(node->emplace(node->get_time_events().begin(), time_event::exec_callback{}));
        QVERIFY(event != nullptr);

        QVERIFY(&event->get_time_sync() == node.get());
        QVERIFY(event->get_expression() == expressions::expression_true());
        QVERIFY(event->get_status() == time_event::status::NONE);

        auto event_with_expression = *(node->emplace(node->get_time_events().begin(), time_event::exec_callback{}, expressions::make_expression_false()));
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
        auto eventA = *(nodeA->emplace(nodeA->get_time_events().begin(), time_event::exec_callback{}));

        auto nodeB = std::make_shared<time_sync>();
        auto eventB = *(nodeB->emplace(nodeB->get_time_events().begin(), time_event::exec_callback{}));

        auto nodeC = std::make_shared<time_sync>();
        auto eventC = *(nodeC->emplace(nodeC->get_time_events().begin(), time_event::exec_callback{}));

        auto interval1 = time_interval::create(ossia::time_interval::exec_callback{}, *eventA, *eventB, 1000_tv);
        auto interval2 = time_interval::create(ossia::time_interval::exec_callback{}, *eventB, *eventC, 1000_tv);
        auto interval3 = time_interval::create(ossia::time_interval::exec_callback{}, *eventA, *eventC, 2000_tv);


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
