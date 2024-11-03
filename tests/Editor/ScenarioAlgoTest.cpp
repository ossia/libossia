// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution/merged_policy.hpp>
#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/nodes/dummy.hpp>
#include <ossia/dataflow/nodes/messages.hpp>
#include <ossia/dataflow/nodes/percentage.hpp>

#include "include_catch.hpp"

#include <iostream>

static auto create_event(ossia::scenario& s)
{
  auto en = std::make_shared<ossia::time_sync>();
  en->set_expression(ossia::expressions::make_expression_true());
  auto ee = std::make_shared<ossia::time_event>(
      ossia::time_event::exec_callback{}, *en,
      ossia::expressions::make_expression_true());
  en->insert(en->get_time_events().end(), ee);
  s.add_time_sync(std::move(en));
  return ee;
}

static std::shared_ptr<ossia::time_interval> create_interval(
    ossia::time_interval::exec_callback callback, ossia::time_event& startEvent,
    ossia::time_event& endEvent, ossia::time_value nominal, ossia::time_value min,
    ossia::time_value max)
{
  auto ptr
      = ossia::time_interval::create(callback, startEvent, endEvent, nominal, min, max);
  // Added because as an optimization nodes aren't added to intervals when they don't have a children
  // which woudl make tests much harder to read / write / understand and does not change anything in practice

  ptr->add_time_process(std::make_shared<ossia::node_process>(
      std::make_shared<ossia::nodes::dummy_node>()));
  return ptr;
}
/*
namespace ossia
{
bool operator==(const std::vector<ossia::token_request>& lhs, const std::vector<ossia::simple_token_request>& rhs)
{
  if(lhs.size() != rhs.size())
    return false;

  for(std::size_t i = 0; i < lhs.size(); i++)
  {
    if(lhs[i] != rhs[i])
      return false;
  }

  return true;
}
bool operator==(const std::vector<ossia::simple_token_request>& lhs, const std::vector<ossia::token_request>& rhs)
{
  return rhs == lhs;
}
}
*/

static auto start_event(ossia::scenario& s)
{
  auto sn = s.get_start_time_sync();
  return *sn->get_time_events().begin();
}

static auto start_and_tick(const std::shared_ptr<ossia::time_interval>& itv)
{
  itv->start();
  itv->tick_current(ossia::time_value{}, {});
}

static ossia::token_request
default_request(ossia::time_value from = {}, ossia::time_value to = {})
{
  ossia::token_request req;
  req.prev_date = from;
  req.date = to;
  req.tempo = 120;
  req.speed = 1.;
  req.musical_start_last_bar = 0.;
  req.musical_end_last_bar = 0.;
  req.musical_start_position = 0.;
  req.musical_end_position = 1.;
  req.signature = {4, 4};
  return req;
}

TEST_CASE("test_exec_simple", "test_exec_simple")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> se = start_event(scenario);
  std::shared_ptr<time_event> ee = create_event(scenario);

  std::shared_ptr<time_interval> c
      = create_interval({}, *se, *ee, 3000_tv, 3000_tv, 3000_tv);
  s.scenario->add_time_interval(c);

  start_and_tick(s.interval);
  REQUIRE(se->get_status() & time_event::status::FINISHED);
  REQUIRE(c->get_date() == 0_tv);

  s.interval->tick(1000_tv, default_request());
  REQUIRE(c->get_date() == 1000_tv);
  s.interval->tick(1000_tv, default_request());
  REQUIRE(c->get_date() == 2000_tv);
  s.interval->tick(999_tv, default_request());
  REQUIRE(c->get_date() == 2999_tv);
  s.interval->tick(1_tv, default_request()); // The interval is stopped

  REQUIRE(c->get_date() == 0_tv);
  REQUIRE(c->get_end_event().get_status() & time_event::status::FINISHED);
  s.interval->tick(1000_tv, default_request());
  REQUIRE(c->get_date() == 0_tv);
  REQUIRE(c->get_end_event().get_status() & time_event::status::FINISHED);
  s.interval->tick(1000_tv, default_request());
  REQUIRE(c->get_date() == 0_tv);
  REQUIRE(c->get_end_event().get_status() & time_event::status::FINISHED);
}
TEST_CASE("test_exec_0", "test_exec_0")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> se = start_event(scenario);
  std::shared_ptr<time_event> ee = create_event(scenario);

  std::shared_ptr<time_interval> c = create_interval({}, *se, *ee, 0_tv, 0_tv, 0_tv);
  s.scenario->add_time_interval(c);

  start_and_tick(s.interval);
  REQUIRE(se->get_status() & time_event::status::FINISHED);
  REQUIRE(c->get_date() == 0_tv);

  s.interval->tick(1000_tv, default_request());
  REQUIRE(c->get_date() == 0_tv);
}

TEST_CASE("test_exec_chain_long", "test_exec_chain_long")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 3000_tv, 3000_tv, 3000_tv);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 3000_tv, 3000_tv, 3000_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  start_and_tick(s.interval);
  s.interval->tick(500_tv, default_request());
  REQUIRE(c0->get_date() == 500_tv);
  REQUIRE(c1->get_date() == 0_tv);
  s.interval->tick(3000_tv, default_request());

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 500_tv);
}

