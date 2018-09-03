// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>


#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/nodes/messages.hpp>
#include <ossia/dataflow/nodes/percentage.hpp>

#include "TestUtils.hpp"


auto create_event(ossia::scenario& s)
{
  auto en = std::make_shared<ossia::time_sync>();
  en->set_expression(ossia::expressions::make_expression_true());
  auto ee = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *en, ossia::expressions::make_expression_true());
  en->insert(en->get_time_events().end(), ee);
  s.add_time_sync(std::move(en));
  return ee;
}

auto start_event(ossia::scenario& s)
{
  auto sn = s.get_start_time_sync();
  return *sn->get_time_events().begin();
}

TEST_CASE ("test_exec_simple", "test_exec_simple")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> se = start_event(scenario);
  std::shared_ptr<time_event> ee = create_event(scenario);

  std::shared_ptr<time_interval> c = time_interval::create({}, *se, *ee, 3000_tv, 3000_tv, 3000_tv);
  s.scenario->add_time_interval(c);

  s.interval->start_and_tick();
  REQUIRE(se->get_status() == time_event::status::HAPPENED);
  REQUIRE(c->get_date() == 0_tv);

  s.interval->tick(1000_tv);
  REQUIRE(c->get_date() == 1000_tv);
  s.interval->tick(1000_tv);
  REQUIRE(c->get_date() == 2000_tv);
  s.interval->tick(999_tv);
  REQUIRE(c->get_date() == 2999_tv);
  s.interval->tick(1_tv); // The interval is stopped

  REQUIRE(c->get_date() == 0_tv);
  REQUIRE(c->get_end_event().get_status() == time_event::status::HAPPENED);
  s.interval->tick(1000_tv);
  REQUIRE(c->get_date() == 0_tv);
  REQUIRE(c->get_end_event().get_status() == time_event::status::HAPPENED);
  s.interval->tick(1000_tv);
  REQUIRE(c->get_date() == 0_tv);
  REQUIRE(c->get_end_event().get_status() == time_event::status::HAPPENED);
}

TEST_CASE ("test_exec_chain_long", "test_exec_chain_long")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 3000_tv, 3000_tv, 3000_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 3000_tv, 3000_tv, 3000_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  s.interval->start_and_tick();
  s.interval->tick(500_tv);
  REQUIRE(c0->get_date() == 500_tv);
  REQUIRE(c1->get_date() == 0_tv);
  s.interval->tick(3000_tv);

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 500_tv);
}


TEST_CASE ("test_exec_chain_tokens", "test_exec_chain_tokens")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 10_tv, 10_tv, 10_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 100_tv, 100_tv, 100_tv);

  // {
  auto n = std::make_shared<ossia::nodes::messages>();
  auto proc = std::make_shared<ossia::node_process>(n);
  c1->add_time_process(proc);
  //} auto n = c1->node;
  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  s.interval->start_and_tick();


  s.interval->tick(50_tv);
  REQUIRE(c1->get_date() == 40_tv);
  for(auto tok : n->requested_tokens)
  {
    std::cerr << tok;
  }

  REQUIRE((int)n->requested_tokens.size() == (int)2);
  REQUIRE(n->requested_tokens[0] == (token_request{0_tv, 0_tv, 0., 10_tv}));
  REQUIRE(n->requested_tokens[1] == (token_request{0_tv, 40_tv, 40. / 100., 10_tv}));
  n->requested_tokens.clear();

  s.interval->tick(50_tv);
  REQUIRE(c1->get_date() == 90_tv);
  for(auto tok : n->requested_tokens)
  {
    std::cerr << tok;
  }

  REQUIRE((int)n->requested_tokens.size() == (int)1);
  REQUIRE(n->requested_tokens[0] == (token_request{40_tv, 90_tv, 90. / 100., 0_tv}));
  n->requested_tokens.clear();

}

TEST_CASE ("test_exec_chain_multi", "test_exec_chain_multi")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  std::shared_ptr<time_event> e3 = create_event(scenario);
  std::shared_ptr<time_event> e4 = create_event(scenario);


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 1_tv, 1_tv, 1_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 1_tv, 1_tv, 1_tv);
  std::shared_ptr<time_interval> c2 = time_interval::create({}, *e2, *e3, 1_tv, 1_tv, 1_tv);
  std::shared_ptr<time_interval> c3 = time_interval::create({}, *e3, *e4, 10_tv, 10_tv, 10_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);
  s.scenario->add_time_interval(c2);
  s.scenario->add_time_interval(c3);


  s.interval->start_and_tick();
  s.interval->tick(5_tv);
  std::cerr << e0->get_status() << " "
            << e1->get_status() << " "
            << e2->get_status() << " "
            << e3->get_status() << std::endl;

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(c2->get_date() == 0_tv);
  REQUIRE(c3->get_date() == 2_tv);
}

