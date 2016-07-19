#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;
using namespace std::placeholders;

class ExpressionNotTest : public QObject
{
    Q_OBJECT

    bool m_result;
    bool m_result_callback_called;

    void result_callback(bool result)
    {
        m_result = result;
        m_result_callback_called = true;
    }

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto expression = ExpressionAtom::create(new Bool(false),
                                                 ExpressionAtom::Operator::DIFFERENT,
                                                 new Bool(false));

        auto not_expression = ExpressionNot::create(expression);
        QVERIFY(not_expression != nullptr);
        QVERIFY(not_expression->getType() == Expression::Type::NOT);
        QVERIFY(not_expression->evaluate() == true);
    }

    /*! test comparison operator */
    void test_comparison()
    {
        auto exprA = ExpressionAtom::create(new Bool(true),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(true));

        auto exprB = ExpressionAtom::create(new Bool(false),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(false));

        auto exprC = ExpressionAtom::create(new Bool(true),
                                            ExpressionAtom::Operator::EQUAL,
                                            new Bool(true));

        auto not_exprA = ExpressionNot::create(exprA);
        auto not_exprB = ExpressionNot::create(exprB);
        auto not_exprC = ExpressionNot::create(exprC);

        QVERIFY(*ExpressionFalse != *not_exprA);
        QVERIFY(*ExpressionTrue != *not_exprA);

        QVERIFY(*not_exprA != *not_exprB);
        QVERIFY(*not_exprA == *not_exprC);
        QVERIFY(*not_exprB != *not_exprC);
    }

    /*! test callback managment */
    void test_callback()
    {
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        auto localIntNode1 = *(device->emplace(device->children().cend(), "my_int.1"));
        auto localIntAddress1 = localIntNode1->createAddress(Type::INT);
        auto localIntNode2 = *(device->emplace(device->children().cend(), "my_int.2"));
        auto localIntAddress2 = localIntNode2->createAddress(Type::INT);

        auto testDestinationExpr = ExpressionAtom::create(new Destination(localIntNode1),
                                                          ExpressionAtom::Operator::DIFFERENT,
                                                          new Destination(localIntNode2));

        auto testDestinationExprNot = ExpressionNot::create(testDestinationExpr);

        ResultCallback callback = std::bind(&ExpressionNotTest::result_callback, this, _1);
        auto callback_index = testDestinationExprNot->addCallback(callback);

        QVERIFY(testDestinationExprNot->callbacks().size() == 1);

        m_result = false;
        m_result_callback_called = false;

        Int i1(5);
        localIntAddress1->pushValue(&i1);

        QVERIFY(m_result_callback_called == true && m_result == false);

        m_result = false;
        m_result_callback_called = false;

        Int i2(5);
        localIntAddress2->pushValue(&i2);

        QVERIFY(m_result_callback_called == true && m_result == true);

        m_result = false;
        m_result_callback_called = false;

        Int i3(10);
        localIntAddress2->pushValue(&i3);

        QVERIFY(m_result_callback_called == true && m_result == false);

        testDestinationExprNot->removeCallback(callback_index);

        QVERIFY(testDestinationExprNot->callbacks().size() == 0);

        m_result = false;
        m_result_callback_called = false;

        localIntAddress2->pushValue(&i2);

        QVERIFY(m_result_callback_called == false && m_result == false);
    }
};

QTEST_APPLESS_MAIN(ExpressionNotTest)

#include "ExpressionNotTest.moc"
