#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

class ExpressionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto expression_default = Expression::create();
        QVERIFY(expression_default != nullptr);
        QVERIFY(expression_default->getType() == Expression::Type::BASE);
        QVERIFY(expression_evaluate(default) == false);

        auto expression_false = Expression::create(false);
        QVERIFY(expression_false != nullptr);
        QVERIFY(expression_false->getType() == Expression::Type::BASE);
        QVERIFY(expression_evaluate(false) == false);

        auto expression_true = Expression::create(true);
        QVERIFY(expression_true != nullptr);
        QVERIFY(expression_true->getType() == Expression::Type::BASE);
        QVERIFY(expression_evaluate(true) == true);

        QVERIFY(ExpressionFalse != nullptr);
        QVERIFY(ExpressionFalse->getType() == Expression::Type::BASE);
        QVERIFY(evaluate(ExpressionFalse) == false);

        QVERIFY(ExpressionTrue != nullptr);
        QVERIFY(ExpressionTrue->getType() == Expression::Type::BASE);
        QVERIFY(evaluate(ExpressionTrue) == true);
    }

    /*! test comparison operator */
    void test_comparison()
    {
        QVERIFY(expressions::expression_false == expressions::expression_false);
        QVERIFY(expressions::expression_false != expressions::expression_true);
        QVERIFY(expressions::expression_true != expressions::expression_false);
        QVERIFY(expressions::expression_true == expressions::expression_true);

        auto expression_false = Expression::create(false);
        auto expression_true = Expression::create(true);

        QVERIFY(expressions::expression_false == *expression_false);
        QVERIFY(expressions::expression_false != *expression_true);

        QVERIFY(expressions::expression_true == *expression_true);
        QVERIFY(expressions::expression_true != *expression_false);
    }
};

QTEST_APPLESS_MAIN(ExpressionTest)

#include "ExpressionTest.moc"