TEST_CASE("test_exec_chain_tokens", "test_exec_chain_tokens")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 10_tv, 10_tv, 10_tv);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 100_tv, 100_tv, 100_tv);

  // {
  auto n = std::make_shared<ossia::nodes::messages>();
  auto proc = std::make_shared<ossia::node_process>(n);
  c1->add_time_process(proc);
  //} auto n = c1->node;
  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  start_and_tick(s.interval);

  s.interval->tick(50_tv, default_request());
  REQUIRE(c1->get_date() == 40_tv);
  for(auto tok : n->requested_tokens)
  {
    std::cout << tok << "\n";
  }

  REQUIRE((int)n->requested_tokens.size() == (int)1);
  REQUIRE(
      n->requested_tokens[0]
      == (simple_token_request{.prev_date = 0_tv, .date = 40_tv, .offset = 10_tv}));
  n->requested_tokens.clear();

  s.interval->tick(50_tv, default_request());
  REQUIRE(c1->get_date() == 90_tv);
  for(auto tok : n->requested_tokens)
  {
    std::cout << tok << "\n";
  }

  REQUIRE((int)n->requested_tokens.size() == (int)1);
  REQUIRE(
      n->requested_tokens[0]
      == (simple_token_request{.prev_date = 40_tv, .date = 90_tv, .offset = 0_tv}));
  n->requested_tokens.clear();
}

TEST_CASE("test_exec_chain_multi", "test_exec_chain_multi")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  std::shared_ptr<time_event> e3 = create_event(scenario);
  std::shared_ptr<time_event> e4 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 1_tv, 1_tv, 1_tv);
  std::shared_ptr<time_interval> c1 = create_interval({}, *e1, *e2, 1_tv, 1_tv, 1_tv);
  std::shared_ptr<time_interval> c2 = create_interval({}, *e2, *e3, 1_tv, 1_tv, 1_tv);
  std::shared_ptr<time_interval> c3 = create_interval({}, *e3, *e4, 10_tv, 10_tv, 10_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);
  s.scenario->add_time_interval(c2);
  s.scenario->add_time_interval(c3);

  start_and_tick(s.interval);
  s.interval->tick(5_tv, default_request());
  std::cout << e0->get_status() << " " << e1->get_status() << " " << e2->get_status()
            << " " << e3->get_status() << std::endl;

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(c2->get_date() == 0_tv);
  REQUIRE(c3->get_date() == 2_tv);
}

TEST_CASE("test_exec_chain_multi_infinite", "test_exec_chain_multi_infinite")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e3 = create_event(scenario);
  e3->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e4 = create_event(scenario);
  e4->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 1_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 1_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c2
      = create_interval({}, *e2, *e3, 1_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c3
      = create_interval({}, *e3, *e4, 1_tv, 0_tv, ossia::Infinite);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);
  s.scenario->add_time_interval(c2);
  s.scenario->add_time_interval(c3);

  start_and_tick(s.interval);
  {
    s.interval->tick(5_tv, default_request());
    std::cout << e1->get_status() << " " << e2->get_status() << " " << e3->get_status()
              << " " << e4->get_status() << std::endl;

    REQUIRE(c0->get_date() == 5_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
    REQUIRE(e4->get_status() == time_event::status::NONE);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(
      1, ossia::expressions::comparator::EQUAL, 1));

  // The "interactivity" tick introduced
  {
    s.interval->tick(5_tv, default_request());
    std::cout << e1->get_status() << " " << e2->get_status() << " " << e3->get_status()
              << " " << e4->get_status() << std::endl;

    REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(e1->get_status() & time_event::status::FINISHED);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
    REQUIRE(e4->get_status() == time_event::status::NONE);
  }

  {
    s.interval->tick(5_tv, default_request());
    std::cout << e1->get_status() << " " << e2->get_status() << " " << e3->get_status()
              << " " << e4->get_status() << std::endl;

    REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(e1->get_status() & time_event::status::FINISHED);

    REQUIRE(c1->get_date() == 5_tv);
    REQUIRE(e2->get_status() == time_event::status::PENDING);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
    REQUIRE(e4->get_status() == time_event::status::NONE);
  }
}

TEST_CASE("test_exec_two_branch_infinite", "test_exec_two_branch_infinite")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e3 = create_event(scenario);
  e3->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  /*           c0
       * e0 - - - - - - - - - - e2 - )
       * |    c1         c2     |      c3
       * | - - - - e1 - - - - - | ------------ e3
       */

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e2, 20_tv, 0_tv, 25_tv);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e0, *e1, 15_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c2
      = create_interval({}, *e1, *e2, 5_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c3
      = create_interval({}, *e2, *e3, 100_tv, 100_tv, 100_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);
  s.scenario->add_time_interval(c2);
  s.scenario->add_time_interval(c3);

  start_and_tick(s.interval);
  {
    s.interval->tick(5_tv, default_request());
    std::cout << e1->get_status() << " " << e2->get_status() << " " << e3->get_status()
              << std::endl;

    REQUIRE(c0->get_date() == 5_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 5_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
  }

  {
    s.interval->tick(30_tv, default_request());
    std::cout << e1->get_status() << " " << e2->get_status() << " " << e3->get_status()
              << std::endl;

    REQUIRE(c0->get_date() == 25_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 35_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(
      1, ossia::expressions::comparator::EQUAL, 1));

  // TODO we would like another behaviour :
  // right now there is an additional tick needed for triggering e2 after e1, while we would like them to happen directly
  // however this means that c2 would not execute at all...
  {
    s.interval->tick(2_tv, default_request());
    std::cout << e1->get_status() << " " << e2->get_status() << " " << e3->get_status()
              << std::endl;

    // FIXME there is some indeterminism here. Hypothesis is that this is due because of a pointer set.
    //REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(c0->get_date() == 25_tv);
    REQUIRE(e1->get_status() & time_event::status::FINISHED);

    REQUIRE(c1->get_date() == 0_tv);
    //REQUIRE(e2->get_status() & time_event::status::FINISHED);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    //REQUIRE(c3->get_date() == 2_tv);
    REQUIRE(c3->get_date() == 0_tv);
  }

  {
    s.interval->tick(2_tv, default_request());
    std::cout << e1->get_status() << " " << e2->get_status() << " " << e3->get_status()
              << std::endl;

    REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(e1->get_status() & time_event::status::FINISHED);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() & time_event::status::FINISHED);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    //REQUIRE(c3->get_date() == 4_tv);
    REQUIRE(c3->get_date() == 2_tv);
  }
}

TEST_CASE("test_min", "test_min")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  start_and_tick(s.interval);
  s.interval->tick(1000_tv, default_request());
  REQUIRE(c0->get_date() == 1000_tv);
  REQUIRE(c1->get_date() == 0_tv);
  s.interval->tick(
      1500_tv, default_request()); // Get into the min; the node is triggered
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 0_tv);

  // whole tick goes in the next interval
  s.interval->tick(5000_tv, default_request());

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 5000_tv);
}

