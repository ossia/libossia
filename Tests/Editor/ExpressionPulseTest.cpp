#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::expressions;
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
    impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};

    // Local tree building
    auto localImpulseNode = device.createChild("my_impulse");
    auto localImpulseAddress = localImpulseNode->createAddress(Type::IMPULSE);

    auto localBoolNode = device.createChild("my_bool");
    auto localBoolAddress = localBoolNode->createAddress(Type::BOOL);

    auto localIntNode = device.createChild("my_int");
    auto localIntAddress = localIntNode->createAddress(Type::INT);

    auto localFloatNode = device.createChild("my_float");
    auto localFloatAddress = localFloatNode->createAddress(Type::FLOAT);

    auto localStringNode = device.createChild("my_string");
    auto localStringAddress = localStringNode->createAddress(Type::STRING);

    auto localTupleNode = device.createChild("my_tuple");
    auto localTupleAddress = localTupleNode->createAddress(Type::TUPLE);

    //! \todo what about Destination address ? do we observe the address ? how to do that ?
    //! auto localDestinationNode = device.createChild("my_destination"));
    //! auto localDestinationAddress = localDestinationNode->createAddress(Type::DESTINATION);

    // evaluate expressions before Destination value updates
    auto testExprA = make_expression_pulse(Destination(*localImpulseNode));
    QVERIFY(evaluate(testExprA) == false);

    auto testExprB = make_expression_pulse(Destination(*localBoolNode));
    QVERIFY(evaluate(testExprB) == false);

    auto testExprC = make_expression_pulse(Destination(*localIntNode));
    QVERIFY(evaluate(testExprC) == false);

    auto testExprD = make_expression_pulse(Destination(*localFloatNode));
    QVERIFY(evaluate(testExprD) == false);

    auto testExprE = make_expression_pulse(Destination(*localStringNode));
    QVERIFY(evaluate(testExprE) == false);

    //! \todo : what about Destination address ? do we observe the address ? how to do that ?
    //auto testExprF = make_expression_pulse(Destination(localDestinationNode));
    //QVERIFY(evaluate(testExprF) == false);

    auto testExprG = make_expression_pulse(Destination(*localTupleNode));
    QVERIFY(evaluate(testExprG) == false);

    // update node's value
    Impulse p;
    localImpulseAddress->pushValue(p);

    Bool b(false);
    localBoolAddress->pushValue(b);

    Int i(5);
    localIntAddress->pushValue(i);

    Float f(0.5);
    localFloatAddress->pushValue(f);

    String s("abc");
    localStringAddress->pushValue(s);

    //! \todo
    //Destination d(localFloatNode);
    //localDestinationAddress->pushValue(d);

    Tuple t{Float(0.1), Float(0.2), Float(0.3)};
    localTupleAddress->pushValue(t);

    // evaluate expressions after Destination value updates
    QVERIFY(evaluate(testExprA) == true);
    QVERIFY(evaluate(testExprB) == true);
    QVERIFY(evaluate(testExprC) == true);
    QVERIFY(evaluate(testExprD) == true);
    QVERIFY(evaluate(testExprE) == true);
    //! \todo QVERIFY(evaluate(testExprF) == true);
    QVERIFY(evaluate(testExprG) == true);

    // evaluate expressions after expression reset
    update(testExprA);
    QVERIFY(evaluate(testExprA) == false);

    update(testExprB);
    QVERIFY(evaluate(testExprB) == false);

    update(testExprC);
    QVERIFY(evaluate(testExprC) == false);

    update(testExprD);
    QVERIFY(evaluate(testExprD) == false);

    update(testExprE);
    QVERIFY(evaluate(testExprE) == false);

    //! \todo update(testExprF);
    //! QVERIFY(evaluate(testExprF) == false);

    update(testExprG);
    QVERIFY(evaluate(testExprG) == false);

    // update node's value again
    localImpulseAddress->pushValue(p);
    localBoolAddress->pushValue(b);
    localIntAddress->pushValue(i);
    localFloatAddress->pushValue(f);
    localStringAddress->pushValue(s);
    //! \todo localDestinationAddress->pushValue(d);
    localTupleAddress->pushValue(t);

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
    impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};

    // Local tree building
    auto localNode1 = device.createChild("my_node.1");
    auto localNode2 = device.createChild("my_node.2");

    auto testExprA = make_expression_pulse(Destination(*localNode1));
    auto testExprB = make_expression_pulse(Destination(*localNode2));
    auto testExprC = make_expression_pulse(Destination(*localNode1));

    QVERIFY(expressions::expression_false != *testExprA);
    QVERIFY(expressions::expression_true != *testExprA);

    QVERIFY(*testExprA != *testExprB);
    QVERIFY(*testExprA == *testExprC);
    QVERIFY(*testExprB != *testExprC);
  }

  /*! test callback managment */
  void test_callback()
  {/* TODO
    // Local device
    impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};

    auto localIntNode = device.createChild("my_int.1");
    auto localIntAddress = localIntNode->createAddress(Type::INT);

    auto testExpr = make_expression_pulse(Destination(*localIntNode));

    auto callback = [&] (bool b) { result_callback(b); };
    auto callback_index = testExpr->addCallback(callback);

    QVERIFY(testExpr->callbacks().size() == 1);

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

    testExpr->removeCallback(callback_index);

    QVERIFY(testExpr->callbacks().size() == 0);

    m_result = false;
    m_result_callback_called = false;

    Int i3(10);
    localIntAddress2->pushValue(i3);

    QVERIFY(m_result_callback_called == false && m_result == false);
    */
  }
};

QTEST_APPLESS_MAIN(ExpressionPulseTest)

#include "ExpressionPulseTest.moc"
