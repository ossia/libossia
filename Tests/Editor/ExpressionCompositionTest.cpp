#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::expressions;
using namespace std::placeholders;

class ExpressionCompositionTest : public QObject
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

  /*! test AND operator */
  void test_AND()
  {
    auto exprA = make_expression_atom(Bool(true),
                                      expression_atom::Comparator::EQUAL,
                                      Bool(true));

    auto exprB = make_expression_atom(Bool(false),
                                      expression_atom::Comparator::EQUAL,
                                      Bool(false));

    auto exprC = make_expression_atom(Bool(false),
                                      expression_atom::Comparator::DIFFERENT,
                                      Bool(false));

    auto composition1 = make_expression_composition(exprA,
                                                    expression_composition::Operator::AND,
                                                    exprB);
    QVERIFY(evaluate(composition1) == true);

    auto composition2 = make_expression_composition(exprA,
                                                    expression_composition::Operator::AND,
                                                    exprC);
    QVERIFY(evaluate(composition2) == false);

    //! \todo test clone()
  }

  /*! test OR operator */
  void test_OR()
  {
    auto exprA = make_expression_atom(Bool(true),
                                      expression_atom::Comparator::EQUAL,
                                      Bool(true));

    auto exprB = make_expression_atom(Bool(false),
                                      expression_atom::Comparator::EQUAL,
                                      Bool(false));

    auto exprC = make_expression_atom(Bool(false),
                                      expression_atom::Comparator::DIFFERENT,
                                      Bool(false));

    auto composition1 = make_expression_composition(exprA,
                                                    expression_composition::Operator::OR,
                                                    exprB);
    QVERIFY(evaluate(composition1) == true);

    auto composition2 = make_expression_composition(exprA,
                                                    expression_composition::Operator::OR,
                                                    exprC);
    QVERIFY(evaluate(composition2) == true);

    //! \todo test clone()
  }

  /*! test XOR operator */
  void test_XOR()
  {
    auto exprA = make_expression_atom(Bool(true),
                                      expression_atom::Comparator::EQUAL,
                                      Bool(true));

    auto exprB = make_expression_atom(Bool(false),
                                      expression_atom::Comparator::EQUAL,
                                      Bool(false));

    auto exprC = make_expression_atom(Bool(false),
                                      expression_atom::Comparator::DIFFERENT,
                                      Bool(false));

    auto composition1 = make_expression_composition(exprA,
                                                    expression_composition::Operator::XOR,
                                                    exprB);
    QVERIFY(evaluate(composition1) == false);

    auto composition2 = make_expression_composition(exprA,
                                                    expression_composition::Operator::XOR,
                                                    exprC);
    QVERIFY(evaluate(composition2) == true);

    //! \todo test clone()
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

    auto exprC = make_expression_atom(Bool(false),
                                      expression_atom::Comparator::DIFFERENT,
                                      Bool(false));

    auto composition1 = make_expression_composition(exprA,
                                                    expression_composition::Operator::XOR,
                                                    exprB);

    auto composition2 = make_expression_composition(exprA,
                                                    expression_composition::Operator::XOR,
                                                    exprC);

    QVERIFY(expressions::expression_false != *composition1);
    QVERIFY(expressions::expression_true != *composition1);

    QVERIFY(*composition1 != *composition2);
    QVERIFY(!(*composition1 == *composition2));
  }

  /*! test callback managment */
  void test_callback()
  {
    // Local device
    impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};

    auto localIntNode1 = device.createChild("my_int.1");
    auto localIntAddress1 = localIntNode1->createAddress(Type::INT);
    auto localIntNode2 = device.createChild("my_int.2");
    auto localIntAddress2 = localIntNode2->createAddress(Type::INT);
    auto localIntNode3 = device.createChild("my_int.3");
    auto localIntAddress3 = localIntNode3->createAddress(Type::INT);

    auto testDestinationExprA = make_expression_atom(Destination(*localIntNode1),
                                                     expression_atom::Comparator::LOWER_THAN,
                                                     Destination(*localIntNode2));

    auto testDestinationExprB = make_expression_atom(Destination(*localIntNode2),
                                                     expression_atom::Comparator::LOWER_THAN,
                                                     Destination(*localIntNode3));

    auto testDestinationComposition = make_expression_composition(testDestinationExprA,
                                                                  expression_composition::Operator::AND,
                                                                  testDestinationExprB);

    expression_result_callback callback = std::bind(&ExpressionCompositionTest::result_callback, this, _1);
    auto callback_index = addCallback(testDestinationComposition, callback);

    QVERIFY(testDestinationComposition->callbacks().size() == 1);

    m_result = false;
    m_result_callback_called = false;

    Int i1(5);
    localIntAddress1->pushValue(i1);

    QVERIFY(m_result_callback_called == true && m_result == false);

    m_result = false;
    m_result_callback_called = false;

    Int i2(6);
    localIntAddress2->pushValue(i2);

    QVERIFY(m_result_callback_called == true && m_result == false);

    m_result = false;
    m_result_callback_called = false;

    Int i3(7);
    localIntAddress3->pushValue(i3);

    QVERIFY(m_result_callback_called == true && m_result == true);

    removeCallback(testDestinationComposition, callback_index);

    QVERIFY(testDestinationComposition->callbacks().size() == 0);

    m_result = false;
    m_result_callback_called = false;

    Int i4(10);
    localIntAddress2->pushValue(i4);

    QVERIFY(m_result_callback_called == false && m_result == false);
  }
};

QTEST_APPLESS_MAIN(ExpressionCompositionTest)

#include "ExpressionCompositionTest.moc"