TEST_CASE("test_max", "test_max")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);

  start_and_tick(s.interval);
  s.interval->tick(3000_tv, default_request());
  REQUIRE(c0->get_date() == 3000_tv);
  REQUIRE(c1->get_date() == 0_tv);
  s.interval->tick(1500_tv, default_request()); // Go past the max
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 500_tv);
}

TEST_CASE("test_inter_tick", "test_inter_tick")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  std::shared_ptr<time_event> e3 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 3000_tv, 3000_tv, 3000_tv);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 500_tv, 500_tv, 500_tv);
  std::shared_ptr<time_interval> c2
      = create_interval({}, *e2, *e3, 3000_tv, 3000_tv, 3000_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  scenario.add_time_interval(c2);

  start_and_tick(s.interval);
  s.interval->tick(1000_tv, default_request());
  REQUIRE(c0->get_date() == 1000_tv);
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(c2->get_date() == 0_tv);
  s.interval->tick(3000_tv, default_request()); // Go past the max and way into c2
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(c2->get_date() == 500_tv);
}

TEST_CASE("test_unconnected", "test_unconnected")
{

  root_scenario s;
  using namespace ossia;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = create_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  s.scenario->add_time_interval(c0);

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);

  start_and_tick(s.interval);

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);

  s.interval->tick(1_tv, default_request());

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);

  s.interval->tick(1_tv, default_request());

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);
}

TEST_CASE("test_1_len_interval_state", "test_1_len_interval_state")
{
  using namespace ossia;
  root_scenario s;
  TestDevice utils;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  std::shared_ptr<time_event> e3 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  s.scenario->add_time_interval(c0);
  std::shared_ptr<time_interval> c1 = create_interval({}, *e1, *e2, 1_tv, 1_tv, 1_tv);
  s.scenario->add_time_interval(c1);
  std::shared_ptr<time_interval> c2 = create_interval({}, *e2, *e3, 20_tv, 20_tv, 20_tv);
  s.scenario->add_time_interval(c2);

  auto msg_node = std::make_shared<ossia::nodes::messages>();
  msg_node->data.push_back({*utils.float_addr, ossia::value(1.234)});

  auto msg_proc = std::make_shared<ossia::node_process>(msg_node);

  c1->add_time_process(msg_proc);
  s.interval->start();
  s.interval->tick(10_tv, default_request());

  for(auto tr : msg_node->requested_tokens)
    std::cout << " -- " << tr << "\n";
  REQUIRE(e0->get_status() & ossia::time_event::status::FINISHED);
  REQUIRE(e1->get_status() & ossia::time_event::status::FINISHED);
  REQUIRE(e2->get_status() & ossia::time_event::status::FINISHED);
  REQUIRE((int)msg_node->requested_tokens.size() == (int)1);
  auto t0 = msg_node->requested_tokens[0];
  auto expected = simple_token_request{.prev_date = 0_tv, .date = 1_tv, .offset = 2_tv};
  REQUIRE(t0 == expected);

  REQUIRE(utils.float_addr->value() == ossia::value(float(0.)));

  auto gg = std::make_unique<graph>();
  auto& g = *gg;
  ossia::execution_state e;
  g.add_node(msg_node);
  g.state(e);
  e.commit();

  REQUIRE(utils.float_addr->value() == ossia::value(float(1.234)));
  REQUIRE(c2->get_date() == 7_tv);
}

