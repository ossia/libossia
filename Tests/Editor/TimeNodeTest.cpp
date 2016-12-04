#include <QtTest>
#include <ossia/ossia.hpp>
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
        auto node = std::make_shared<time_node>();
        QVERIFY(node != nullptr);

        time_value date = node->getDate();
        QVERIFY(date == Zero);

        QVERIFY(node->getExpression() == expressions::expression_true);

        auto expression = expressions::make_expression_true();
        auto& expr_ref = *expression;
        node->setExpression(std::move(expression));

        QVERIFY(node->getExpression() == expr_ref);

        time_value simultaneity_margin = node->getSimultaneityMargin();
        QVERIFY(simultaneity_margin == Zero);

        node->setSimultaneityMargin(50._tv);
        simultaneity_margin = node->getSimultaneityMargin();
        QVERIFY(simultaneity_margin == time_value(50.));

        QVERIFY(node->timeEvents().size() == 0);

        //! \todo test clone()
    }

    /*! test edition functions */
    void test_edition()
    {
        auto node = std::make_shared<time_node>();
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