TEST_CASE ("test_exec_chain_multi_infinite", "test_exec_chain_multi_infinite")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario); e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario); e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e3 = create_event(scenario); e3->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e4 = create_event(scenario); e4->get_time_sync().set_expression(ossia::expressions::make_expression_false());


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 1_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 1_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c2 = time_interval::create({}, *e2, *e3, 1_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c3 = time_interval::create({}, *e3, *e4, 1_tv, 0_tv, ossia::Infinite);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);
  s.scenario->add_time_interval(c2);
  s.scenario->add_time_interval(c3);


  s.interval->start_and_tick();
  {
    s.interval->tick(5_tv);
    std::cerr << e1->get_status() << " "
             << e2->get_status() << " "
             << e3->get_status() << " "
             << e4->get_status();

    REQUIRE(c0->get_date() == 5_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
    REQUIRE(e4->get_status() == time_event::status::NONE);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_true());

  // The "interactivity" tick introduced
  {
    s.interval->tick(5_tv);
    std::cerr << e1->get_status() << " "
             << e2->get_status() << " "
             << e3->get_status() << " "
             << e4->get_status();

    REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(e1->get_status() == time_event::status::HAPPENED);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
    REQUIRE(e4->get_status() == time_event::status::NONE);
  }

  {
    s.interval->tick(5_tv);
    std::cerr << e1->get_status() << " "
             << e2->get_status() << " "
             << e3->get_status() << " "
             << e4->get_status();

    REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(e1->get_status() == time_event::status::HAPPENED);

    REQUIRE(c1->get_date() == 5_tv);
    REQUIRE(e2->get_status() == time_event::status::PENDING);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
    REQUIRE(e4->get_status() == time_event::status::NONE);
  }
}


TEST_CASE ("test_exec_two_branch_infinite", "test_exec_two_branch_infinite")
{

  using namespace ossia;

  root_scenario s;
  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario); e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e2 = create_event(scenario); e2->get_time_sync().set_expression(ossia::expressions::make_expression_false());
  std::shared_ptr<time_event> e3 = create_event(scenario); e3->get_time_sync().set_expression(ossia::expressions::make_expression_false());

  /*           c0
       * e0 - - - - - - - - - - e2 - )
       * |    c1         c2     |      c3
       * | - - - - e1 - - - - - | ------------ e3
       */

  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e2, 20_tv, 0_tv, 25_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e0, *e1, 15_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c2 = time_interval::create({}, *e1, *e2, 5_tv, 0_tv, ossia::Infinite);
  std::shared_ptr<time_interval> c3 = time_interval::create({}, *e2, *e3, 100_tv, 100_tv, 100_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);
  s.scenario->add_time_interval(c2);
  s.scenario->add_time_interval(c3);

  s.interval->start_and_tick();
  {
    s.interval->tick(5_tv);
    std::cerr << e1->get_status() << " "
             << e2->get_status() << " "
             << e3->get_status();

    REQUIRE(c0->get_date() == 5_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 5_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
  }

  {
    s.interval->tick(30_tv);
    std::cerr << e1->get_status() << " "
             << e2->get_status() << " "
             << e3->get_status();

    REQUIRE(c0->get_date() == 25_tv);
    REQUIRE(e1->get_status() == time_event::status::PENDING);

    REQUIRE(c1->get_date() == 35_tv);
    REQUIRE(e2->get_status() == time_event::status::NONE);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 0_tv);
  }

  e1->get_time_sync().set_expression(ossia::expressions::make_expression_true());

  {
    s.interval->tick(2_tv);
    std::cerr << e1->get_status() << " "
             << e2->get_status() << " "
             << e3->get_status();

    REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(e1->get_status() == time_event::status::HAPPENED);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::HAPPENED);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 2_tv);
  }

  {
    s.interval->tick(2_tv);
    std::cerr << e1->get_status() << " "
             << e2->get_status() << " "
             << e3->get_status();

    REQUIRE(c0->get_date() == 0_tv);
    REQUIRE(e1->get_status() == time_event::status::HAPPENED);

    REQUIRE(c1->get_date() == 0_tv);
    REQUIRE(e2->get_status() == time_event::status::HAPPENED);

    REQUIRE(c2->get_date() == 0_tv);
    REQUIRE(e3->get_status() == time_event::status::NONE);

    REQUIRE(c3->get_date() == 4_tv);
  }

}