TEST_CASE("test_0_len_interval_state", "test_0_len_interval_state")
{
  using namespace ossia;
  root_scenario s;
  TestDevice utils;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  std::shared_ptr<time_event> e3 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  s.scenario->add_time_interval(c0);
  std::shared_ptr<time_interval> c1 = create_interval({}, *e1, *e2, 0_tv, 0_tv, 0_tv);
  s.scenario->add_time_interval(c1);
  std::shared_ptr<time_interval> c2 = create_interval({}, *e2, *e3, 20_tv, 20_tv, 20_tv);
  s.scenario->add_time_interval(c2);

  auto msg_node = std::make_shared<ossia::nodes::messages>();
  msg_node->data.push_back({*utils.float_addr, ossia::value(1.234)});

  auto msg_proc = std::make_shared<ossia::node_process>(msg_node);

  c1->add_time_process(msg_proc);
  s.interval->start();
  s.interval->tick(10_tv, default_request());

  for(auto tr : msg_node->requested_tokens)
    std::cout << " -- " << tr << "\n";
  REQUIRE(e0->get_status() & ossia::time_event::status::FINISHED);
  REQUIRE(e1->get_status() & ossia::time_event::status::FINISHED);
  REQUIRE(e2->get_status() & ossia::time_event::status::FINISHED);
  REQUIRE((int)msg_node->requested_tokens.size() == (int)1);
  auto t0 = msg_node->requested_tokens[0];
  auto expected = simple_token_request{.prev_date = 0_tv, .date = 0_tv, .offset = 2_tv};
  REQUIRE(t0 == expected);

  REQUIRE(utils.float_addr->value() == ossia::value(float(0.)));

  auto gg = std::make_unique<graph>();
  auto& g = *gg;
  ossia::execution_state e;
  g.add_node(msg_node);
  g.state(e);
  e.commit();

  REQUIRE(utils.float_addr->value() == ossia::value(float(1.234)));
  REQUIRE(c2->get_date() == 8_tv);
}

TEST_CASE("test_trigger", "test_trigger")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(
      1, ossia::expressions::comparator::EQUAL, 1));

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 30_tv, 20_tv, 40_tv);
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);

  start_and_tick(s.interval);
  s.interval->tick(15_tv, default_request());
  REQUIRE(c0->get_date() == 15_tv);
  REQUIRE(c1->get_date() == 0_tv);

  s.interval->tick(15_tv, default_request());
  std::cout << c0->get_date() << c1->get_date() << std::endl;
  REQUIRE(c0->get_date() == 0_tv); // executed, we go on to the next one
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(e1->get_status() & ossia::time_event::status::FINISHED);

  s.interval->tick(15_tv, default_request());
  std::cout << c0->get_date() << c1->get_date() << std::endl;
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 15_tv);
}

TEST_CASE("test_trigger_at_start", "test_trigger_at_start")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  scenario.get_start_time_sync()->set_expression(
      ossia::expressions::make_expression_false());

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 30_tv, 30_tv, 30_tv);

  scenario.add_time_interval(c0);

  REQUIRE(!scenario.get_start_time_sync()->is_evaluating());
  REQUIRE(c0->get_date() == 0_tv);

  start_and_tick(s.interval);
  s.interval->tick(15_tv, default_request());
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(scenario.get_start_time_sync()->is_evaluating());
}

TEST_CASE("test_speed", "test_speed") { }

TEST_CASE("test_tokens", "test_tokens")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  // Dummy processes are added, else token requests aren't registered as an optimization
  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 300_tv, 300_tv, 300_tv);
  c0->add_time_process(dummy_process());
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 500_tv, 500_tv, 500_tv);
  c1->add_time_process(dummy_process());

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  s.interval->start();
  s.interval->tick(700_tv, default_request());

  ossia::simple_token_request_vec expected0{
      {.prev_date = 0_tv, .date = 300_tv, .parent_duration = 300_tv}};

  REQUIRE(c0->node->requested_tokens == expected0);

  std::cout << c1->node->requested_tokens.size() << std::endl;
  std::cout << c1->node->requested_tokens[0] << std::endl;

  REQUIRE(c1->node->requested_tokens.size() == 1);

  ossia::simple_token_request_vec expected1{
      {.prev_date = 0_tv, .date = 400_tv, .parent_duration = 500_tv, .offset = 300_tv}};
  REQUIRE(c1->node->requested_tokens == expected1);
}

TEST_CASE("test_tokens_max", "test_tokens_max")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 300_tv, 300_tv, 300_tv);
  c0->add_time_process(dummy_process());
  std::shared_ptr<time_interval> c1
      = create_interval({}, *e0, *e1, 500_tv, 100_tv, 1000_tv);
  c1->add_time_process(dummy_process());

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  s.interval->start();
  s.interval->tick(700_tv, default_request());

  // In this case (when there are flexible bounds) we go as far as possible in the tick.
  // Else this would cause deadlocks if one interval reached its max before another reached its min
  ossia::simple_token_request_vec expected0{
      {.prev_date = 0_tv, .date = 300_tv, .parent_duration = 300_tv, .offset = 0_tv}};
  REQUIRE(c0->node->requested_tokens == expected0);
  std::cout << c1->node->requested_tokens.size() << std::endl;
  std::cout << c1->node->requested_tokens[0] << std::endl;
  REQUIRE(c1->node->requested_tokens.size() == 1);
  ossia::simple_token_request_vec expected1{
      {.prev_date = 0_tv, .date = 700_tv, .parent_duration = 500_tv, .offset = 0_tv}};
  REQUIRE(c1->node->requested_tokens == expected1);
}

