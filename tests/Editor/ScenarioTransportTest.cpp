#include <ossia/detail/config.hpp>

#include <ossia/dataflow/token_request.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include "include_catch.hpp"

using namespace ossia;

namespace
{
//! A root interval holding a scenario:
//!   A: rigid, 0 - 313
//!   B: 313 - 576, min 0, max infinite, ends on a trigger that never holds
//!   C: 576 - 848, the same
//!   D: rigid, 848 - 1091
struct transport_fixture
{
  std::shared_ptr<time_sync> root_start = std::make_shared<time_sync>();
  std::shared_ptr<time_sync> root_end = std::make_shared<time_sync>();
  std::shared_ptr<time_interval> root;
  std::shared_ptr<scenario> sc = std::make_shared<scenario>();
  std::shared_ptr<time_event> e0, e1, e2, e3, e4;
  std::shared_ptr<time_interval> A, B, C, D;

  transport_fixture()
  {
    auto rse = add_event(*root_start);
    auto ree = add_event(*root_end);
    root = time_interval::create(
        {}, *rse, *ree, time_value{100000}, time_value{100000}, time_value{100000});
    root->add_time_process(sc);

    e0 = add_event(*sc->get_start_time_sync());
    e1 = add_event(*new_sync(false));
    e2 = add_event(*new_sync(true));
    e3 = add_event(*new_sync(true));
    e4 = add_event(*new_sync(false));

    A = interval(*e0, *e1, 313, 313, time_value{313});
    B = interval(*e1, *e2, 263, 0, Infinite);
    C = interval(*e2, *e3, 272, 0, Infinite);
    D = interval(*e3, *e4, 243, 243, time_value{243});
  }

  static std::shared_ptr<time_event> add_event(time_sync& sync)
  {
    auto e = std::make_shared<time_event>(
        time_event::exec_callback{}, sync, expressions::make_expression_true());
    sync.insert(sync.get_time_events().end(), e);
    return e;
  }

  std::shared_ptr<time_sync> new_sync(bool trigger)
  {
    auto sync = std::make_shared<time_sync>();
    if(trigger)
      sync->set_expression(expressions::make_expression_false());
    sc->add_time_sync(sync);
    return sync;
  }

  std::shared_ptr<time_interval>
  interval(time_event& s, time_event& e, int64_t nominal, int64_t min, time_value max)
  {
    auto itv = time_interval::create(
        {}, s, e, time_value{nominal}, time_value{min}, max);
    sc->add_time_interval(itv);
    return itv;
  }

  void tick_to(int64_t date)
  {
    while(root->get_date().impl < date)
      root->tick(time_value{1}, token_request{});
  }
};

void play_from_here_twice(bool transport)
{
  transport_fixture f;

  // Play from here inside C...
  f.root->start();
  f.root->offset(time_value{700});
  f.tick_to(750);

  // ... then, without stopping, from inside A.
  if(transport)
    f.root->transport(time_value{200});
  else
    f.root->offset(time_value{200});

  // Nothing of the first offset is left
  CHECK(f.e2->get_status() == time_event::status::NONE);
  CHECK(f.e3->get_status() == time_event::status::NONE);
  CHECK(f.B->get_min_duration() == Zero);
  CHECK(f.B->get_max_duration() == Infinite);
  CHECK(f.C->get_max_duration() == Infinite);

  // B starts at the end of A and runs
  f.tick_to(320);
  REQUIRE(f.B->running());
  const auto b_date = f.B->get_date();
  f.tick_to(400);
  CHECK(f.B->get_date() > b_date);

  // Past its nominal end, B still waits for its trigger
  f.tick_to(700);
  CHECK(f.B->running());
  CHECK_FALSE(f.C->running());
}
}

TEST_CASE("transport to an earlier date while playing", "[scenario][transport]")
{
  play_from_here_twice(true);
}

TEST_CASE("offset to an earlier date while playing", "[scenario][transport]")
{
  play_from_here_twice(false);
}

TEST_CASE("transport forward again after going back", "[scenario][transport]")
{
  transport_fixture f;
  f.root->start();
  f.root->offset(time_value{200});
  f.tick_to(250);
  f.root->transport(time_value{700});

  // As for a first play from 7 s: A and B are behind, C runs
  CHECK(f.e1->get_status() == time_event::status::HAPPENED);
  CHECK(f.e2->get_status() == time_event::status::HAPPENED);
  f.tick_to(750);
  CHECK(f.C->get_date() > time_value{124});
}

TEST_CASE("an interval stopped after an offset starts again from its beginning", "[scenario][transport]")
{
  transport_fixture f;

  // Play from here inside B
  f.root->start();
  f.root->offset(time_value{400});
  f.tick_to(450);
  REQUIRE(f.B->get_date() > time_value{100});

  // Its stop then play buttons, while the rest keeps playing
  f.sc->request_stop_interval(*f.B);
  f.tick_to(460);
  f.sc->request_start_interval(*f.B);
  f.tick_to(470);
  CHECK(f.B->running());
  CHECK(f.B->get_date() < time_value{50});
}
