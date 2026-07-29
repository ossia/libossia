#include "../Editor/TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/token_request.hpp>

#include "include_catch.hpp"

#include <iterator>
#include <utility>
#include <vector>
TEST_CASE("test_loops_integer", "test_loops_integer")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 0_tv;
  r.date = 100_tv;

  token_request_vec vec;
  r.loop(
      0_tv, 10_tv, [&](auto& req) { vec.push_back(req); }, [](const auto&) {});

  simple_token_request_vec expected{
      {.prev_date = 0_tv, .date = 10_tv, .offset = 0_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 10_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 20_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 30_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 40_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 50_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 60_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 70_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 80_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 90_tv}};

  REQUIRE(expected == vec);
}

TEST_CASE("test_loops_fract", "test_loops_fract")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 0_tv;
  r.date = 97_tv;

  token_request_vec vec;
  r.loop(
      0_tv, 10_tv, [&](auto& req) { vec.push_back(req); }, [](const auto&) {});

  simple_token_request_vec expected{
      {.prev_date = 0_tv, .date = 10_tv, .offset = 0_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 10_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 20_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 30_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 40_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 50_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 60_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 70_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 80_tv},
      {.prev_date = 0_tv, .date = 7_tv, .offset = 90_tv}};

  REQUIRE(expected == vec);
}

TEST_CASE("test_loops_offset", "test_loops_offset")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 2_tv;
  r.date = 12_tv;

  token_request_vec vec;
  r.loop(
      0_tv, 5_tv, [&](auto& req) { vec.push_back(req); }, [](const auto&) {});

  simple_token_request_vec expected{
      {.prev_date = 2_tv, .date = 5_tv, .offset = 0_tv},
      {.prev_date = 0_tv, .date = 5_tv, .offset = 3_tv},
      {.prev_date = 0_tv, .date = 2_tv, .offset = 8_tv},
  };

  REQUIRE(expected == vec);
}

TEST_CASE("test_loops_smaller", "test_loops_smaller")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 0_tv;
  r.date = 15_tv;

  token_request_vec vec;
  r.loop(
      0_tv, 20_tv, [&](auto& req) { vec.push_back(req); }, [](const auto&) {});

  simple_token_request_vec expected{{.prev_date = 0_tv, .date = 15_tv, .offset = 0_tv}};

  REQUIRE(expected == vec);
}

TEST_CASE("test_loops_mid", "test_loops_mid")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 15_tv;
  r.date = 30_tv;

  token_request_vec vec;
  r.loop(
      0_tv, 20_tv, [&](auto& req) { vec.push_back(req); }, [](const auto&) {});

  simple_token_request_vec expected{
      {.prev_date = 15_tv, .date = 20_tv, .offset = 0_tv},
      {.prev_date = 0_tv, .date = 10_tv, .offset = 5_tv}};

  REQUIRE(expected == vec);
}

TEST_CASE("test_musical_quantization", "test_musical_quantization")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 0_tv;
  r.date = 2048000_tv;
  r.musical_end_last_bar = 0.005804985827664399;
  r.musical_end_position = 0.005804985827664399;

  auto res = r.get_quantification_date(8.0);
  REQUIRE(res == 0_tv);
}

namespace
{
using date_list = std::vector<std::pair<int64_t, int64_t>>;

//! A tick covering [prev; date[ in model time and [mstart; mend[ in quarters.
ossia::token_request musical_tick(
    int64_t prev, int64_t date, double mstart, double mend, int num = 4, int denom = 4)
{
  ossia::token_request t;
  t.prev_date = ossia::time_value{prev};
  t.date = ossia::time_value{date};
  t.musical_start_position = mstart;
  t.musical_end_position = mend;
  t.musical_start_last_bar = 0.;
  t.musical_start_last_signature = 0.;
  t.signature = ossia::time_signature{num, denom};
  return t;
}

date_list dates(const ossia::quantification_points& p)
{
  date_list out;
  for(auto& q : p)
    out.emplace_back(q.date.impl, q.index);
  return out;
}
}

