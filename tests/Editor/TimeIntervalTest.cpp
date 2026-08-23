// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include "include_catch.hpp"

#include <iostream>

using namespace ossia;

void event_callback(time_event::status newStatus) { }

/*! test life cycle and accessors functions */
TEST_CASE("test_basic", "test_basic")
{
  auto start_node = std::make_shared<time_sync>();
  auto start_event
      = *(start_node->emplace(start_node->get_time_events().begin(), &event_callback));

  auto end_node = std::make_shared<time_sync>();
  auto end_event
      = *(end_node->emplace(end_node->get_time_events().begin(), &event_callback));

  auto interval = time_interval::create(
      ossia::time_interval::exec_callback{[](auto&&... args) {}}, *start_event,
      *end_event, 1000._tv);
  ossia::clock c{*interval};
  REQUIRE(interval != nullptr);

  REQUIRE(c.get_granularity() == 1._tv);
  REQUIRE(interval->get_offset() == 0._tv);
  REQUIRE(interval->get_internal_speed() == 1.);
  REQUIRE(interval->get_nominal_duration() == 1000._tv);
  REQUIRE(interval->get_min_duration() == 0._tv);
  REQUIRE(interval->get_max_duration() == Infinite);

  using namespace std::literals;

  c.set_granularity(50ms);
  interval->set_speed(2.);
  interval->set_nominal_duration(2000._tv);
  interval->set_min_duration(1000._tv);
  interval->set_max_duration(3000._tv);
  interval->offset(500._tv);

  REQUIRE(c.get_granularity() == 50000._tv);
  REQUIRE(interval->get_internal_speed() == 2.);
  REQUIRE(interval->get_nominal_duration() == 2000._tv);
  REQUIRE(interval->get_min_duration() == 1000._tv);
  REQUIRE(interval->get_max_duration() == 3000._tv);
  REQUIRE(interval->get_offset() == 500._tv);

  REQUIRE(c.running() == false);
  REQUIRE(interval->get_date() == 500._tv);

  REQUIRE(&interval->get_start_event() == start_event.get());
  REQUIRE(&interval->get_end_event() == end_event.get());

  //! \todo test clone()
}

/*! test edition functions */
TEST_CASE("test_edition", "test_edition")
{
  auto start_node = std::make_shared<time_sync>();
  auto start_event
      = *(start_node->emplace(start_node->get_time_events().begin(), &event_callback));

  auto end_node = std::make_shared<time_sync>();
  auto end_event
      = *(end_node->emplace(end_node->get_time_events().begin(), &event_callback));

  auto interval = time_interval::create(
      ossia::time_interval::exec_callback{[](auto&&... args) {}}, *start_event,
      *end_event, 1000._tv);
  auto scenar = std::make_unique<scenario>();

  auto scenar_ptr = scenar.get();
  interval->add_time_process(std::move(scenar));
  REQUIRE(interval->get_time_processes().size() == 1);

  interval->remove_time_process(scenar_ptr);
  REQUIRE(interval->get_time_processes().size() == 0);
}

