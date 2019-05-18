// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include <iostream>

using namespace ossia;

void event_callback(time_event::status newStatus)
{
  ;
}


/*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
{
  auto node = std::make_shared<time_sync>();
  REQUIRE(node != nullptr);

  ossia::time_value date = node->get_date();
  REQUIRE(date == Zero);

  REQUIRE(node->get_expression() == expressions::expression_true());

  auto expression = expressions::make_expression_true();
  auto& expr_ref = *expression;
  node->set_expression(std::move(expression));

  REQUIRE(node->get_expression() == expr_ref);

  REQUIRE(node->get_time_events().size() == 0);

  //! \todo test clone()
}

/*! test edition functions */
TEST_CASE ("test_edition", "test_edition")
{
  auto node = std::make_shared<time_sync>();
  REQUIRE(node != nullptr);

  auto event = *(node->emplace(
                   node->get_time_events().begin(),
                   &event_callback, expressions::make_expression_true()));

  REQUIRE(event != nullptr);

  REQUIRE(node->get_time_events().size() == 1);
}
