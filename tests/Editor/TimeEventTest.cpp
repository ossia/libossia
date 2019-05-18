// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/expression/expression.hpp>

#include <iostream>

using namespace ossia;

/*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
{
  auto node = std::make_shared<time_sync>();
  auto event = *(node->emplace(node->get_time_events().begin(), time_event::exec_callback{}));
  REQUIRE(event != nullptr);

  REQUIRE(&event->get_time_sync() == node.get());
  REQUIRE(event->get_expression() == expressions::expression_true());
  REQUIRE(event->get_status() == time_event::status::NONE);

  auto event_with_expression = *(node->emplace(node->get_time_events().begin(), time_event::exec_callback{}, expressions::make_expression_false()));
  REQUIRE(event_with_expression != nullptr);

  REQUIRE(event_with_expression->get_expression() == expressions::expression_false());

  auto expression = expressions::make_expression_true();
  auto& expr_ref = *expression;
  event_with_expression->set_expression(std::move(expression));
  REQUIRE(event_with_expression->get_expression() == expr_ref);

  //! \todo test clone()
}

/*! test edition functions */
TEST_CASE ("test_edition", "test_edition")
{
  auto nodeA = std::make_shared<time_sync>();
  auto eventA = *(nodeA->emplace(nodeA->get_time_events().begin(), time_event::exec_callback{}));

  auto nodeB = std::make_shared<time_sync>();
  auto eventB = *(nodeB->emplace(nodeB->get_time_events().begin(), time_event::exec_callback{}));

  auto nodeC = std::make_shared<time_sync>();
  auto eventC = *(nodeC->emplace(nodeC->get_time_events().begin(), time_event::exec_callback{}));

  auto interval1 = time_interval::create(ossia::time_interval::exec_callback{}, *eventA, *eventB, 1000_tv);
  auto interval2 = time_interval::create(ossia::time_interval::exec_callback{}, *eventB, *eventC, 1000_tv);
  auto interval3 = time_interval::create(ossia::time_interval::exec_callback{}, *eventA, *eventC, 2000_tv);


  REQUIRE(eventA->previous_time_intervals().size() == 0);
  REQUIRE(eventA->next_time_intervals().size() == 2);

  REQUIRE(eventB->previous_time_intervals().size() == 1);
  REQUIRE(eventB->next_time_intervals().size() == 1);

  REQUIRE(eventC->previous_time_intervals().size() == 2);
  REQUIRE(eventC->next_time_intervals().size() == 0);
}
