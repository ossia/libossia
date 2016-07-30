#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

void event_callback(time_event::Status newStatus)
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
        auto node = time_node::create();
        QVERIFY(node != nullptr);

        time_value date = node->getDate();
        QVERIFY(date == Zero);

        QVERIFY(node->getExpression() == expressions::expression_true);

        auto expression = expressions::make_expression_bool(true);
        auto& expr_ref = *expression;
        node->setExpression(std::move(expression));

        QVERIFY(node->getExpression() == expr_ref);

        time_value simultaneity_margin = node->getSimultaneityMargin();
        QVERIFY(simultaneity_margin == Zero);

        node->setSimultaneityMargin(50.);
        simultaneity_margin = node->getSimultaneityMargin();
        QVERIFY(simultaneity_margin == time_value(50.));

        QVERIFY(node->timeEvents().size() == 0);

        //! \todo test clone()
    }

    /*! test edition functions */
    void test_edition()
    {
        auto node = time_node::create();
        QVERIFY(node != nullptr);

        auto event = *(node->emplace(
                           node->timeEvents().begin(),
                           &event_callback, expressions::make_expression_true()));

        QVERIFY(event != nullptr);

        QVERIFY(node->timeEvents().size() == 1);
    }
};

QTEST_APPLESS_MAIN(TimeNodeTest)

#include "TimeNodeTest.moc"
