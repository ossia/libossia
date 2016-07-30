#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class ExpressionPulseTest : public QObject
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

    /*! evaluate expressions with destination values */
    void test_basic()
    {
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        // Local tree building
        auto localImpulseNode = *(device->emplace(device->children().cend(), "my_impulse"));
        auto localImpulseAddress = localImpulseNode->createAddress(Type::IMPULSE);

        auto localBoolNode = *(device->emplace(device->children().cend(), "my_bool"));
        auto localBoolAddress = localBoolNode->createAddress(Type::BOOL);

        auto localIntNode = *(device->emplace(device->children().cend(), "my_int"));
        auto localIntAddress = localIntNode->createAddress(Type::INT);

        auto localFloatNode = *(device->emplace(device->children().cend(), "my_float"));
        auto localFloatAddress = localFloatNode->createAddress(Type::FLOAT);

        auto localStringNode = *(device->emplace(device->children().cend(), "my_string"));
        auto localStringAddress = localStringNode->createAddress(Type::STRING);

        auto localTupleNode = *(device->emplace(device->children().cend(), "my_tuple"));
        auto localTupleAddress = localTupleNode->createAddress(Type::TUPLE);

        //! \todo what about Destination address ? do we observe the address ? how to do that ?
        //! auto localDestinationNode = *(device->emplace(device->children().cend(), "my_destination"));
        //! auto localDestinationAddress = localDestinationNode->createAddress(Type::DESTINATION);

        // evaluate expressions before Destination value updates
        auto testExprA = ExpressionPulse::create(new Destination(localImpulseNode));
        QVERIFY(testExprA->getType() == Expression::Type::PULSE);
        QVERIFY(evaluate(testExprA) == false);

        auto testExprB = ExpressionPulse::create(new Destination(localBoolNode));
        QVERIFY(evaluate(testExprB) == false);

        auto testExprC = ExpressionPulse::create(new Destination(localIntNode));
        QVERIFY(evaluate(testExprC) == false);

        auto testExprD = ExpressionPulse::create(new Destination(localFloatNode));
        QVERIFY(evaluate(testExprD) == false);

        auto testExprE = ExpressionPulse::create(new Destination(localStringNode));
        QVERIFY(evaluate(testExprE) == false);

        //! \todo : what about Destination address ? do we observe the address ? how to do that ?
        //auto testExprF = ExpressionPulse::create(new Destination(localDestinationNode));
        //QVERIFY(evaluate(testExprF) == false);

        auto testExprG = ExpressionPulse::create(new Destination(localTupleNode));
        QVERIFY(evaluate(testExprG) == false);

        // update node's value
        Impulse p;
        localImpulseAddress->pushValue(&p);

        Bool b(false);
        localBoolAddress->pushValue(&b);

        Int i(5);
        localIntAddress->pushValue(&i);

        Float f(0.5);
        localFloatAddress->pushValue(&f);

        String s("abc");
        localStringAddress->pushValue(&s);

        //! \todo
        //Destination d(localFloatNode);
        //localDestinationAddress->pushValue(&d);

        Tuple t = {new Float(0.1), new Float(0.2), new Float(0.3)};
        localTupleAddress->pushValue(&t);

        // evaluate expressions after Destination value updates
        QVERIFY(evaluate(testExprA) == true);
        QVERIFY(evaluate(testExprB) == true);
        QVERIFY(evaluate(testExprC) == true);
        QVERIFY(evaluate(testExprD) == true);
        QVERIFY(evaluate(testExprE) == true);
        //! \todo QVERIFY(evaluate(testExprF) == true);
        QVERIFY(evaluate(testExprG) == true);

        // evaluate expressions after expression reset
        testExprA->update();
        QVERIFY(evaluate(testExprA) == false);

        testExprB->update();
        QVERIFY(evaluate(testExprB) == false);

        testExprC->update();
        QVERIFY(evaluate(testExprC) == false);

        testExprD->update();
        QVERIFY(evaluate(testExprD) == false);

        testExprE->update();
        QVERIFY(evaluate(testExprE) == false);

        //! \todo testExprF->update();
        //! QVERIFY(evaluate(testExprF) == false);

        testExprG->update();
        QVERIFY(evaluate(testExprG) == false);

        // update node's value again
        localImpulseAddress->pushValue(&p);
        localBoolAddress->pushValue(&b);
        localIntAddress->pushValue(&i);
        localFloatAddress->pushValue(&f);
        localStringAddress->pushValue(&s);
        //! \todo localDestinationAddress->pushValue(&d);
        localTupleAddress->pushValue(&t);

        // evaluate expressions after Destination value updates
        QVERIFY(evaluate(testExprA) == true);
        QVERIFY(evaluate(testExprB) == true);
        QVERIFY(evaluate(testExprC) == true);
        QVERIFY(evaluate(testExprD) == true);
        QVERIFY(evaluate(testExprE) == true);
        //! \todo QVERIFY(evaluate(testExprF) == true);
        QVERIFY(evaluate(testExprG) == true);

        //! \todo test clone()
    }

    /*! test comparison operator */
    void test_comparison()
    {
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        // Local tree building
        auto localNode1 = *(device->emplace(device->children().cend(), "my_node.1"));
        auto localNode2 = *(device->emplace(device->children().cend(), "my_node.2"));

        auto testExprA = ExpressionPulse::create(new Destination(localNode1));
        auto testExprB = ExpressionPulse::create(new Destination(localNode2));
        auto testExprC = ExpressionPulse::create(new Destination(localNode1));

        QVERIFY(expressions::expression_false != *testExprA);
        QVERIFY(expressions::expression_true != *testExprA);

        QVERIFY(*testExprA != *testExprB);
        QVERIFY(*testExprA == *testExprC);
        QVERIFY(*testExprB != *testExprC);
    }

    /*! test callback managment */
    void test_callback()
    {
        /*
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        auto localIntNode = *(device->emplace(device->children().cend(), "my_int.1"));
        auto localIntAddress = localIntNode->createAddress(Type::INT);

        auto testExpr = ExpressionPulse::create(new Destination(localIntNode));

        auto callback = [&] (bool b) { result_callback(b); };
        auto callback_index = testExpr->addCallback(callback);

        QVERIFY(testExpr->callbacks().size() == 1);

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

        testExpr->removeCallback(callback_index);

        QVERIFY(testExpr->callbacks().size() == 0);

        m_result = false;
        m_result_callback_called = false;

        Int i3(10);
        localIntAddress2->pushValue(&i3);

        QVERIFY(m_result_callback_called == false && m_result == false);
        */
    }
};

QTEST_APPLESS_MAIN(ExpressionPulseTest)

#include "ExpressionPulseTest.moc"
