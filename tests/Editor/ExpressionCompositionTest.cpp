// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <iostream>

using namespace ossia;
using namespace ossia::expressions;
using namespace std::placeholders;


bool m_result;
bool m_result_callback_called;

void result_callback(bool result)
{
  m_result = result;
  m_result_callback_called = true;
}

auto make_exprA()
{ return make_expression_atom(true, comparator::EQUAL, true); }
auto make_exprB()
{ return make_expression_atom(false, comparator::EQUAL, false); }
auto make_exprC()
{ return make_expression_atom(false, comparator::DIFFERENT, false); }

/*! test AND operator */
TEST_CASE ("test_AND", "test_AND")
{
  auto composition1 = make_expression_composition(
        make_exprA(), binary_operator::AND, make_exprB());
  REQUIRE(evaluate(composition1) == true);

  auto composition2 = make_expression_composition(
        make_exprA(), binary_operator::AND, make_exprC());

  REQUIRE(evaluate(composition2) == false);

  //! \todo test clone()
}

/*! test OR operator */
TEST_CASE ("test_OR", "test_OR")
{
  auto composition1 = make_expression_composition(make_exprA(),
                                                  binary_operator::OR,
                                                  make_exprB());
  REQUIRE(evaluate(composition1) == true);

  auto composition2 = make_expression_composition(make_exprA(),
                                                  binary_operator::OR,
                                                  make_exprC());
  REQUIRE(evaluate(composition2) == true);

  //! \todo test clone()
}

/*! test XOR operator */
TEST_CASE ("test_XOR", "test_XOR")
{
  auto composition1 = make_expression_composition(make_exprA(),
                                                  binary_operator::XOR,
                                                  make_exprB());
  REQUIRE(evaluate(composition1) == false);

  auto composition2 = make_expression_composition(make_exprA(),
                                                  binary_operator::XOR,
                                                  make_exprC());
  REQUIRE(evaluate(composition2) == true);

  //! \todo test clone()
}

/*! test comparison operator */
TEST_CASE ("test_comparison", "test_comparison")
{
  auto composition1 = make_expression_composition(make_exprA(),
                                                  binary_operator::XOR,
                                                  make_exprB());
  auto composition2 = make_expression_composition(make_exprA(),
                                                  binary_operator::XOR,
                                                  make_exprC());

  REQUIRE(expressions::expression_false() != *composition1);
  REQUIRE(expressions::expression_true() != *composition1);

  REQUIRE(*composition1 != *composition2);
  REQUIRE(!(*composition1 == *composition2));
}

/*! test callback managment */
TEST_CASE ("test_callback", "test_callback")
{
  // Local device
  ossia::net::generic_device device{"test"};

  auto localIntNode1 = device.create_child("my_int.1");
  auto localIntAddress1 = localIntNode1->create_parameter(val_type::INT);
  auto localIntNode2 = device.create_child("my_int.2");
  auto localIntAddress2 = localIntNode2->create_parameter(val_type::INT);
  auto localIntNode3 = device.create_child("my_int.3");
  auto localIntAddress3 = localIntNode3->create_parameter(val_type::INT);

  auto testDestinationExprA = make_expression_atom(destination(*localIntAddress1),
                                                   comparator::LOWER,
                                                   destination(*localIntAddress2));

  auto testDestinationExprB = make_expression_atom(destination(*localIntAddress2),
                                                   comparator::LOWER,
                                                   destination(*localIntAddress3));

  auto testDestinationComposition = make_expression_composition(std::move(testDestinationExprA),
                                                                binary_operator::AND,
                                                                std::move(testDestinationExprB));

  expression_result_callback callback = std::bind(&result_callback, _1);
  auto callback_index = add_callback(*testDestinationComposition, callback);

  REQUIRE(callback_count(*testDestinationComposition) == 1);

  m_result = false;
  m_result_callback_called = false;

  int i1(5);
  localIntAddress1->push_value(i1);

  REQUIRE((m_result_callback_called == true && m_result == false));

  m_result = false;
  m_result_callback_called = false;

  int i2(6);
  localIntAddress2->push_value(i2);

  REQUIRE((m_result_callback_called == true && m_result == false));

  m_result = false;
  m_result_callback_called = false;

  int i3(7);
  localIntAddress3->push_value(i3);

  REQUIRE((m_result_callback_called == true && m_result == true));

  remove_callback(*testDestinationComposition, callback_index);

  REQUIRE(callback_count(*testDestinationComposition) == 0);

  m_result = false;
  m_result_callback_called = false;

  int i4(10);
  localIntAddress2->push_value(i4);

  REQUIRE((m_result_callback_called == false && m_result == false));
}
