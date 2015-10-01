#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

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
        QVERIFY(expression_default->evaluate() == false);

        auto expression_false = Expression::create(false);
        QVERIFY(expression_false != nullptr);
        QVERIFY(expression_false->getType() == Expression::Type::BASE);
        QVERIFY(expression_false->evaluate() == false);

        auto expression_true = Expression::create(true);
        QVERIFY(expression_true != nullptr);
        QVERIFY(expression_true->getType() == Expression::Type::BASE);
        QVERIFY(expression_true->evaluate() == true);

        QVERIFY(ExpressionFalse != nullptr);
        QVERIFY(ExpressionFalse->getType() == Expression::Type::BASE);
        QVERIFY(ExpressionFalse->evaluate() == false);

        QVERIFY(ExpressionTrue != nullptr);
        QVERIFY(ExpressionTrue->getType() == Expression::Type::BASE);
        QVERIFY(ExpressionTrue->evaluate() == true);
    }

    /*! test comparison operator */
    void test_comparison()
    {
        QVERIFY(*ExpressionFalse == *ExpressionFalse);
        QVERIFY(*ExpressionFalse != *ExpressionTrue);
        QVERIFY(*ExpressionTrue != *ExpressionFalse);
        QVERIFY(*ExpressionTrue == *ExpressionTrue);

        auto expression_false = Expression::create(false);
        auto expression_true = Expression::create(true);

        QVERIFY(*ExpressionFalse == *expression_false);
        QVERIFY(*ExpressionFalse != *expression_true);

        QVERIFY(*ExpressionTrue == *expression_true);
        QVERIFY(*ExpressionTrue != *expression_false);
    }
};

QTEST_APPLESS_MAIN(ExpressionTest)

#include "ExpressionTest.moc"
