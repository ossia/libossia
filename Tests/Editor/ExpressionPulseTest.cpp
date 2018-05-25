// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/ossia.hpp>
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
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    // Local tree building
    auto localImpulseNode = device.create_child("my_impulse");
    auto localImpulseAddress = localImpulseNode->create_parameter(val_type::IMPULSE);

    auto localBoolNode = device.create_child("my_bool");
    auto localBoolAddress = localBoolNode->create_parameter(val_type::BOOL);

    auto localIntNode = device.create_child("my_int");
    auto localIntAddress = localIntNode->create_parameter(val_type::INT);

    auto localFloatNode = device.create_child("my_float");
    auto localFloatAddress = localFloatNode->create_parameter(val_type::FLOAT);

    auto localStringNode = device.create_child("my_string");
    auto localStringAddress = localStringNode->create_parameter(val_type::STRING);

    auto localTupleNode = device.create_child("my_tuple");
    auto localTupleAddress = localTupleNode->create_parameter(val_type::LIST);

    //! \todo what about Destination address ? do we observe the address ? how to do that ?
    //! auto localDestinationNode = device.create_child("my_destination"));
    //! auto localDestinationAddress = localDestinationNode->create_parameter(Type::DESTINATION);

    // evaluate expressions before Destination value updates
    auto testExprA = make_expression_pulse(destination(*localImpulseAddress));
    QVERIFY(evaluate(testExprA) == false);

    auto testExprB = make_expression_pulse(destination(*localBoolAddress));
    QVERIFY(evaluate(testExprB) == false);

    auto testExprC = make_expression_pulse(destination(*localIntAddress));
    QVERIFY(evaluate(testExprC) == false);

    auto testExprD = make_expression_pulse(destination(*localFloatAddress));
    QVERIFY(evaluate(testExprD) == false);

    auto testExprE = make_expression_pulse(destination(*localStringAddress));
    QVERIFY(evaluate(testExprE) == false);

    //! \todo : what about Destination address ? do we observe the address ? how to do that ?
    //auto testExprF = make_expression_pulse(Destination(localDestinationNode));
    //QVERIFY(evaluate(testExprF) == false);

    auto testExprG = make_expression_pulse(destination(*localStringAddress));
    QVERIFY(evaluate(testExprG) == false);

    // update node's value
    impulse p;
    localImpulseAddress->push_value(p);

    bool b = false;
    localBoolAddress->push_value(b);

    int i = 5;
    localIntAddress->push_value(i);

    float f = 0.5;
    localFloatAddress->push_value(f);

    std::string s = "abc";
    localStringAddress->push_value(s);

    //! \todo
    //Destination d(localFloatNode);
    //localDestinationAddress->push_value(d);

    std::vector<ossia::value> t{1., 2., 3.};
    localTupleAddress->push_value(t);

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
    localImpulseAddress->push_value(p);
    localBoolAddress->push_value(b);
    localIntAddress->push_value(i);
    localFloatAddress->push_value(f);
    localStringAddress->push_value(s);
    //! \todo localDestinationAddress->push_value(d);
    localTupleAddress->push_value(t);

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
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    // Local tree building
    auto localNode1 = device.create_child("my_node.1");
    auto lcalAddr1 = localNode1->create_parameter(ossia::val_type::IMPULSE);
    auto localNode2 = device.create_child("my_node.2");
    auto lcalAddr2 = localNode2->create_parameter(ossia::val_type::IMPULSE);

    auto testExprA = make_expression_pulse(destination(*lcalAddr1));
    auto testExprB = make_expression_pulse(destination(*lcalAddr2));
    auto testExprC = make_expression_pulse(destination(*lcalAddr1));

    QVERIFY(expressions::expression_false() != *testExprA);
    QVERIFY(expressions::expression_true() != *testExprA);

    QVERIFY(*testExprA != *testExprB);
    QVERIFY(*testExprA == *testExprC);
    QVERIFY(*testExprB != *testExprC);
  }

  /*! test callback managment */
  void test_callback()
  {/* TODO
    // Local device
    impl::BasicDevice device{std::make_unique<ossia::net::Local2>(), "test"};

    auto localIntNode = device.create_child("my_int.1");
    auto localIntAddress = localIntNode->create_parameter(Type::INT);

    auto testExpr = make_expression_pulse(Destination(*localIntNode));

    auto callback = [&] (bool b) { result_callback(b); };
    auto callback_index = testExpr->addCallback(callback);

    QVERIFY(testExpr->callbacks().size() == 1);

    m_result = false;
    m_result_callback_called = false;

    Int i1(5);
    localIntAddress1->push_value(i1);

    QVERIFY(m_result_callback_called == true && m_result == false);

    m_result = false;
    m_result_callback_called = false;

    Int i2(5);
    localIntAddress2->push_value(i2);

    QVERIFY(m_result_callback_called == true && m_result == true);

    testExpr->removeCallback(callback_index);

    QVERIFY(testExpr->callbacks().size() == 0);

    m_result = false;
    m_result_callback_called = false;

    Int i3(10);
    localIntAddress2->push_value(i3);

    QVERIFY(m_result_callback_called == false && m_result == false);
    */
  }
};

QTEST_APPLESS_MAIN(ExpressionPulseTest)

#include "ExpressionPulseTest.moc"
