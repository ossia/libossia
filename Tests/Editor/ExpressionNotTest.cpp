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


/*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
{
  auto expression = make_expression_atom(false,
                                         comparator::DIFFERENT,
                                         false);

  auto not_expression = make_expression_not(std::move(expression));
  REQUIRE(not_expression != nullptr);
  REQUIRE(evaluate(not_expression) == true);
}

/*! test comparison operator */
TEST_CASE ("test_comparison", "test_comparison")
{
  auto exprA = make_expression_atom(true,
                                    comparator::EQUAL,
                                    true);

  auto exprB = make_expression_atom(false,
                                    comparator::EQUAL,
                                    false);

  auto exprC = make_expression_atom(true,
                                    comparator::EQUAL,
                                    true);

  auto not_exprA = make_expression_not(std::move(exprA));
  auto not_exprB = make_expression_not(std::move(exprB));
  auto not_exprC = make_expression_not(std::move(exprC));

  REQUIRE(expressions::expression_false() != *not_exprA);
  REQUIRE(expressions::expression_true() != *not_exprA);

  REQUIRE(*not_exprA != *not_exprB);
  REQUIRE(*not_exprA == *not_exprC);
  REQUIRE(*not_exprB != *not_exprC);
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

  auto testDestinationExpr = make_expression_atom(destination(*localIntAddress1),
                                                  comparator::DIFFERENT,
                                                  destination(*localIntAddress2));

  auto testDestinationExprNot = make_expression_not(std::move(testDestinationExpr));

  expression_result_callback callback = std::bind(&result_callback, _1);
  auto callback_index = add_callback(*testDestinationExprNot, callback);

  REQUIRE(callback_count(*testDestinationExprNot) == 1);

  m_result = false;
  m_result_callback_called = false;

  localIntAddress1->push_value(5);

  REQUIRE((m_result_callback_called == true && m_result == false));

  m_result = false;
  m_result_callback_called = false;

  localIntAddress2->push_value(5);

  REQUIRE((m_result_callback_called == true && m_result == true));

  m_result = false;
  m_result_callback_called = false;

  localIntAddress2->push_value(10);

  REQUIRE((m_result_callback_called == true && m_result == false));

  remove_callback(*testDestinationExprNot, callback_index);

  REQUIRE(callback_count(*testDestinationExprNot) == 0);

  m_result = false;
  m_result_callback_called = false;

  localIntAddress2->push_value(5);

  REQUIRE((m_result_callback_called == false && m_result == false));
}
