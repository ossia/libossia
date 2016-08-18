#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::expressions;
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
        auto expression = make_expression_atom(Bool(false),
                                                 expression_atom::Comparator::DIFFERENT,
                                                 Bool(false));

        auto not_expression = make_expression_not(std::move(expression));
        QVERIFY(not_expression != nullptr);
        QVERIFY(evaluate(not_expression) == true);
    }

    /*! test comparison operator */
    void test_comparison()
    {
        auto exprA = make_expression_atom(Bool(true),
                                            expression_atom::Comparator::EQUAL,
                                            Bool(true));

        auto exprB = make_expression_atom(Bool(false),
                                            expression_atom::Comparator::EQUAL,
                                            Bool(false));

        auto exprC = make_expression_atom(Bool(true),
                                            expression_atom::Comparator::EQUAL,
                                            Bool(true));

        auto not_exprA = make_expression_not(std::move(exprA));
        auto not_exprB = make_expression_not(std::move(exprB));
        auto not_exprC = make_expression_not(std::move(exprC));

        QVERIFY(expressions::expression_false != *not_exprA);
        QVERIFY(expressions::expression_true != *not_exprA);

        QVERIFY(*not_exprA != *not_exprB);
        QVERIFY(*not_exprA == *not_exprC);
        QVERIFY(*not_exprB != *not_exprC);
    }

    /*! test callback managment */
    void test_callback()
    {
        // Local device
        ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};

        auto localIntNode1 = device.createChild("my_int.1");
        auto localIntAddress1 = localIntNode1->createAddress(val_type::INT);
        auto localIntNode2 = device.createChild("my_int.2");
        auto localIntAddress2 = localIntNode2->createAddress(val_type::INT);

        auto testDestinationExpr = make_expression_atom(Destination(*localIntAddress1),
                                                          expression_atom::Comparator::DIFFERENT,
                                                          Destination(*localIntAddress2));

        auto testDestinationExprNot = make_expression_not(std::move(testDestinationExpr));

        expression_result_callback callback = std::bind(&ExpressionNotTest::result_callback, this, _1);
        auto callback_index = add_callback(*testDestinationExprNot, callback);

        QVERIFY(callback_count(*testDestinationExprNot) == 1);

        m_result = false;
        m_result_callback_called = false;

        Int i1(5);
        localIntAddress1->pushValue(i1);

        QVERIFY(m_result_callback_called == true && m_result == false);

        m_result = false;
        m_result_callback_called = false;

        Int i2(5);
        localIntAddress2->pushValue(i2);

        QVERIFY(m_result_callback_called == true && m_result == true);

        m_result = false;
        m_result_callback_called = false;

        Int i3(10);
        localIntAddress2->pushValue(i3);

        QVERIFY(m_result_callback_called == true && m_result == false);

        remove_callback(*testDestinationExprNot, callback_index);

        QVERIFY(callback_count(*testDestinationExprNot) == 0);

        m_result = false;
        m_result_callback_called = false;

        localIntAddress2->pushValue(i2);

        QVERIFY(m_result_callback_called == false && m_result == false);
    }
};

QTEST_APPLESS_MAIN(ExpressionNotTest)

#include "ExpressionNotTest.moc"