TEST_CASE("quantification_dates_empty_tick", "quantification_dates")
{
  auto t = musical_tick(100, 100, 0., 1.);
  REQUIRE(t.get_quantification_dates(4).empty());
}

TEST_CASE("quantification_dates_no_rate", "quantification_dates")
{
  // A null or negative rate means "as soon as possible".
  auto t = musical_tick(100, 200, 0., 1.);
  REQUIRE(dates(t.get_quantification_dates(0)) == date_list{{100, 0}});
  REQUIRE(dates(t.get_quantification_dates(-1)) == date_list{{100, 0}});
}

TEST_CASE("quantification_dates_no_musical_info", "quantification_dates")
{
  // Without a musical position there is nothing to quantize against: the tick
  // itself is the point. Several nodes rely on this to step once per tick.
  auto t = musical_tick(100, 200, 0., 0.);
  REQUIRE(dates(t.get_quantification_dates(16)) == date_list{{100, 0}});
}

TEST_CASE("quantification_dates_several_per_tick", "quantification_dates")
{
  // The whole point of the multi-date version: one tick worth a quarter note
  // holds four sixteenths, and none of them may be dropped.
  auto t = musical_tick(0, 1000, 0., 1.);

  const auto expected = date_list{{0, 0}, {250, 1}, {500, 2}, {750, 3}};
  REQUIRE(dates(t.get_quantification_dates(16)) == expected);
}

TEST_CASE("quantification_dates_interior_point", "quantification_dates")
{
  auto t = musical_tick(0, 1000, 0.5, 1.5);
  const auto res = t.get_quantification_dates(4);

  REQUIRE(dates(res) == date_list{{500, 1}});
  // and it agrees with the single-date version
  auto single = t.get_quantification_date(4);
  REQUIRE(single.has_value());
  REQUIRE(*single == res[0].date);
}

TEST_CASE("quantification_dates_on_the_start_of_the_tick", "quantification_dates")
{
  auto t = musical_tick(400, 1000, 1., 1.5);
  const auto res = t.get_quantification_dates(4);

  REQUIRE(dates(res) == date_list{{400, 1}});

  auto single = t.get_quantification_date(4);
  REQUIRE(single.has_value());
  REQUIRE(*single == res[0].date);
}

TEST_CASE("quantification_dates_on_the_end_of_the_tick", "quantification_dates")
{
  // A point falling exactly on the end of a tick belongs to the next one,
  // otherwise it would fire twice.
  auto t = musical_tick(0, 1000, 0.5, 1.);
  REQUIRE(t.get_quantification_dates(4).empty());
  REQUIRE(!t.get_quantification_date(4).has_value());
}

TEST_CASE("quantification_dates_bars", "quantification_dates")
{
  // rate <= 1 counts bars: 1 is every bar, 0.5 every two bars.
  auto t = musical_tick(0, 1000, 0., 8.);

  REQUIRE(dates(t.get_quantification_dates(1)) == date_list{{0, 0}, {500, 1}});
  REQUIRE(dates(t.get_quantification_dates(0.5)) == date_list{{0, 0}});
}

TEST_CASE("quantification_dates_odd_signature", "quantification_dates")
{
  // 7/8: a bar is 3.5 quarters.
  auto t = musical_tick(0, 1000, 0., 7., 7, 8);
  REQUIRE(dates(t.get_quantification_dates(1)) == date_list{{0, 0}, {500, 1}});
}

TEST_CASE("quantification_dates_are_ordered_and_inside_the_tick", "quantification_dates")
{
  for(double rate : {1., 2., 4., 8., 16., 32.})
  {
    auto t = musical_tick(0, 4096, 0.3, 5.7);
    const auto res = t.get_quantification_dates(rate);

    INFO("rate " << rate);
    int64_t previous = -1;
    for(const auto& q : res)
    {
      CHECK(q.date.impl >= t.prev_date.impl);
      CHECK(q.date.impl < t.date.impl);
      CHECK(q.date.impl >= previous);
      previous = q.date.impl;
    }
  }
}