TEST_CASE("test_tokens_min", "test_tokens_min")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 30_tv, 30_tv, 30_tv);
  c0->add_time_process(dummy_process());
  std::shared_ptr<time_interval> c1 = create_interval({}, *e0, *e1, 10_tv, 10_tv, 10_tv);
  c1->add_time_process(dummy_process());

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  s.interval->start();

  {
    s.interval->tick(20_tv, default_request());
    ossia::simple_token_request_vec expected0{
        {.prev_date = 0_tv, .date = 20_tv, .offset = 0_tv}};
    REQUIRE(c0->node->requested_tokens == expected0);
    std::cout << c1->node->requested_tokens.size() << std::endl;
    std::cout << c1->node->requested_tokens[0] << std::endl;
    REQUIRE(c1->node->requested_tokens.size() == 1);
    ossia::simple_token_request_vec expected1{
        {.prev_date = 0_tv, .date = 10_tv, .offset = 0_tv}};
    REQUIRE(c1->node->requested_tokens == expected1);

    REQUIRE(c0->get_date() == 20_tv);
    REQUIRE(c1->get_date() == 10_tv);
  }
  c0->node->requested_tokens.clear();
  c1->node->requested_tokens.clear();

  REQUIRE(e1->get_status() == time_event::status::NONE);
  {
    s.interval->tick(20_tv, default_request());
    ossia::simple_token_request_vec expected0{
        {.prev_date = 20_tv, .date = 30_tv, .offset = 0_tv}};
    std::cout << c0->node->requested_tokens << std::endl;
    std::cout << c1->node->requested_tokens << std::endl;
    REQUIRE(c0->node->requested_tokens.size() == 1);
    REQUIRE(c0->node->requested_tokens == expected0);

    REQUIRE(c1->node->requested_tokens.size() == 0);
  }

  REQUIRE(e1->get_status() & time_event::status::FINISHED);
}

TEST_CASE("test_autom", "test_autom")
{
  //
  //      using namespace ossia;
  //      root_scenario s;
  //      TestDevice utils;

  //      ossia::scenario& scenario = *s.scenario;
  //      std::shared_ptr<time_event> e0 = start_event(scenario);
  //      std::shared_ptr<time_event> e1 = create_event(scenario);

  //      std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  //      s.scenario->add_time_interval(c0);

  //      std::shared_ptr<ossia::automation> proc = std::make_shared<ossia::automation>();
  //      proc->set_destination(*utils.float_addr);

  //      auto crv = std::make_shared<curve<double, float>>();
  //      curve_segment_linear<float> linearSegment;

  //      crv->set_x0(0.);
  //      crv->set_y0(0.);
  //      crv->add_point(linearSegment, 1., 1.);

  //      proc->set_behavior(crv);
  //      c0->add_time_process(proc);

  //      start_and_tick(s.interval);
  //      s.interval->tick(1000_tv);
  //      s.interval->tick(999_tv);
  //      s.interval->tick(1_tv);
}

TEST_CASE("test_autom_and_state", "test_autom_and_state")
{
  //
  //      using namespace ossia;
  //      root_scenario s;
  //      TestDevice utils;

  //      ossia::scenario& scenario = *s.scenario;
  //      std::shared_ptr<time_event> e0 = start_event(scenario);
  //      std::shared_ptr<time_event> e1 = create_event(scenario);
  //      e0->add_state(ossia::message{*utils.float_addr, ossia::value{36.}});
  //      e1->add_state(ossia::message{*utils.float_addr, ossia::value{24.}});

  //      std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  //      s.scenario->add_time_interval(c0);

  //      std::shared_ptr<ossia::automation> proc = std::make_shared<ossia::automation>();
  //      proc->set_destination(*utils.float_addr);

  //      auto crv = std::make_shared<curve<double, float>>();
  //      curve_segment_linear<float> linearSegment;

  //      crv->set_x0(0.);
  //      crv->set_y0(0.);
  //      crv->add_point(linearSegment, 1., 1.);

  //      proc->set_behavior(crv);
  //      c0->add_time_process(proc);

  //      start_and_tick(s.interval);
  //      s.interval->tick(1000_tv);
  //      s.interval->tick(999_tv);
  //      s.interval->tick(1_tv);
}

static auto& get_value_state(ossia::execution_state& s)
{
  return static_cast<ossia::merged_execution_state_policy*>(s.m_policy.get())
      ->m_valueState;
};

TEST_CASE("test_percentage", "test_percentage")
{

  using namespace ossia;
  root_scenario s;
  TestDevice utils;

  auto gg = std::make_unique<graph>();
  auto& g = *gg;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  s.scenario->add_time_interval(c0);

  auto node = std::make_shared<ossia::nodes::percentage>(*utils.float_addr);
  auto proc = std::make_shared<ossia::node_process>(node);
  c0->add_time_process(proc);
  g.add_node(node);

  s.interval->start();
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(get_value_state(s).size() == 0);
  }

  s.interval->tick(1_tv, default_request());
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(get_value_state(s).size() == 1);
    REQUIRE(get_value_state(s).begin()->second.back().first.value == ossia::value(0.5f));
  }

  s.interval->tick(1_tv, default_request());
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(get_value_state(s).size() == 1);
    REQUIRE(get_value_state(s).begin()->second.back().first.value == ossia::value(1.f));
  }

  s.interval->tick(1_tv, default_request());
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(get_value_state(s).empty());
  }
}

