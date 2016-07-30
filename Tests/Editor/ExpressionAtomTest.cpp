#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::expressions;
using namespace std::placeholders;

class ExpressionAtomTest : public QObject
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

    /*! evaluate expressions with impulse values */
    void test_impulse()
    {
        // evaluate expressions with Impulse
        auto testImpulseExprA = make_expression_atom(
                    Impulse(), expression_atom::Comparator::EQUAL, Impulse());

        QVERIFY(evaluate(testImpulseExprA) == true);

        auto testImpulseExprB = make_expression_atom(
                    Impulse(), expression_atom::Comparator::DIFFERENT, Impulse());

        QVERIFY(evaluate(testImpulseExprB) == false);

        auto testImpulseExprC = make_expression_atom(
                    Impulse(), expression_atom::Comparator::EQUAL, Bool(true));
        QCOMPARE(testImpulseExprC->target<expression_atom>()->getFirstOperand().getType(), Type::IMPULSE);
        QCOMPARE(testImpulseExprC->target<expression_atom>()->getSecondOperand().getType(), Type::BOOL);

        QVERIFY(evaluate(testImpulseExprC) == true);

        auto testImpulseExprD = make_expression_atom(
                    Impulse(), expression_atom::Comparator::EQUAL, Int());

        QVERIFY(evaluate(testImpulseExprD) == true);

        auto testImpulseExprE = make_expression_atom(
                    Impulse(), expression_atom::Comparator::EQUAL, Float());

        QVERIFY(evaluate(testImpulseExprE) == true);

        auto testImpulseExprF = make_expression_atom(
                    Impulse(), expression_atom::Comparator::EQUAL, Char());

        QVERIFY(evaluate(testImpulseExprF) == true);

        auto testImpulseExprG = make_expression_atom(
                    Impulse(), expression_atom::Comparator::EQUAL, String());

        QVERIFY(evaluate(testImpulseExprG) == true);

        auto testImpulseExprH = make_expression_atom(
                    Impulse(), expression_atom::Comparator::EQUAL, Tuple());

        QVERIFY(evaluate(testImpulseExprH) == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with bool values */
    void test_bool()
    {
        auto testBoolExprA = make_expression_atom(
                    Bool(true), expression_atom::Comparator::EQUAL, Bool(true));

        QVERIFY(evaluate(testBoolExprA) == true);

        auto testBoolExprB = make_expression_atom(
                    Bool(true), expression_atom::Comparator::DIFFERENT, Bool(true));

        QVERIFY(evaluate(testBoolExprB) == false);

        auto testBoolExprC = make_expression_atom(
                    Bool(true), expression_atom::Comparator::EQUAL, Impulse());

        QVERIFY(evaluate(testBoolExprC) == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with int values */
    void test_int()
    {
        auto testIntExprA = make_expression_atom(Int(10),
                                                   expression_atom::Comparator::GREATER_THAN,
                                                   Int(5));

        QVERIFY(evaluate(testIntExprA) == true);

        auto testIntExprB = make_expression_atom(Int(10),
                                                   expression_atom::Comparator::LOWER_THAN,
                                                   Int(5));

        QVERIFY(evaluate(testIntExprB) == false);

        auto testIntExprC = make_expression_atom(Int(10),
                                                    expression_atom::Comparator::EQUAL,
                                                    Impulse());

        QVERIFY(evaluate(testIntExprC) == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with float values */
    void test_float()
    {
        auto testFloatExprA = make_expression_atom(Float(10.),
                                                     expression_atom::Comparator::GREATER_THAN_OR_EQUAL,
                                                     Float(10.));

        QVERIFY(evaluate(testFloatExprA) == true);

        auto testFloatExprB = make_expression_atom(Float(10.),
                                                     expression_atom::Comparator::LOWER_THAN_OR_EQUAL,
                                                     Float(10.));

        QVERIFY(evaluate(testFloatExprB) == true);

        auto testFloatExprC = make_expression_atom(Float(10.),
                                                     expression_atom::Comparator::EQUAL,
                                                     Impulse());

        QVERIFY(evaluate(testFloatExprC) == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with string values */
    void test_string()
    {
        auto testStringExprA = make_expression_atom(String("abc"),
                                                      expression_atom::Comparator::GREATER_THAN_OR_EQUAL,
                                                      String("bcd"));

        QVERIFY(evaluate(testStringExprA) == false);

        auto testStringExprB = make_expression_atom(String("abc"),
                                                      expression_atom::Comparator::LOWER_THAN_OR_EQUAL,
                                                      String("bcd"));

        QVERIFY(evaluate(testStringExprB) == true);

        auto testStringExprC = make_expression_atom(String("abc"),
                                                      expression_atom::Comparator::EQUAL,
                                                      Impulse());

        QVERIFY(evaluate(testStringExprC) == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with tuple values */
    void test_tuple()
    {
        std::vector<value> value1 = {Float(0.1), Float(0.2), Float(0.3)};
        std::vector<value> value2 = {Float(0.2), Float(0.3), Float(0.4)};

        auto testTupleExprA = make_expression_atom(Tuple(value1),
                                                     expression_atom::Comparator::GREATER_THAN,
                                                     Tuple(value2));

        QVERIFY(evaluate(testTupleExprA) == false);

        auto testTupleExprB = make_expression_atom(Tuple(value1),
                                                     expression_atom::Comparator::LOWER_THAN,
                                                     Tuple(value2));

        QVERIFY(evaluate(testTupleExprB) == true);

        auto testTupleExprC = make_expression_atom(Tuple(value1),
                                                     expression_atom::Comparator::EQUAL,
                                                     Impulse());

        QVERIFY(evaluate(testTupleExprC) == true);

        //! \todo test clone()
    }

    /*! evaluate expressions with destination values */
    void test_destination()
    {
        /* TODO
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        // Local tree building
        auto localImpulseNode1 = *(device->emplace(device->children().cend(), "my_impulse.1"));
        auto localImpulseAddress1 = localImpulseNode1->createAddress(Type::IMPULSE);
        auto localImpulseNode2 = *(device->emplace(device->children().cend(), "my_impulse.2"));
        auto localImpulseAddress2 = localImpulseNode2->createAddress(Type::IMPULSE);

        auto localBoolNode1 = *(device->emplace(device->children().cend(), "my_bool.1"));
        auto localBoolAddress1 = localBoolNode1->createAddress(Type::BOOL);
        auto localBoolNode2 = *(device->emplace(device->children().cend(), "my_bool.2"));
        auto localBoolAddress2 = localBoolNode2->createAddress(Type::BOOL);

        auto localIntNode1 = *(device->emplace(device->children().cend(), "my_int.1"));
        auto localIntAddress1 = localIntNode1->createAddress(Type::INT);
        auto localIntNode2 = *(device->emplace(device->children().cend(), "my_int.2"));
        auto localIntAddress2 = localIntNode2->createAddress(Type::INT);

        auto localFloatNode1 = *(device->emplace(device->children().cend(), "my_float.1"));
        auto localFloatAddress1 = localFloatNode1->createAddress(Type::FLOAT);
        auto localFloatNode2 = *(device->emplace(device->children().cend(), "my_float.2"));
        auto localFloatAddress2 = localFloatNode2->createAddress(Type::FLOAT);

        auto localStringNode1 = *(device->emplace(device->children().cend(), "my_string.1"));
        auto localStringAddress1 = localStringNode1->createAddress(Type::STRING);
        auto localStringNode2 = *(device->emplace(device->children().cend(), "my_string.2"));
        auto localStringAddress2 = localStringNode2->createAddress(Type::STRING);

        auto localTupleNode1 = *(device->emplace(device->children().cend(), "my_tuple.1"));
        auto localTupleAddress1 = localTupleNode1->createAddress(Type::TUPLE);
        auto localTupleNode2 = *(device->emplace(device->children().cend(), "my_tuple.2"));
        auto localTupleAddress2 = localTupleNode2->createAddress(Type::TUPLE);

        auto localDestinationNode1 = *(device->emplace(device->children().cend(), "my_destination.1"));
        auto localDestinationAddress1 = localDestinationNode1->createAddress(Type::DESTINATION);
        auto localDestinationNode2 = *(device->emplace(device->children().cend(), "my_destination.2"));
        auto localDestinationAddress2 = localDestinationNode2->createAddress(Type::DESTINATION);

        // update node's value
        Bool b1(false);
        localBoolAddress1->setValue(&b1);

        Bool b2(true);
        localBoolAddress2->setValue(&b2);

        Int i1(5);
        localIntAddress1->setValue(&i1);

        Int i2(10);
        localIntAddress2->setValue(&i2);

        Float f1(0.5);
        localFloatAddress1->setValue(&f1);

        Float f2(0.2);
        localFloatAddress2->setValue(&f2);

        String s1("abc");
        localStringAddress1->setValue(&s1);

        String s2("bcd");
        localStringAddress2->setValue(&s2);

        Destination d1(localFloatNode1);
        localDestinationAddress1->setValue(&d1);

        Destination d2(localFloatNode2);
        localDestinationAddress2->setValue(&d2);

        Tuple t1 = {Float(0.1), Float(0.2), Float(0.3)};
        localTupleAddress1->setValue(&t1);

        Tuple t2 = {Float(0.2), Float(0.3), Float(0.4)};
        localTupleAddress2->setValue(&t2);

        // evaluate expressions with Destination
        auto testDestinationExprA = make_expression_atom(new Destination(localImpulseNode1),
                                                          expression_atom::Comparator::EQUAL,
                                                          new Destination(localImpulseNode2));

        QVERIFY(testDestinationExprA->getType() == Expression::Type::ATOM);
        QVERIFY(evaluate(testDestinationExprA) == true);

        auto testDestinationExprB = make_expression_atom(new Destination(localBoolNode1),
                                                           expression_atom::Comparator::EQUAL,
                                                           new Destination(localBoolNode2));

        QVERIFY(evaluate(testDestinationExprB) == false);

        auto testDestinationExprC = make_expression_atom(new Destination(localIntNode1),
                                                           expression_atom::Comparator::DIFFERENT,
                                                           new Destination(localIntNode2));

        QVERIFY(evaluate(testDestinationExprC) == true);

        auto testDestinationExprD = make_expression_atom(new Destination(localFloatNode1),
                                                           expression_atom::Comparator::GREATER_THAN,
                                                           new Destination(localFloatNode2));

        QVERIFY(evaluate(testDestinationExprD) == true);

        auto testDestinationExprE = make_expression_atom(new Destination(localStringNode1),
                                                           expression_atom::Comparator::LOWER_THAN,
                                                           new Destination(localStringNode2));

        QVERIFY(evaluate(testDestinationExprE) == true);

        auto testDestinationExprF = make_expression_atom(new Destination(localDestinationNode1),
                                                           expression_atom::Comparator::GREATER_THAN_OR_EQUAL,
                                                           new Destination(localDestinationNode2));

        QVERIFY(evaluate(testDestinationExprF) == true);

        auto testDestinationExprG = make_expression_atom(new Destination(localTupleNode1),
                                                           expression_atom::Comparator::LOWER_THAN_OR_EQUAL,
                                                           new Destination(localTupleNode2));

        QVERIFY(evaluate(testDestinationExprG) == true);

        //! \todo test clone()
        */
    }

    /*! test comparison operator */
    void test_comparison()
    {
        auto testExprA = make_expression_atom(Bool(true),
                                                    expression_atom::Comparator::EQUAL,
                                                    Bool(true));

        auto testExprB = make_expression_atom(Bool(true),
                                                    expression_atom::Comparator::DIFFERENT,
                                                    Bool(true));

        auto testExprC = make_expression_atom(Bool(true),
                                                expression_atom::Comparator::EQUAL,
                                                Bool(true));

        QVERIFY(expression_false != *testExprA);
        QVERIFY(expression_true != *testExprA);

        QVERIFY(*testExprA != *testExprB);
        QVERIFY(*testExprA == *testExprC);
        QVERIFY(*testExprB != *testExprC);
    }

    /*! test callback managment */
    void test_callback()
    {
        /* TODO
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        auto localIntNode1 = *(device->emplace(device->children().cend(), "my_int.1"));
        auto localIntAddress1 = localIntNode1->createAddress(Type::INT);
        auto localIntNode2 = *(device->emplace(device->children().cend(), "my_int.2"));
        auto localIntAddress2 = localIntNode2->createAddress(Type::INT);

        auto testDestinationExpr = make_expression_atom(new Destination(localIntNode1),
                                                           expression_atom::Comparator::EQUAL,
                                                           new Destination(localIntNode2));

        auto callback = [&] (bool b) { result_callback(b); };
        auto callback_index = testDestinationExpr->addCallback(callback);

        QVERIFY(testDestinationExpr->callbacks().size() == 1);

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

        testDestinationExpr->removeCallback(callback_index);

        QVERIFY(testDestinationExpr->callbacks().size() == 0);

        m_result = false;
        m_result_callback_called = false;

        Int i3(10);
        localIntAddress2->pushValue(&i3);

        QVERIFY(m_result_callback_called == false && m_result == false);
        */
    }
};

QTEST_APPLESS_MAIN(ExpressionAtomTest)

#include "ExpressionAtomTest.moc"
