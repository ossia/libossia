#include <QtTest>
#include <ossia/ossia.hpp>
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

  auto make_exprA()
  { return make_expression_atom(true, expression_atom::Comparator::EQUAL, true); }
  auto make_exprB()
  { return make_expression_atom(false, expression_atom::Comparator::EQUAL, false); }
  auto make_exprC()
  { return make_expression_atom(false, expression_atom::Comparator::DIFFERENT, false); }

private Q_SLOTS:

  /*! test AND operator */
  void test_AND()
  {
    auto composition1 = make_expression_composition(
          make_exprA(), expression_composition::Operator::AND, make_exprB());
    QVERIFY(evaluate(composition1) == true);

    auto composition2 = make_expression_composition(
          make_exprA(), expression_composition::Operator::AND, make_exprC());

    QVERIFY(evaluate(composition2) == false);

    //! \todo test clone()
  }

  /*! test OR operator */
  void test_OR()
  {
    auto composition1 = make_expression_composition(make_exprA(),
                                                    expression_composition::Operator::OR,
                                                    make_exprB());
    QVERIFY(evaluate(composition1) == true);

    auto composition2 = make_expression_composition(make_exprA(),
                                                    expression_composition::Operator::OR,
                                                    make_exprC());
    QVERIFY(evaluate(composition2) == true);

    //! \todo test clone()
  }

  /*! test XOR operator */
  void test_XOR()
  {
    auto composition1 = make_expression_composition(make_exprA(),
                                                    expression_composition::Operator::XOR,
                                                    make_exprB());
    QVERIFY(evaluate(composition1) == false);

    auto composition2 = make_expression_composition(make_exprA(),
                                                    expression_composition::Operator::XOR,
                                                    make_exprC());
    QVERIFY(evaluate(composition2) == true);

    //! \todo test clone()
  }

  /*! test comparison operator */
  void test_comparison()
  {
    auto composition1 = make_expression_composition(make_exprA(),
                                                    expression_composition::Operator::XOR,
                                                    make_exprB());
    auto composition2 = make_expression_composition(make_exprA(),
                                                    expression_composition::Operator::XOR,
                                                    make_exprC());

    QVERIFY(expressions::expression_false != *composition1);
    QVERIFY(expressions::expression_true != *composition1);

    QVERIFY(*composition1 != *composition2);
    QVERIFY(!(*composition1 == *composition2));
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
    auto localIntNode3 = device.createChild("my_int.3");
    auto localIntAddress3 = localIntNode3->createAddress(val_type::INT);

    auto testDestinationExprA = make_expression_atom(Destination(*localIntAddress1),
                                                     expression_atom::Comparator::LOWER_THAN,
                                                     Destination(*localIntAddress2));

    auto testDestinationExprB = make_expression_atom(Destination(*localIntAddress2),
                                                     expression_atom::Comparator::LOWER_THAN,
                                                     Destination(*localIntAddress3));

    auto testDestinationComposition = make_expression_composition(std::move(testDestinationExprA),
                                                                  expression_composition::Operator::AND,
                                                                  std::move(testDestinationExprB));

    expression_result_callback callback = std::bind(&ExpressionCompositionTest::result_callback, this, _1);
    auto callback_index = add_callback(*testDestinationComposition, callback);

    QVERIFY(callback_count(*testDestinationComposition) == 1);

    m_result = false;
    m_result_callback_called = false;

    int i1(5);
    localIntAddress1->pushValue(i1);

    QVERIFY(m_result_callback_called == true && m_result == false);

    m_result = false;
    m_result_callback_called = false;

    int i2(6);
    localIntAddress2->pushValue(i2);

    QVERIFY(m_result_callback_called == true && m_result == false);

    m_result = false;
    m_result_callback_called = false;

    int i3(7);
    localIntAddress3->pushValue(i3);

    QVERIFY(m_result_callback_called == true && m_result == true);

    remove_callback(*testDestinationComposition, callback_index);

    QVERIFY(callback_count(*testDestinationComposition) == 0);

    m_result = false;
    m_result_callback_called = false;

    int i4(10);
    localIntAddress2->pushValue(i4);

    QVERIFY(m_result_callback_called == false && m_result == false);
  }
};

QTEST_APPLESS_MAIN(ExpressionCompositionTest)

#include "ExpressionCompositionTest.moc"