TEST_CASE("test_percentage_long", "test_percentage_long")
{

  using namespace ossia;
  root_scenario s;
  TestDevice utils;
  auto gg = std::make_unique<graph>();
  auto& g = *gg;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 5_tv, 5_tv, 5_tv);
  s.scenario->add_time_interval(c0);

  auto node = std::make_shared<ossia::nodes::percentage>(*utils.float_addr);
  auto proc = std::make_shared<ossia::node_process>(node);
  c0->add_time_process(proc);
  g.add_node(node);

  s.interval->start();
  s.interval->tick(1_tv, default_request());
  REQUIRE(node->requested_tokens.size() == 1);
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(node->requested_tokens.size() == 0);
    REQUIRE(get_value_state(s).size() == 1);
    REQUIRE(get_value_state(s).begin()->second.back().first.value == ossia::value(0.2f));
  }

  s.interval->tick(2_tv, default_request());
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(get_value_state(s).size() == 1);
    REQUIRE(
        get_value_state(s).begin()->second.back().first.value == ossia::value(3. / 5.));
  }

  s.interval->tick(7_tv, default_request());
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(get_value_state(s).size() == 1);
    REQUIRE(get_value_state(s).begin()->second.back().first.value == ossia::value(1.f));
  }

  s.interval->tick(1_tv, default_request());
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(get_value_state(s).empty());
  }
}

TEST_CASE("test_offset", "test_offset")
{
  // TODO reinstate me
  //        using namespace ossia;
  //        root_scenario s;
  //        TestDevice utils;

  //        ossia::scenario& scenario = *s.scenario;
  //        std::shared_ptr<time_event> e0 = start_event(scenario);
  //        std::shared_ptr<time_event> e1 = create_event(scenario);
  //        std::shared_ptr<time_event> e2 = create_event(scenario);
  //        std::shared_ptr<time_event> e3 = create_event(scenario);

  //        e1->add_state(ossia::message{*utils.float_addr, ossia::value{24.}});
  //        e2->add_state(ossia::message{*utils.float_addr, ossia::value{31.}});

  //        std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 5_tv, 5_tv, 5_tv);
  //        s.scenario->add_time_interval(c0);
  //        std::shared_ptr<time_interval> c1 = create_interval({}, *e1, *e2, 5_tv, 5_tv, 5_tv);
  //        s.scenario->add_time_interval(c1);
  //        std::shared_ptr<time_interval> c2 = create_interval({}, *e2, *e3, 5_tv, 5_tv, 5_tv);
  //        s.scenario->add_time_interval(c1);

  //        {
  //            auto st = s.interval->offset(7_tv);
  //            ossia::print(std::cout, st);

  //            ossia::state expected;
  //            {
  //                ossia::state sub;
  //                sub.add(ossia::message{*utils.float_addr, ossia::value{24.}});
  //                expected.add(sub);
  //            }
  //            REQUIRE(st == expected);
  //        }

  //        {
  //            ossia::state st;

  //            ossia::state expected;

  //            s.interval->start_and_tick(st);
  //            ossia::print(std::cout, st);

  //            REQUIRE(st == expected);
  //        }

  //        s.interval->stop();

  //        {
  //            auto st = s.interval->offset(13_tv);
  //            ossia::print(std::cout, st);

  //            ossia::state expected;
  //            {
  //                ossia::state sub;
  //                sub.add(ossia::message{*utils.float_addr, ossia::value{31.}});
  //                expected.add(sub);
  //            }
  //            REQUIRE(st == expected);
  //        }

  //        {
  //            ossia::state st;

  //            ossia::state expected;

  //            s.interval->start_and_tick(st);
  //            ossia::print(std::cout, st);

  //            REQUIRE(st == expected);
  //        }
}