/*! test execution functions */
TEST_CASE("test_execution", "test_execution")
{
  //      TestDevice t;
  //      auto start_node = std::make_shared<time_sync>();
  //      auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), &event_callback));

  //      auto end_node = std::make_shared<time_sync>();
  //      auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), &event_callback));

  //      auto interval = time_interval::create(&interval_callback, *start_event, *end_event, 1000._tv);

  //      auto s = new scenario;

  //      interval->add_time_process(std::unique_ptr<ossia::time_process>(s));
  //      auto a = new automation{*t.float_addr, std::make_shared<curve<double, float>>()};
  //      interval->add_time_process(std::unique_ptr<ossia::time_process>(a));
  //      auto m = new mapper{*t.float_addr, *t.float_addr, std::make_shared<curve<float, float>>()};
  //      interval->add_time_process(std::unique_ptr<ossia::time_process>(m));
  //      auto l = new loop{10._tv, {}, {}, {}};
  //      interval->add_time_process(std::unique_ptr<ossia::time_process>(l));

  //      interval->start_and_tick();
  //      std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //      interval->pause();
  //      std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //      interval->resume();
  //      std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //      interval->stop();
  //      std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

#if defined(OSSIA_SCENARIO_DATAFLOW)
#include <ossia/dataflow/nodes/forward_node.hpp>

// Regression test: the interval node's Speed inlet (m_inlets[2]) used to be
// allocated, registered and serialized but read by nobody - writing to it was
// a silent no-op. A value written there must now scale the observed tick rate.
TEST_CASE("speed_inlet_override", "speed_inlet_override")
{
  root_scenario s;

  const auto req = [] {
    ossia::token_request req;
    req.prev_date = ossia::time_value{};
    req.date = ossia::time_value{};
    req.tempo = 120;
    req.speed = 1.;
    req.signature = {4, 4};
    return req;
  }();

  // Neutral constant tempo curve at the root tempo, so that the only rate
  // factor under test is the speed override itself.
  ossia::tempo_curve curve;
  curve.set_x0(0);
  curve.set_y0(120.);
  s.interval->set_tempo_curve(curve);

  auto node = static_cast<ossia::nodes::interval*>(s.interval->node.get());
  REQUIRE(node->root_inputs().size() == 4);
  REQUIRE(node->speed_override == ossia::nodes::interval::no_speed);
  REQUIRE(s.interval->speed_override() == 1.);

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});

  // Baseline: tempo = root tempo, no override: 1000 units advance 1000.
  s.interval->tick(1000_tv, req);
  REQUIRE(s.interval->get_date() == 1000_tv);
  REQUIRE(s.interval->get_speed(s.interval->get_date()) == 1.);

  // Write 2.0 into the Speed inlet, run the node as the graph would.
  node->root_inputs()[2]->target<ossia::value_port>()->write_value(2.0f, 0);
  node->run(ossia::token_request{}, ossia::exec_state_facade{});
  REQUIRE(node->speed_override == 2.0f);
  REQUIRE(s.interval->speed_override() == 2.);
  REQUIRE(s.interval->get_speed(s.interval->get_date()) == 2.);

  // The observed tick rate must double: this is the regression assertion.
  s.interval->tick(1000_tv, req);
  REQUIRE(s.interval->get_date() == 3000_tv);

  // Half speed.
  node->root_inputs()[2]->target<ossia::value_port>()->get_data().clear();
  node->root_inputs()[2]->target<ossia::value_port>()->write_value(0.5f, 0);
  node->run(ossia::token_request{}, ossia::exec_state_facade{});
  REQUIRE(s.interval->get_speed(s.interval->get_date()) == 0.5);
  s.interval->tick(1000_tv, req);
  REQUIRE(s.interval->get_date() == 3500_tv);

  // A non-finite value must be ignored, keeping the previous override.
  node->root_inputs()[2]->target<ossia::value_port>()->get_data().clear();
  node->root_inputs()[2]->target<ossia::value_port>()->write_value(
      std::numeric_limits<float>::quiet_NaN(), 0);
  node->run(ossia::token_request{}, ossia::exec_state_facade{});
  REQUIRE(node->speed_override == 0.5f);

  // local_time_factor must see it too (used by scenario children).
  REQUIRE(s.interval->local_time_factor(req) == 0.5);
}

// Without a tempo curve the inlets do not exist and nothing changes.
TEST_CASE("speed_inlet_absent_without_tempo", "speed_inlet_absent_without_tempo")
{
  root_scenario s;
  auto node = static_cast<ossia::nodes::interval*>(s.interval->node.get());
  REQUIRE(node->root_inputs().size() == 1);
  REQUIRE(s.interval->get_speed(s.interval->get_date()) == 1.);

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});
  ossia::token_request req;
  req.tempo = 120;
  req.speed = 1.;
  s.interval->tick(1000_tv, req);
  REQUIRE(s.interval->get_date() == 1000_tv);
}

// Regression test for the Tempo inlet (m_inlets[1]). This is the inlet that
// has always worked; it is asserted here because the Speed inlet work added a
// sibling read in the same block and nothing covered tempo propagation.
TEST_CASE("tempo_inlet_propagates", "tempo_inlet_propagates")
{
  root_scenario s;

  const auto req = [] {
    ossia::token_request req;
    req.prev_date = ossia::time_value{};
    req.date = ossia::time_value{};
    req.tempo = 120;
    req.speed = 1.;
    req.signature = {4, 4};
    return req;
  }();

  ossia::tempo_curve curve;
  curve.set_x0(0);
  curve.set_y0(120.);
  s.interval->set_tempo_curve(curve);

  auto node = static_cast<ossia::nodes::interval*>(s.interval->node.get());
  REQUIRE(node->root_inputs().size() == 4);
  REQUIRE(node->tempo == ossia::nodes::interval::no_tempo);

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});

  // Baseline: curve says 120 == root_tempo, so rate is 1.
  REQUIRE(s.interval->get_speed(s.interval->get_date()) == 1.);
  s.interval->tick(1000_tv, req);
  REQUIRE(s.interval->get_date() == 1000_tv);

  // Write 240 BPM into the Tempo inlet: the live value must override the curve.
  node->root_inputs()[1]->target<ossia::value_port>()->write_value(240.0f, 0);
  node->run(ossia::token_request{}, ossia::exec_state_facade{});
  REQUIRE(node->tempo == 240.0f);
  REQUIRE(s.interval->get_speed(s.interval->get_date()) == 2.);

  // and the observed tick rate must double
  s.interval->tick(1000_tv, req);
  REQUIRE(s.interval->get_date() == 3000_tv);

  // 60 BPM -> half rate
  node->root_inputs()[1]->target<ossia::value_port>()->get_data().clear();
  node->root_inputs()[1]->target<ossia::value_port>()->write_value(60.0f, 0);
  node->run(ossia::token_request{}, ossia::exec_state_facade{});
  REQUIRE(node->tempo == 60.0f);
  REQUIRE(s.interval->get_speed(s.interval->get_date()) == 0.5);
  s.interval->tick(1000_tv, req);
  REQUIRE(s.interval->get_date() == 3500_tv);

  // local_time_factor must agree
  REQUIRE(s.interval->local_time_factor(req) == 0.5);
}
#endif
