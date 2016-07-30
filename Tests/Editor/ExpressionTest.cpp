#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::expressions;

class ExpressionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto expr_default = make_expression_bool(false);
        QVERIFY(expr_default != nullptr);
        QVERIFY(evaluate(expr_default) == false);

        auto expr_false = make_expression_bool(false);
        QVERIFY(expr_false != nullptr);
        QVERIFY(evaluate(expr_false) == false);

        auto expr_true = make_expression_bool(true);
        QVERIFY(expr_true != nullptr);
        QVERIFY(evaluate(expr_true) == true);

        QVERIFY(evaluate(expression_false) == false);
        QVERIFY(evaluate(expression_true) == true);
    }

    /*! test comparison operator */
    void test_comparison()
    {
        QVERIFY(expressions::expression_false == expressions::expression_false);
        QVERIFY(expressions::expression_false != expressions::expression_true);
        QVERIFY(expressions::expression_true != expressions::expression_false);
        QVERIFY(expressions::expression_true == expressions::expression_true);

        auto expression_false = make_expression_bool(false);
        auto expression_true = make_expression_bool(true);

        QVERIFY(expressions::expression_false == *expression_false);
        QVERIFY(expressions::expression_false != *expression_true);

        QVERIFY(expressions::expression_true == *expression_true);
        QVERIFY(expressions::expression_true != *expression_false);
    }
};

QTEST_APPLESS_MAIN(ExpressionTest)

#include "ExpressionTest.moc"