TEST_CASE ("test_min", "test_min")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

  s.scenario->add_time_interval(c0);
  s.scenario->add_time_interval(c1);

  s.interval->start_and_tick();
  s.interval->tick(1000_tv);
  REQUIRE(c0->get_date() == 1000_tv);
  REQUIRE(c1->get_date() == 0_tv);
  s.interval->tick(1500_tv); // Get into the min; the node is triggered
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 0_tv);

  // whole tick goes in the next interval
  s.interval->tick(5000_tv);

  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 5000_tv);
}

TEST_CASE ("test_max", "test_max")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 3000_tv, 2000_tv, 4000_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);

  s.interval->start_and_tick();
  s.interval->tick(3000_tv);
  REQUIRE(c0->get_date() == 3000_tv);
  REQUIRE(c1->get_date() == 0_tv);
  s.interval->tick(1500_tv); // Go past the max
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 500_tv);
}



TEST_CASE ("test_inter_tick", "test_inter_tick")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  std::shared_ptr<time_event> e3 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 3000_tv, 3000_tv, 3000_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 500_tv, 500_tv, 500_tv);
  std::shared_ptr<time_interval> c2 = time_interval::create({}, *e2, *e3, 3000_tv, 3000_tv, 3000_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  scenario.add_time_interval(c2);

  s.interval->start_and_tick();
  s.interval->tick(1000_tv);
  REQUIRE(c0->get_date() == 1000_tv);
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(c2->get_date() == 0_tv);
  s.interval->tick(3000_tv); // Go past the max and way into c2
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(c2->get_date() == 500_tv);
}

TEST_CASE ("test_unconnected", "test_unconnected")
{


  root_scenario s;
  using namespace ossia;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = create_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  s.scenario->add_time_interval(c0);

  REQUIRE(c0->get_position() == 0.);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);

  s.interval->start_and_tick();

  REQUIRE(c0->get_position() == 0.);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);

  s.interval->tick(1_tv);

  REQUIRE(c0->get_position() == 0.);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);

  s.interval->tick(1_tv);

  REQUIRE(c0->get_position() == 0.);
  REQUIRE(e0->get_status() == time_event::status::NONE);
  REQUIRE(e1->get_status() == time_event::status::NONE);
}


TEST_CASE ("test_simulated_state", "test_simulated_state")
{

  using namespace ossia;
  root_scenario s;
  TestDevice utils;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  std::shared_ptr<time_event> e3 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  s.scenario->add_time_interval(c0);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 0_tv, 0_tv, 0_tv);
  s.scenario->add_time_interval(c1);
  std::shared_ptr<time_interval> c2 = time_interval::create({}, *e2, *e3, 20_tv, 20_tv, 20_tv);
  s.scenario->add_time_interval(c2);

  auto msg_node = std::make_shared<ossia::nodes::messages>();
  msg_node->data.push_back({*utils.float_addr, ossia::value(1.234)});

  auto msg_proc = std::make_shared<ossia::node_process>(msg_node);

  c1->add_time_process(msg_proc);
  s.interval->start();
  s.interval->tick(10_tv);

  for(auto tr : msg_node->requested_tokens)
    std::cerr << tr;
  REQUIRE((int)msg_node->requested_tokens.size() == (int) 1);
  auto t0 = msg_node->requested_tokens[0];
  auto expected = token_request{0_tv, 0_tv, 0., 2_tv};
  REQUIRE(t0 == expected);

  REQUIRE(utils.float_addr->value() == ossia::value(float(0.)));

  ossia::graph g;
  ossia::execution_state e;
  g.add_node(msg_node);
  g.state(e);
  e.commit();

  REQUIRE(utils.float_addr->value() == ossia::value(float(1.234)));
  REQUIRE(c2->get_date() == 8_tv);
}

TEST_CASE ("test_trigger", "test_trigger")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_atom(1, ossia::expressions::comparator::EQUAL, 1));


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 30_tv, 20_tv, 40_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 100000_tv, 100000_tv, 100000_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);

  s.interval->start_and_tick();
  s.interval->tick(15_tv);
  REQUIRE(c0->get_date() == 15_tv);
  REQUIRE(c1->get_date() == 0_tv);

  s.interval->tick(15_tv);
  std::cerr << c0->get_date() << c1->get_date();
  REQUIRE(c0->get_date() == 0_tv); // executed, we go on to the next one
  REQUIRE(c1->get_date() == 0_tv);
  REQUIRE(e1->get_status() == ossia::time_event::status::HAPPENED);

  s.interval->tick(15_tv);
  std::cerr << c0->get_date() << c1->get_date();
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(c1->get_date() == 15_tv);

}


