// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>

#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include "include_catch.hpp"

using namespace ossia;
using namespace ossia::expressions;

// After time_sync::reset(), execution of the trigger is stopped: the
// expression must not keep observing device parameters. If it does, a device
// that is torn down before the scenario structures leaves the expression
// holding a callback registration into a freed parameter, and
// expression_atom::on_removing_last_callback() then reads freed memory in
// callback_container::remove_callback (heap-use-after-free).
TEST_CASE("time_sync reset stops expression observation", "[expression][lifetime]")
{
  auto device = std::make_unique<ossia::net::generic_device>("test");
  auto param = device->create_child("b")->create_parameter(val_type::BOOL);

  auto ts = std::make_shared<ossia::time_sync>();
  ts->set_expression(
      make_expression_atom(destination{*param}, comparator::EQUAL, value(true)));

  ts->observe_expression(true);
  CHECK(param->callback_count() == 1);

  ts->reset();
  CHECK(param->callback_count() == 0);

  // Device is torn down before the scenario structures; if the expression is
  // still registered on the parameter, its destructor then erases freed
  // memory.
  device.reset();
  ts.reset();
}

// Replacing the expression of a time_sync whose stored callback index is
// still engaged (as left behind by reset()) must not leave the time_sync
// holding an iterator into the destroyed expression: the next
// observe_expression(true) passes that stale iterator to
// expression_callback_container::remove_callback of the new expression.
TEST_CASE("time_sync set_expression drops stale observation", "[expression][lifetime]")
{
  ossia::net::generic_device device{"test"};
  auto param = device.create_child("b")->create_parameter(val_type::BOOL);

  auto ts = std::make_shared<ossia::time_sync>();
  ts->set_expression(
      make_expression_atom(destination{*param}, comparator::EQUAL, value(true)));

  ts->observe_expression(true);
  ts->reset();

  // The old expression is destroyed here while m_callback still refers to it.
  ts->set_expression(make_expression_atom(
      destination{*param}, comparator::EQUAL, value(false)));

  // Re-observing passes the stale iterator to the new expression.
  ts->observe_expression(true);
  CHECK(param->callback_count() == 1);

  ts->observe_expression(false);
  CHECK(param->callback_count() == 0);
}

// Same shape without reset(): replacing the expression while observing.
TEST_CASE(
    "time_sync set_expression while observing", "[expression][lifetime]")
{
  ossia::net::generic_device device{"test"};
  auto param = device.create_child("b")->create_parameter(val_type::BOOL);

  auto ts = std::make_shared<ossia::time_sync>();
  ts->set_expression(
      make_expression_atom(destination{*param}, comparator::EQUAL, value(true)));

  ts->observe_expression(true);
  CHECK(param->callback_count() == 1);

  ts->set_expression(make_expression_atom(
      destination{*param}, comparator::EQUAL, value(false)));
  CHECK(param->callback_count() == 0);

  ts->observe_expression(true);
  CHECK(param->callback_count() == 1);
  ts->observe_expression(false);
  CHECK(param->callback_count() == 0);
}

// Control: the plain add/remove cycle on an expression_atom pairs
// on_first_callback_added / on_removing_last_callback correctly.
TEST_CASE("expression_atom add/remove pairs correctly", "[expression][lifetime]")
{
  ossia::net::generic_device device{"test"};
  auto param = device.create_child("b")->create_parameter(val_type::BOOL);

  auto expr
      = make_expression_atom(destination{*param}, comparator::EQUAL, value(true));

  auto it = add_callback(*expr, [](bool) {});
  CHECK(param->callback_count() == 1);
  remove_callback(*expr, it);
  CHECK(param->callback_count() == 0);

  auto it2 = add_callback(*expr, [](bool) {});
  CHECK(param->callback_count() == 1);
  remove_callback(*expr, it2);
  CHECK(param->callback_count() == 0);
}
