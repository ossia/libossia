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
        QVERIFY(expression_default->evaluate() == false);

        auto expression_false = Expression::create(false);
        QVERIFY(expression_false != nullptr);
        QVERIFY(expression_false->evaluate() == false);

        auto expression_true = Expression::create(true);
        QVERIFY(expression_true != nullptr);
        QVERIFY(expression_true->evaluate() == true);

        QVERIFY(ExpressionFalse != nullptr);
        QVERIFY(ExpressionFalse->evaluate() == false);

        QVERIFY(ExpressionTrue != nullptr);
        QVERIFY(ExpressionTrue->evaluate() == true);
    }
};

QTEST_APPLESS_MAIN(ExpressionTest)

#include "ExpressionTest.moc"
