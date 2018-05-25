// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

void event_callback(time_event::status newStatus)
{
    ;
}

class TimeSyncTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto node = std::make_shared<time_sync>();
        QVERIFY(node != nullptr);

        ossia::time_value date = node->get_date();
        QVERIFY(date == Zero);

        QVERIFY(node->get_expression() == expressions::expression_true());

        auto expression = expressions::make_expression_true();
        auto& expr_ref = *expression;
        node->set_expression(std::move(expression));

        QVERIFY(node->get_expression() == expr_ref);

        QVERIFY(node->get_time_events().size() == 0);

        //! \todo test clone()
    }

    /*! test edition functions */
    void test_edition()
    {
        auto node = std::make_shared<time_sync>();
        QVERIFY(node != nullptr);

        auto event = *(node->emplace(
                           node->get_time_events().begin(),
                           &event_callback, expressions::make_expression_true()));

        QVERIFY(event != nullptr);

        QVERIFY(node->get_time_events().size() == 1);
    }
};

QTEST_APPLESS_MAIN(TimeSyncTest)

#include "TimeSyncTest.moc"
