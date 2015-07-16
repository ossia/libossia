#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class ExpressionNotTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto expression = ExpressionAtom::create(new Bool(false),
                                                 ExpressionAtom::Operator::DIFFERENT,
                                                 new Bool(false));

        auto not_expression = ExpressionNot::create(expression);
        QVERIFY(not_expression != nullptr);
        QVERIFY(not_expression->evaluate() == true);
    }
};

QTEST_APPLESS_MAIN(ExpressionNotTest)

#include "ExpressionNotTest.moc"
