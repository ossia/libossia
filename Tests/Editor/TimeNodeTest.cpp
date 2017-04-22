#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

void event_callback(time_event::status newStatus)
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
        auto node = std::make_shared<time_node>();
        QVERIFY(node != nullptr);

        time_value date = node->get_date();
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
        auto node = std::make_shared<time_node>();
        QVERIFY(node != nullptr);

        auto event = *(node->emplace(
                           node->get_time_events().begin(),
                           &event_callback, expressions::make_expression_true()));

        QVERIFY(event != nullptr);

        QVERIFY(node->get_time_events().size() == 1);
    }
};

QTEST_APPLESS_MAIN(TimeNodeTest)

#include "TimeNodeTest.moc"