TEST_CASE ("test_trigger_at_start", "test_trigger_at_start")
{
  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  scenario.get_start_time_sync()->set_expression(ossia::expressions::make_expression_false());

  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 30_tv, 30_tv, 30_tv);

  scenario.add_time_interval(c0);

  REQUIRE(!scenario.get_start_time_sync()->is_evaluating());
  REQUIRE(c0->get_date() == 0_tv);

  s.interval->start_and_tick();
  s.interval->tick(15_tv);
  REQUIRE(c0->get_date() == 0_tv);
  REQUIRE(scenario.get_start_time_sync()->is_evaluating());
}

TEST_CASE ("test_speed", "test_speed")
{

}


TEST_CASE ("test_tokens", "test_tokens")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  std::shared_ptr<time_event> e2 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 300_tv, 300_tv, 300_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 500_tv, 500_tv, 500_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  s.interval->start();
  s.interval->tick(700_tv);

  ossia::small_vector<token_request, 4> expected0{{0_tv, 0_tv, 0., 0_tv}, {0_tv, 300_tv, 1., 0_tv}};
  REQUIRE(c0->node->requested_tokens == expected0);
  std::cerr << c1->node->requested_tokens.size();
  std::cerr << c1->node->requested_tokens[0];
  REQUIRE(c1->node->requested_tokens.size() == 2);
  std::cerr << c1->node->requested_tokens[1];
  ossia::small_vector<token_request, 4> expected1{{0_tv, 0_tv, 0., 300_tv}, {0_tv, 400_tv, 4./5., 300_tv}};
  REQUIRE(c1->node->requested_tokens == expected1);

}



TEST_CASE ("test_tokens_max", "test_tokens_max")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 300_tv, 300_tv, 300_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e0, *e1, 500_tv, 100_tv, 1000_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  s.interval->start();
  s.interval->tick(700_tv);

  // In this case (when there are flexible bounsd) we go as far as possible in the tick.
  // Else this would cause deadlocks if one interval reached its max before another reached its min
  ossia::small_vector<token_request, 4> expected0{{0_tv, 0_tv, 0., 0_tv}, {0_tv, 300_tv, 1., 0_tv}};
  REQUIRE(c0->node->requested_tokens == expected0);
  std::cerr << c1->node->requested_tokens.size();
  std::cerr << c1->node->requested_tokens[0];
  REQUIRE(c1->node->requested_tokens.size() == 2);
  std::cerr << c1->node->requested_tokens[1];
  ossia::small_vector<token_request, 4> expected1{{0_tv, 0_tv, 0., 0_tv}, {0_tv, 700_tv, 7./5., 0_tv}};
  REQUIRE(c1->node->requested_tokens == expected1);
}


TEST_CASE ("test_tokens_min", "test_tokens_min")
{

  using namespace ossia;
  root_scenario s;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);
  e1->get_time_sync().set_expression(ossia::expressions::make_expression_false());


  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 30_tv, 30_tv, 30_tv);
  std::shared_ptr<time_interval> c1 = time_interval::create({}, *e0, *e1, 10_tv, 10_tv, 10_tv);

  scenario.add_time_interval(c0);
  scenario.add_time_interval(c1);
  s.interval->start();

  {
    s.interval->tick(20_tv);
    ossia::small_vector<token_request, 4> expected0{{0_tv, 0_tv, 0., 0_tv}, {0_tv, 20_tv, 2./3., 0_tv}};
    REQUIRE(c0->node->requested_tokens == expected0);
    std::cerr << c1->node->requested_tokens.size();
    std::cerr << c1->node->requested_tokens[0];
    REQUIRE(c1->node->requested_tokens.size() == 2);
    std::cerr << c1->node->requested_tokens[1];
    ossia::small_vector<token_request, 4> expected1{{0_tv, 0_tv, 0., 0_tv}, {0_tv, 10_tv, 1., 0_tv}};
    REQUIRE(c1->node->requested_tokens == expected1);
  }
  c0->node->requested_tokens.clear();
  c1->node->requested_tokens.clear();

  REQUIRE(e1->get_status() == time_event::status::NONE);
  {
    s.interval->tick(20_tv);
    ossia::small_vector<token_request, 4> expected0{{0_tv, 30_tv, 1., 0_tv}};
    std::cerr << c0->node->requested_tokens;
    std::cerr << c1->node->requested_tokens;
    REQUIRE(c0->node->requested_tokens.size() == 1);
    REQUIRE(c0->node->requested_tokens == expected0);

    REQUIRE(c1->node->requested_tokens.size() == 0);
  }

  REQUIRE(e1->get_status() == time_event::status::HAPPENED);

}


