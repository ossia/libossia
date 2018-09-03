// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/expression/expression.hpp>

#include <iostream>

using namespace ossia;
using namespace ossia::expressions;


/*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
{
  auto expr_default = make_expression_bool(false);
  REQUIRE(expr_default != nullptr);
  REQUIRE(evaluate(expr_default) == false);

  auto expr_false = make_expression_bool(false);
  REQUIRE(expr_false != nullptr);
  REQUIRE(evaluate(expr_false) == false);

  auto expr_true = make_expression_true();
  REQUIRE(expr_true != nullptr);
  REQUIRE(evaluate(expr_true) == true);

  REQUIRE(evaluate(expression_false()) == false);
  REQUIRE(evaluate(expression_true()) == true);
}

/*! test comparison operator */
TEST_CASE ("test_comparison", "test_comparison")
{
  REQUIRE(expressions::expression_false() == expressions::expression_false());
  REQUIRE(expressions::expression_false() != expressions::expression_true());
  REQUIRE(expressions::expression_true() != expressions::expression_false());
  REQUIRE(expressions::expression_true() == expressions::expression_true());

  auto expression_false = make_expression_bool(false);
  auto expression_true = make_expression_true();

  REQUIRE(expressions::expression_false() == *expression_false);
  REQUIRE(expressions::expression_false() != *expression_true);

  REQUIRE(expressions::expression_true() == *expression_true);
  REQUIRE(expressions::expression_true() != *expression_false);
}