TEST_CASE("test_musical_bar", "test_musical_bar")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  // assume sampling rate is 44100 -> a quarter note is 22050
  e1->get_time_sync().set_sync_rate(1);

  /*                c0                  c1
    * e0 - - - - - - - - - - e1 - - - - - - - - - - e2
   */

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 0_tv, 0_tv, ossia::Infinite);
  c0->add_time_process(dummy_process());

  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 0_tv, 0_tv, ossia::Infinite);
  c1->add_time_process(dummy_process());

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  start_and_tick(s.interval);
  {
    s.interval->tick(5_tv, default_request());

    REQUIRE(c0->get_date() == 5_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(
      1, ossia::expressions::comparator::EQUAL, 1));

  // Tick a bit : nothing happens
  {
    s.interval->tick(500_tv, default_request());

    REQUIRE(c0->get_date() == 505_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }
  {
    s.interval->tick(500_tv, default_request());

    REQUIRE(c0->get_date() == 1005_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }
  {
    s.interval->tick(22050_tv - 1005_tv, default_request());

    REQUIRE(c0->get_date() == 22050_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  c0->node->requested_tokens.clear();
  c1->node->requested_tokens.clear();
  // Tick enough to go past the next quantification value :
  {
    ossia::token_request req;
    req.tempo = 120;
    req.speed = 1.;
    req.musical_start_last_bar = 0.;
    req.musical_end_last_bar = 4.;
    req.musical_start_position = 1;
    req.musical_end_position = 5;
    req.signature = {4, 4};
    std::cout << "start musical tick:" << std::endl;
    s.interval->tick(88200_tv, req); // go forward a whole bar
    std::cout << std::flush;
    std::cout << std::flush;

    REQUIRE(e1->get_status() & time_event::status::FINISHED);
    ossia::simple_token_request_vec expected0{
        {.prev_date = 22050_tv, .date = 88199_tv, .offset = 0_tv}};

    REQUIRE(c0->get_date() == 0_tv);

    REQUIRE(!c0->node->requested_tokens.empty());
    REQUIRE(c0->node->requested_tokens == expected0);

    ossia::simple_token_request_vec expected1{
        ossia::simple_token_request{
            .prev_date = 0_tv, .date = 22051_tv, .offset = 66149_tv}
        // == 88200_tv - 22050_tv
    };

    REQUIRE(c1->get_date() == 22051_tv);
    REQUIRE(!c1->node->requested_tokens.empty());
    REQUIRE(c1->node->requested_tokens == expected1);
    REQUIRE(e2->get_status() == time_event::status::PENDING);
  }
}

TEST_CASE("test_musical_bar_offset", "test_musical_bar_offset")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> ex = create_event(scenario);
  ex->get_time_sync().set_expression(ossia::expressions::make_expression_true());
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  // assume sampling rate is 44100 -> a quarter note is 22050
  e1->get_time_sync().set_sync_rate(1);

  /*                      c0                  c1
    * ex -- e0 - - - - - - - - - - e1 - - - - - - - - - - e2
   */

  std::shared_ptr<time_interval> cx
      = create_interval({}, *e0, *ex, 88200_tv * 1.5, 88200_tv * 1.5, 88200_tv * 1.5);
  cx->add_time_process(dummy_process());

  std::shared_ptr<time_interval> c0
      = create_interval({}, *ex, *e1, 0_tv, 0_tv, ossia::Infinite);
  c0->add_time_process(dummy_process());

  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 0_tv, 0_tv, ossia::Infinite);
  c1->add_time_process(dummy_process());

  s.scenario->add_time_interval(cx);
  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  start_and_tick(s.interval);
  {
    // we go past cx, 1/4 in c0
    s.interval->tick(88200_tv * 2, default_request());

    REQUIRE(c0->get_date() == 88200_tv * 0.5);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(
      1, ossia::expressions::comparator::EQUAL, 1));

  // Tick a bit : nothing happens
  {
    s.interval->tick(500_tv, default_request());

    REQUIRE(c0->get_date() == 88200_tv * 0.5 + 500_tv); // 44600
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  c0->node->requested_tokens.clear();
  c1->node->requested_tokens.clear();
  // Tick enough to go past the next quantification value :
  {
    // globally: we're at 88200 * 2 + 500, we are going to 88200 * 3
    // locally : c0 is at 44600.
    //  -> In absolute time it has started at 88200_tv * 1.5 (132300)
    //  -> Next quantification step is 88200 * 3
    //  -> 88200 * 3 - (88200 * 2 + 500) = 87700
    //  -> c0 has to go to 44600 + 87700
    //  -> c1 will go from 0 to 500
    ossia::token_request req;
    req.tempo = 120;
    req.speed = 1.;
    req.musical_start_last_bar = 8.;
    req.musical_end_last_bar = 12.;
    req.musical_start_position = (88200_tv * 2 + 500_tv).impl / 22050.;
    req.musical_end_position = (88200_tv * 3 + 500_tv).impl / 22050.;
    req.signature = {4, 4};
    std::cout << "start musical tick:" << std::endl;
    s.interval->tick(88200_tv, req); // go forward a whole bar
    std::cout << std::flush;
    std::cout << std::flush;

    REQUIRE(e1->get_status() & time_event::status::FINISHED);
    ossia::simple_token_request_vec expected0{
        {.prev_date = 44600_tv, .date = 44600_tv + 87700_tv - 1_tv, .offset = 0_tv}};

    REQUIRE(c0->get_date() == 0_tv);

    REQUIRE(!c0->node->requested_tokens.empty());
    REQUIRE(c0->node->requested_tokens == expected0);

    ossia::simple_token_request_vec expected1{
        {.prev_date = 0_tv, .date = 501_tv, .offset = 88200_tv - 501_tv}};

    REQUIRE(c1->get_date() == 501_tv);
    REQUIRE(!c1->node->requested_tokens.empty());
    REQUIRE(c1->node->requested_tokens == expected1);
    REQUIRE(e2->get_status() == time_event::status::PENDING);
  }
}

TEST_CASE("test_musical_quarter", "test_musical_quarter")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  // assume sampling rate is 44100 -> a quarter note is 22050
  e1->get_time_sync().set_sync_rate(4);

  /*                c0                  c1
    * e0 - - - - - - - - - - e1 - - - - - - - - - - e2
   */

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 0_tv, 0_tv, ossia::Infinite);
  c0->add_time_process(dummy_process());

  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 0_tv, 0_tv, ossia::Infinite);
  c1->add_time_process(dummy_process());

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  start_and_tick(s.interval);
  {
    s.interval->tick(50_tv, default_request());

    REQUIRE(c0->get_date() == 50_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(
      1, ossia::expressions::comparator::EQUAL, 1));

  // Tick a bit : nothing happens
  {
    ossia::token_request req;
    req.tempo = 120;
    req.speed = 1.;
    req.musical_start_last_bar = 0.;
    req.musical_end_last_bar = 0.;
    req.musical_start_position = 50. / 22050.;
    req.musical_end_position = 20050. / 22050.;
    req.signature = {4, 4};
    s.interval->tick(20000_tv, req);

    REQUIRE(c0->get_date() == 20050_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  c0->node->requested_tokens.clear();
  c1->node->requested_tokens.clear();
  // Tick enough to go past the next quantification value :
  {
    ossia::token_request req;
    req.tempo = 120;
    req.speed = 1.;
    req.musical_start_last_bar = 0.;
    req.musical_end_last_bar = 0.;
    req.musical_start_position = 20050. / 22050.;
    req.musical_end_position = 25050. / 22050.;
    req.signature = {4, 4};
    std::cout << "start musical tick:" << std::endl;
    s.interval->tick(5000_tv, req); // go forward past the first quarter note
    std::cout << std::flush;
    std::cout << std::flush;

    REQUIRE(e1->get_status() & time_event::status::FINISHED);
    ossia::simple_token_request_vec expected0{
        {.prev_date = 20050_tv, .date = 22049_tv, .offset = 0_tv}};

    REQUIRE(c0->get_date() == 0_tv);

    REQUIRE(!c0->node->requested_tokens.empty());
    REQUIRE(c0->node->requested_tokens == expected0);

    ossia::simple_token_request_vec expected1{
        ossia::simple_token_request{
            .prev_date = 0_tv, .date = 3001_tv, .offset = 1999_tv}
        // == 88200_tv - 22050_tv
    };

    REQUIRE(c1->get_date() == 3001_tv);
    REQUIRE(!c1->node->requested_tokens.empty());
    REQUIRE(c1->node->requested_tokens == expected1);
    REQUIRE(e2->get_status() == time_event::status::PENDING);
  }
}

TEST_CASE("test_musical_eighth", "test_musical_eighth")
{
  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  // assume sampling rate is 44100 -> a quarter note is 22050
  e1->get_time_sync().set_sync_rate(8);

  /*                c0                  c1
    * e0 - - - - - - - - - - e1 - - - - - - - - - - e2
   */

  std::shared_ptr<time_interval> c0
      = create_interval({}, *e0, *e1, 0_tv, 0_tv, ossia::Infinite);
  c0->add_time_process(dummy_process());

  std::shared_ptr<time_interval> c1
      = create_interval({}, *e1, *e2, 0_tv, 0_tv, ossia::Infinite);
  c1->add_time_process(dummy_process());

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  start_and_tick(s.interval);
  {
    s.interval->tick(50_tv, default_request());

    REQUIRE(c0->get_date() == 50_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(
      1, ossia::expressions::comparator::EQUAL, 1));

  // Tick a bit : nothing happens
  {
    ossia::token_request req;
    req.tempo = 120;
    req.speed = 1.;
    req.musical_start_last_bar = 0.;
    req.musical_end_last_bar = 0.;
    req.musical_start_position = 50. / 22050.;
    req.musical_end_position = 60. / 22050.;
    req.signature = {4, 4};
    s.interval->tick(10_tv, req);

    REQUIRE(c0->get_date() == 60_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);
  }

  c0->node->requested_tokens.clear();
  c1->node->requested_tokens.clear();
  // Tick enough to go past the next quantification value :
  {
    ossia::token_request req;
    req.tempo = 120;
    req.speed = 1.;
    req.musical_start_last_bar = 0.;
    req.musical_end_last_bar = 0.;
    req.musical_start_position = 60. / 22050.;
    req.musical_end_position = (60. + 20000.) / 22050.;
    req.signature = {4, 4};
    std::cout << "start musical tick:" << std::endl;
    s.interval->tick(20000_tv, req); // go forward past the first quarter note
    std::cout << std::flush;
    std::cout << std::flush;

    REQUIRE(e1->get_status() & time_event::status::FINISHED);
    ossia::simple_token_request_vec expected0{
        {.prev_date = 60_tv, .date = 11024_tv, .offset = 0_tv}};

    REQUIRE(c0->get_date() == 0_tv);

    REQUIRE(!c0->node->requested_tokens.empty());
    REQUIRE(c0->node->requested_tokens == expected0);

    ossia::simple_token_request_vec expected1{
        {.prev_date = 0_tv, .date = 9036_tv, .offset = 10964_tv}};

    REQUIRE(c1->get_date() == 9036_tv);
    REQUIRE(!c1->node->requested_tokens.empty());
    REQUIRE(c1->node->requested_tokens == expected1);
    REQUIRE(e2->get_status() == time_event::status::PENDING);
  }
}

TEST_CASE("test_exec_chain_loop", "test_exec_chain_loop")
{
  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = create_interval({}, *e0, *e1, 10_tv, 10_tv, 10_tv);
  std::shared_ptr<time_interval> c1 = create_interval({}, *e1, *e0, 10_tv, 10_tv, 10_tv);
  c1->graphal = true;

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  auto print = [&] {
    std::cout << e0->get_status() << " " << e1->get_status() << " -- " << c0->get_date()
              << " " << c1->get_date() << std::endl;
  };

  scenario.start();
  scenario.state(default_request(0_tv, 0_tv));

  // +5
  scenario.state(default_request(0_tv, 5_tv));
  print();

  REQUIRE(c0->get_date() == 5_tv);
  REQUIRE(c1->get_date() == 0_tv);

  // +10
  scenario.state(default_request(5_tv, 15_tv));
  print();

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 5_tv);

  // +10
  scenario.state(default_request(15_tv, 25_tv));
  print();

  REQUIRE(c0->get_date() == 5_tv);
  REQUIRE(c1->get_date() == 0_tv);

  // +10
  scenario.state(default_request(25_tv, 35_tv));
  print();

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 5_tv);

  // +10
  scenario.state(default_request(35_tv, 45_tv));
  print();

  REQUIRE(c0->get_date() == 5_tv);
  REQUIRE(c1->get_date() == 0_tv);
}