TEST_CASE ("test_autom", "test_autom")
{
  //
  //      using namespace ossia;
  //      root_scenario s;
  //      TestDevice utils;

  //      ossia::scenario& scenario = *s.scenario;
  //      std::shared_ptr<time_event> e0 = start_event(scenario);
  //      std::shared_ptr<time_event> e1 = create_event(scenario);

  //      std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
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

  //      s.interval->start_and_tick();
  //      s.interval->tick(1000_tv);
  //      s.interval->tick(999_tv);
  //      s.interval->tick(1_tv);
}

TEST_CASE ("test_autom_and_state", "test_autom_and_state")
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


  //      std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
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

  //      s.interval->start_and_tick();
  //      s.interval->tick(1000_tv);
  //      s.interval->tick(999_tv);
  //      s.interval->tick(1_tv);
}


TEST_CASE ("test_percentage", "test_percentage")
{

  using namespace ossia;
  root_scenario s;
  TestDevice utils;
  ossia::graph g;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 2_tv, 2_tv, 2_tv);
  s.scenario->add_time_interval(c0);

  auto node = std::make_shared<ossia::nodes::percentage>(*utils.float_addr);
  auto proc = std::make_shared<ossia::node_process>(node);
  c0->add_time_process(proc);
  g.add_node(node);


  s.interval->start_and_tick();
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.size() == 1);
    REQUIRE(s.m_valueState.begin()->second.back().first.value == ossia::value(0.f));
  }

  s.interval->tick(1_tv);
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.size() == 1);
    REQUIRE(s.m_valueState.begin()->second.back().first.value == ossia::value(0.5f));
  }

  s.interval->tick(1_tv);
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.size() == 1);
    REQUIRE(s.m_valueState.begin()->second.back().first.value == ossia::value(1.f));
  }

  s.interval->tick(1_tv);
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.empty());
  }
}

TEST_CASE ("test_percentage_long", "test_percentage_long")
{

  using namespace ossia;
  root_scenario s;
  TestDevice utils;
  ossia::graph g;

  ossia::scenario& scenario = *s.scenario;
  std::shared_ptr<time_event> e0 = start_event(scenario);
  std::shared_ptr<time_event> e1 = create_event(scenario);

  std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 5_tv, 5_tv, 5_tv);
  s.scenario->add_time_interval(c0);

  auto node = std::make_shared<ossia::nodes::percentage>(*utils.float_addr);
  auto proc = std::make_shared<ossia::node_process>(node);
  c0->add_time_process(proc);
  g.add_node(node);


  s.interval->start_and_tick();
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.size() == 1);
    REQUIRE(s.m_valueState.begin()->second.back().first.value == ossia::value(0.f));
  }

  s.interval->tick(3_tv);
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.size() == 1);
    REQUIRE(s.m_valueState.begin()->second.back().first.value == ossia::value(3./5.));
  }

  s.interval->tick(7_tv);
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.size() == 1);
    REQUIRE(s.m_valueState.begin()->second.back().first.value == ossia::value(1.f));
  }

  s.interval->tick(1_tv);
  {
    ossia::execution_state s;
    g.state(s);
    REQUIRE(s.m_valueState.empty());
  }
}

TEST_CASE ("test_offset", "test_offset")
{
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

  //        std::shared_ptr<time_interval> c0 = time_interval::create({}, *e0, *e1, 5_tv, 5_tv, 5_tv);
  //        s.scenario->add_time_interval(c0);
  //        std::shared_ptr<time_interval> c1 = time_interval::create({}, *e1, *e2, 5_tv, 5_tv, 5_tv);
  //        s.scenario->add_time_interval(c1);
  //        std::shared_ptr<time_interval> c2 = time_interval::create({}, *e2, *e3, 5_tv, 5_tv, 5_tv);
  //        s.scenario->add_time_interval(c1);

  //        {
  //            auto st = s.interval->offset(7_tv);
  //            ossia::print(std::cerr, st);

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
  //            ossia::print(std::cerr, st);

  //            REQUIRE(st == expected);
  //        }

  //        s.interval->stop();

  //        {
  //            auto st = s.interval->offset(13_tv);
  //            ossia::print(std::cerr, st);

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
  //            ossia::print(std::cerr, st);

  //            REQUIRE(st == expected);
  //        }

}
