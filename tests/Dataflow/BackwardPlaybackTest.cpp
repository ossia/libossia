// Unit tests for backwards playback (speed < 0).
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include "include_catch.hpp"

#include <optional>
#include <tuple>
#include <vector>

namespace
{
// 1 model unit == 1 sample, which keeps the expected values readable.
constexpr double unit_ratio = 1.;

ossia::token_request
tick(int64_t prev, int64_t date, int64_t offset, double speed) noexcept
{
  return ossia::token_request{
      ossia::time_value{prev},   ossia::time_value{date}, ossia::time_value{1000000},
      ossia::time_value{offset}, speed,                   ossia::time_signature{4, 4},
      120.};
}
}

TEST_CASE("test_time_value_unary_minus_is_pure", "test_time_value_unary_minus_is_pure")
{
  ossia::time_value v{200};
  const auto n = -v;

  REQUIRE(n.impl == -200);
  REQUIRE(v.impl == 200);

  // Two negations in the same expression must not interfere.
  ossia::time_value a{7};
  ossia::time_value b{5};
  REQUIRE((-a + -b).impl == -12);
  REQUIRE(a.impl == 7);
  REQUIRE(b.impl == 5);

  // Infinity stays infinite.
  ossia::time_value inf{ossia::time_value::infinity};
  REQUIRE((-inf).infinite());
  REQUIRE(inf.infinite());

  // And it is usable on a const time_value / a temporary.
  const ossia::time_value c{42};
  REQUIRE((-c).impl == -42);
  REQUIRE((-ossia::time_value{3}).impl == -3);
}

TEST_CASE("test_physical_helpers_are_direction_symmetric",
          "test_physical_helpers_are_direction_symmetric")
{
  constexpr int bufferSize = 512;

  struct
  {
    const char* name;
    double speed;
    int64_t dt;
  } speeds[] = {
      {"x1", 1., 200},   {"-x1", -1., 200},   {"x2", 2., 400},
      {"-x2", -2., 400}, {"x0.5", 0.5, 100},  {"-x0.5", -0.5, 100},
  };

  for(auto& s : speeds)
  {
    const int64_t offset = 100;
    const auto t = s.speed > 0 ? tick(1000, 1000 + s.dt, offset, s.speed)
                               : tick(1000 + s.dt, 1000, offset, s.speed);

    const auto abs_speed = s.speed < 0 ? -s.speed : s.speed;
    const auto expected_start = int64_t(offset / abs_speed);
    const auto expected_len = int64_t(s.dt / abs_speed);

    CAPTURE(s.name);
    REQUIRE(t.physical_start(unit_ratio) == expected_start);
    REQUIRE(t.physical_write_duration(unit_ratio) == expected_len);
    REQUIRE(t.physical_read_duration(unit_ratio) == s.dt);
    REQUIRE(
        t.safe_physical_write_duration(unit_ratio, bufferSize)
        == bufferSize - expected_start);

    // Never negative, whichever way we go.
    REQUIRE(t.physical_start(unit_ratio) >= 0);
    REQUIRE(t.physical_write_duration(unit_ratio) >= 0);
    REQUIRE(t.safe_physical_write_duration(unit_ratio, bufferSize) >= 0);
  }
}

TEST_CASE("test_to_physical_time_in_tick_both_directions",
          "test_to_physical_time_in_tick_both_directions")
{
  {
    // Forward: prev_date -> start of the span, date -> end of it.
    const auto t = tick(1000, 1200, 100, 1.);
    REQUIRE(t.to_physical_time_in_tick(ossia::time_value{1000}, unit_ratio) == 100);
    REQUIRE(t.to_physical_time_in_tick(ossia::time_value{1100}, unit_ratio) == 200);
    REQUIRE(t.to_physical_time_in_tick(ossia::time_value{1200}, unit_ratio) == 300);
    REQUIRE(
        t.to_physical_time_in_tick(t.prev_date, unit_ratio)
        == t.physical_start(unit_ratio));
    REQUIRE(
        t.to_physical_time_in_tick(t.date, unit_ratio)
        == t.physical_start(unit_ratio) + t.physical_write_duration(unit_ratio));
  }

  {
    // Backward: same span, prev_date is still the first sample written.
    const auto t = tick(1200, 1000, 100, -1.);
    REQUIRE(t.to_physical_time_in_tick(ossia::time_value{1200}, unit_ratio) == 100);
    REQUIRE(t.to_physical_time_in_tick(ossia::time_value{1100}, unit_ratio) == 200);
    REQUIRE(t.to_physical_time_in_tick(ossia::time_value{1000}, unit_ratio) == 300);
    REQUIRE(
        t.to_physical_time_in_tick(t.prev_date, unit_ratio)
        == t.physical_start(unit_ratio));
    REQUIRE(
        t.to_physical_time_in_tick(t.date, unit_ratio)
        == t.physical_start(unit_ratio) + t.physical_write_duration(unit_ratio));
  }

  {
    // Monotonically increasing in buffer position as the playhead advances,
    // in both directions.
    const auto fwd = tick(1000, 1200, 0, 1.);
    const auto bwd = tick(1200, 1000, 0, -1.);
    int64_t prev_f = -1, prev_b = -1;
    for(int64_t i = 0; i <= 200; i++)
    {
      const auto f = fwd.to_physical_time_in_tick(ossia::time_value{1000 + i}, unit_ratio);
      const auto b = bwd.to_physical_time_in_tick(ossia::time_value{1200 - i}, unit_ratio);
      REQUIRE(f > prev_f);
      REQUIRE(b > prev_b);
      REQUIRE(f == b);
      prev_f = f;
      prev_b = b;
    }
  }
}

TEST_CASE("test_physical_time_roundtrip", "test_physical_time_roundtrip")
{
  const ossia::token_request ticks[] = {
      tick(1000, 1200, 0, 1.),   tick(1200, 1000, 0, -1.),
      tick(1000, 1200, 100, 1.), tick(1200, 1000, 100, -1.),
      tick(1000, 1400, 100, 2.), tick(1400, 1000, 100, -2.),
  };

  for(const auto& t : ticks)
  {
    CAPTURE(t.prev_date.impl, t.date.impl, t.offset.impl, t.speed);
    const auto start = t.physical_start(unit_ratio);
    const auto len = t.physical_write_duration(unit_ratio);
    for(int64_t s = start; s <= start + len; s++)
    {
      const auto model = t.from_physical_time_in_tick(s, unit_ratio);
      REQUIRE(t.to_physical_time_in_tick(model, unit_ratio) == s);
    }
  }
}

namespace
{
struct loop_result
{
  std::vector<ossia::token_request> subs;
  std::vector<int64_t> transports;
};

loop_result run_loop(
    int64_t from, int64_t to, int64_t start_offset, int64_t loop_duration,
    int64_t initial_offset = 0)
{
  loop_result res;
  const auto t = tick(from, to, initial_offset, to >= from ? 1. : -1.);
  t.loop(
      ossia::time_value{start_offset}, ossia::time_value{loop_duration},
      [&](const ossia::token_request& sub) {
    // Guard against a runaway subdivision rather than filling memory.
    if(res.subs.size() < 64)
      res.subs.push_back(sub);
      },
      [&](const ossia::time_value& d) { res.transports.push_back(d.impl); });
  return res;
}

//! Invariants that must hold for every loop() subdivision, both directions.
void check_loop(
    int64_t from, int64_t to, int64_t start_offset, int64_t loop_duration,
    int64_t expected_subs)
{
  CAPTURE(from, to, start_offset, loop_duration);
  const auto res = run_loop(from, to, start_offset, loop_duration);

  REQUIRE(res.subs.size() == std::size_t(expected_subs));

  const int64_t requested = to - from;
  int64_t emitted = 0;
  int64_t expected_offset = 0;

  for(std::size_t i = 0; i < res.subs.size(); i++)
  {
    const auto& sub = res.subs[i];
    const int64_t dt = (sub.date - sub.prev_date).impl;
    CAPTURE(i, sub.prev_date.impl, sub.date.impl, dt, sub.offset.impl);

    // The direction of each piece matches the direction of the request.
    REQUIRE((dt < 0) == (requested < 0));
    REQUIRE(dt != 0);

    // Every piece stays inside the loop.
    REQUIRE(sub.prev_date.impl >= start_offset);
    REQUIRE(sub.prev_date.impl <= start_offset + loop_duration);
    REQUIRE(sub.date.impl >= start_offset);
    REQUIRE(sub.date.impl <= start_offset + loop_duration);

    // The pieces tile the buffer contiguously.
    REQUIRE(sub.offset.impl == expected_offset);
    expected_offset += dt < 0 ? -dt : dt;

    emitted += dt;
  }

  // Nothing lost, nothing invented.
  REQUIRE(emitted == requested);
}
}

TEST_CASE("test_loop_subdivision_forward", "test_loop_subdivision_forward")
{
  check_loop(2600, 2900, 0, 1000, 1);    // inside one iteration
  check_loop(2900, 3200, 0, 1000, 2);    // crosses the loop point
  check_loop(3000, 3100, 0, 1000, 1);    // starts on the loop point
  check_loop(500, 2500, 0, 1000, 3);     // crosses two loop points
  check_loop(2600, 2900, 5000, 1000, 1); // with a start offset
  check_loop(2900, 3200, 5000, 1000, 2);
}

TEST_CASE("test_loop_subdivision_backward", "test_loop_subdivision_backward")
{
  check_loop(2900, 2600, 0, 1000, 1);    // inside one iteration
  check_loop(3200, 2900, 0, 1000, 2);    // crosses the loop point
  check_loop(3100, 3000, 0, 1000, 1);    // lands exactly on the loop point
  check_loop(3000, 2900, 0, 1000, 1);    // starts exactly on the loop point
  check_loop(2500, 500, 0, 1000, 3);     // crosses two loop points
  check_loop(2900, 2600, 5000, 1000, 1); // with a start offset
  check_loop(3200, 2900, 5000, 1000, 2);
}

TEST_CASE("test_loop_subdivision_backward_values", "test_loop_subdivision_backward_values")
{
  {
    // 3200 -> 2900 with a 1000-long loop: 200 units back to the loop start,
    // then the last 100 taken from the end of the previous iteration.
    const auto res = run_loop(3200, 2900, 0, 1000);
    REQUIRE(res.subs.size() == 2);

    REQUIRE(res.subs[0].prev_date.impl == 200);
    REQUIRE(res.subs[0].date.impl == 0);
    REQUIRE(res.subs[0].offset.impl == 0);

    REQUIRE(res.subs[1].prev_date.impl == 1000);
    REQUIRE(res.subs[1].date.impl == 900);
    REQUIRE(res.subs[1].offset.impl == 200);

    // The source must be repositioned to the loop end before the second piece.
    REQUIRE(res.transports.size() == 1);
    REQUIRE(res.transports[0] == 1000);
  }

  {
    // Sitting exactly on the loop start: rewinding enters the previous
    // iteration from its end, so a transport happens before anything is played.
    const auto res = run_loop(3000, 2900, 0, 1000);
    REQUIRE(res.subs.size() == 1);
    REQUIRE(res.subs[0].prev_date.impl == 1000);
    REQUIRE(res.subs[0].date.impl == 900);
    REQUIRE(res.transports.size() == 1);
    REQUIRE(res.transports[0] == 1000);
  }

  {
    // With a start offset, the emitted dates are shifted by it.
    const auto res = run_loop(3200, 2900, 10000, 1000);
    REQUIRE(res.subs.size() == 2);
    REQUIRE(res.subs[0].prev_date.impl == 10200);
    REQUIRE(res.subs[0].date.impl == 10000);
    REQUIRE(res.subs[1].prev_date.impl == 11000);
    REQUIRE(res.subs[1].date.impl == 10900);
    REQUIRE(res.transports.size() == 1);
    REQUIRE(res.transports[0] == 11000);
  }
}

TEST_CASE("test_loop_no_duration", "test_loop_no_duration")
{
  // A zero / negative loop duration means "no looping": one sub-request,
  // shifted by the start offset, in both directions.
  for(int64_t dur : {int64_t(0), int64_t(-1)})
  {
    {
      const auto res = run_loop(1000, 1200, 50, dur);
      REQUIRE(res.subs.size() == 1);
      REQUIRE(res.subs[0].prev_date.impl == 1050);
      REQUIRE(res.subs[0].date.impl == 1250);
    }
    {
      const auto res = run_loop(1200, 1000, 50, dur);
      REQUIRE(res.subs.size() == 1);
      REQUIRE(res.subs[0].prev_date.impl == 1250);
      REQUIRE(res.subs[0].date.impl == 1050);
    }
  }
}

TEST_CASE("test_sound_sample_info_partial_ticks", "test_sound_sample_info_partial_ticks")
{
  constexpr int64_t bufferSize = 256;

  // A full tick fills the whole buffer, both directions.
  {
    const auto f = ossia::snd::sample_info(bufferSize, unit_ratio, tick(0, 256, 0, 1.));
    REQUIRE(f.samples_to_write == 256);
    const auto b = ossia::snd::sample_info(bufferSize, unit_ratio, tick(256, 0, 0, -1.));
    REQUIRE(b.samples_to_write == 256);
  }

  // An interval *ending* mid-buffer must write only its own part of it: it used
  // to write all the way to the end of the buffer, over the samples belonging
  // to whatever starts playing next, and drift its read position by the
  // difference on every such tick.
  {
    const auto f = ossia::snd::sample_info(bufferSize, unit_ratio, tick(0, 128, 0, 1.));
    REQUIRE(f.samples_to_write == 128);
    const auto b = ossia::snd::sample_info(bufferSize, unit_ratio, tick(128, 0, 0, -1.));
    REQUIRE(b.samples_to_write == 128);
  }

  // An interval *starting* mid-buffer writes the second half.
  {
    const auto f = ossia::snd::sample_info(bufferSize, unit_ratio, tick(0, 128, 128, 1.));
    REQUIRE(f.samples_to_write == 128);
    const auto b
        = ossia::snd::sample_info(bufferSize, unit_ratio, tick(128, 0, 128, -1.));
    REQUIRE(b.samples_to_write == 128);
  }

  // Never more than what is left in the buffer.
  {
    const auto f = ossia::snd::sample_info(bufferSize, unit_ratio, tick(0, 512, 200, 1.));
    REQUIRE(f.samples_to_write == bufferSize - 200);
    const auto b
        = ossia::snd::sample_info(bufferSize, unit_ratio, tick(512, 0, 200, -1.));
    REQUIRE(b.samples_to_write == bufferSize - 200);
  }

  // A paused tick reads and writes nothing.
  {
    const auto p = ossia::snd::sample_info(bufferSize, unit_ratio, tick(128, 128, 0, 1.));
    REQUIRE(p.samples_to_write == 0);
    REQUIRE(p.samples_to_read == 0);
  }
}

namespace
{
//! A tick carrying musical positions, the way time_interval::tick_impl fills
//! them in. 4/4, bar and signature change at quarter 0.
ossia::token_request musical_tick(
    int64_t prev, int64_t date, double musical_start, double musical_end,
    int64_t offset = 0)
{
  auto t = tick(prev, date, offset, date >= prev ? 1. : -1.);
  t.musical_start_last_signature = 0.;
  t.musical_start_last_bar = 0.;
  t.musical_start_position = musical_start;
  t.musical_end_last_bar = 0.;
  t.musical_end_position = musical_end;
  return t;
}
}

TEST_CASE("test_quantification_dates_backward", "test_quantification_dates_backward")
{
  // One quarter note per tick, sixteenth-note division: four steps.
  const auto fwd = musical_tick(0, 1000, 0., 1.);
  const auto bwd = musical_tick(1000, 0, 1., 0.);

  const auto f = fwd.get_quantification_dates(16.);
  const auto b = bwd.get_quantification_dates(16.);

  REQUIRE(f.size() == 4);
  REQUIRE(b.size() == 4);

  // Forward covers [start; end[ : quarters 0, 1/4, 1/2, 3/4.
  const int64_t expected_f[4] = {0, 250, 500, 750};
  for(int i = 0; i < 4; i++)
  {
    CAPTURE(i);
    REQUIRE(f[i].date.impl == expected_f[i]);
  }

  // Rewinding covers the mirror interval ]end; start] : quarters 1, 3/4, 1/2,
  // 1/4, reported from the top so that the buffer positions increase.
  const int64_t expected_b[4] = {1000, 750, 500, 250};
  for(int i = 0; i < 4; i++)
  {
    CAPTURE(i);
    REQUIRE(b[i].date.impl == expected_b[i]);
    if(i > 0)
      REQUIRE(b[i].date < b[i - 1].date);
    REQUIRE(
        bwd.to_physical_time_in_tick(b[i].date, unit_ratio) == int64_t(i) * 250);
  }

  // The step indices count the same points, in the opposite order.
  REQUIRE(f[0].index == 0);
  REQUIRE(f[3].index == 3);
  REQUIRE(b[0].index == 4);
  REQUIRE(b[3].index == 1);
}

TEST_CASE("test_quantification_dates_backward_no_spurious_step",
          "test_quantification_dates_backward_no_spurious_step")
{
  // A tick shorter than the division reports nothing.
  const auto bwd = musical_tick(1000, 900, 0.95, 0.85);
  REQUIRE(bwd.get_quantification_dates(4.).empty());

  // And a tick spanning exactly one point reports exactly one.
  const auto one = musical_tick(1000, 750, 1., 0.75);
  const auto r = one.get_quantification_dates(16.);
  REQUIRE(r.size() == 1);
  REQUIRE(r[0].date.impl == 1000);

  // A tick with no musical duration at all still falls back to prev_date, which
  // is what nodes driven with the musical fields left at zero rely on.
  const auto flat = musical_tick(1000, 900, 0., 0.);
  const auto fr = flat.get_quantification_dates(16.);
  REQUIRE(fr.size() == 1);
  REQUIRE(fr[0].date.impl == 1000);
}

TEST_CASE("test_quantification_dates_direction_mismatch",
          "test_quantification_dates_direction_mismatch")
{
  // An interval whose own speed is negative under a parent that is already
  // rewinding ticks forward while inheriting the parent's decreasing musical
  // positions. Walking that with the clamps of the other direction discards
  // every point.
  auto fwd_tick_bwd_music = musical_tick(0, 1000, 1., 0.);
  fwd_tick_bwd_music.speed = 1.;
  const auto r = fwd_tick_bwd_music.get_quantification_dates(16.);
  REQUIRE(r.size() == 1);
  REQUIRE(r[0].date.impl == 0);

  // And the mirror: a backward tick carrying increasing musical positions.
  auto bwd_tick_fwd_music = musical_tick(1000, 0, 0., 1.);
  bwd_tick_fwd_music.speed = -1.;
  const auto r2 = bwd_tick_fwd_music.get_quantification_dates(16.);
  REQUIRE(r2.size() == 1);
  REQUIRE(r2[0].date.impl == 1000);

  // metronome() must not interpolate with a negative musical duration either.
  bool fired = false;
  fwd_tick_bwd_music.musical_start_last_bar = 1.;
  fwd_tick_bwd_music.musical_end_last_bar = 0.;
  fwd_tick_bwd_music.metronome(
      unit_ratio, [&](int64_t) { fired = true; }, [&](int64_t) { fired = true; });
  REQUIRE(!fired);
}

TEST_CASE("test_quantification_date_backward_is_none",
          "test_quantification_date_backward_is_none")
{
  // Used for quantized triggering: interval start / stop, looper record.
  const auto bwd = musical_tick(1000, 0, 1., 0.);
  REQUIRE(!bwd.get_quantification_date(16.).has_value());
  REQUIRE(!bwd.get_quantification_date(1.).has_value());
  REQUIRE(!bwd.get_physical_quantification_date(16., unit_ratio).has_value());

  const auto fwd = musical_tick(0, 900, 0.1, 0.9);
  REQUIRE(fwd.get_quantification_date(16.).has_value());
}

namespace
{
struct metro_result
{
  std::optional<int64_t> hi;
  std::optional<int64_t> lo;
};

metro_result run_metronome(const ossia::token_request& t)
{
  metro_result r;
  t.metronome(
      unit_ratio, [&](int64_t s) { r.hi = s; }, [&](int64_t s) { r.lo = s; });
  return r;
}
}

TEST_CASE("test_metronome_both_directions", "test_metronome_both_directions")
{
  // A bar line halfway through the tick, crossed forwards then backwards: the
  // click lands on the same sample either way, because rewinding plays that
  // half of the buffer in reverse.
  {
    auto fwd = musical_tick(0, 1000, 3.5, 4.5);
    fwd.musical_start_last_bar = 0.;
    fwd.musical_end_last_bar = 4.;
    const auto r = run_metronome(fwd);
    REQUIRE(r.hi.has_value());
    REQUIRE(*r.hi == 500);
    REQUIRE(!r.lo.has_value());
  }
  {
    auto bwd = musical_tick(1000, 0, 4.5, 3.5);
    bwd.musical_start_last_bar = 4.;
    bwd.musical_end_last_bar = 0.;
    const auto r = run_metronome(bwd);
    REQUIRE(r.hi.has_value());
    REQUIRE(*r.hi == 500);
    REQUIRE(!r.lo.has_value());
  }

  // Same for a quarter inside the bar.
  {
    const auto fwd = musical_tick(0, 1000, 0.5, 1.5);
    const auto r = run_metronome(fwd);
    REQUIRE(r.lo.has_value());
    REQUIRE(*r.lo == 500);
    REQUIRE(!r.hi.has_value());
  }
  {
    const auto bwd = musical_tick(1000, 0, 1.5, 0.5);
    const auto r = run_metronome(bwd);
    REQUIRE(r.lo.has_value());
    REQUIRE(*r.lo == 500);
    REQUIRE(!r.hi.has_value());
  }

  // A tick that crosses nothing is silent, both ways.
  for(const auto& t :
      {musical_tick(0, 1000, 0.1, 0.4), musical_tick(1000, 0, 0.4, 0.1)})
  {
    const auto r = run_metronome(t);
    REQUIRE(!r.hi.has_value());
    REQUIRE(!r.lo.has_value());
  }
}

TEST_CASE("test_metronome_stays_inside_the_tick", "test_metronome_stays_inside_the_tick")
{
  // A bar line landing exactly on a tick boundary belongs to the tick that
  // starts on it, where it is sample 0, not to the one that ends on it, where
  // it could only be the last sample and so a sample early.
  {
    auto ends_on_it = musical_tick(0, 1000, 3.5, 4.0);
    ends_on_it.musical_start_last_bar = 0.;
    ends_on_it.musical_end_last_bar = 4.;
    const auto r = run_metronome(ends_on_it);
    REQUIRE(!r.hi.has_value());

    auto starts_on_it = musical_tick(1000, 2000, 4.0, 4.5);
    starts_on_it.musical_start_last_bar = 4.;
    starts_on_it.musical_end_last_bar = 4.;
    const auto r2 = run_metronome(starts_on_it);
    REQUIRE(r2.hi.has_value());
    REQUIRE(*r2.hi == 0);
  }

  // Every crossing, whatever the geometry, reports a sample inside the tick.
  for(double start : {0.0, 0.3, 1.0, 3.5, 3.99})
  {
    for(double span : {0.25, 1., 2., 4.5})
    {
      for(bool rewind : {false, true})
      {
        auto t = rewind ? musical_tick(1000, 0, start + span, start)
                        : musical_tick(0, 1000, start, start + span);
        t.musical_start_last_bar = rewind ? 4. : 0.;
        t.musical_end_last_bar = rewind ? 0. : 4.;
        CAPTURE(start, span, rewind);

        const auto res = run_metronome(t);
        for(auto s : {res.hi, res.lo})
        {
          if(s)
          {
            REQUIRE(*s >= 0);
            REQUIRE(*s < 1000);
          }
        }
      }
    }
  }
}

TEST_CASE("test_metronome_direction_mismatch", "test_metronome_direction_mismatch")
{
  // Musical positions that disagree with the tick's direction: nothing to
  // interpolate, and interpolating anyway would land outside the buffer.
  auto t = musical_tick(0, 1000, 4.5, 3.5);
  t.speed = 1.;
  t.musical_start_last_bar = 4.;
  t.musical_end_last_bar = 0.;
  const auto r = run_metronome(t);
  REQUIRE(!r.hi.has_value());
  REQUIRE(!r.lo.has_value());
}

TEST_CASE("test_bar_change_backward", "test_bar_change_backward")
{
  // A bar change is a bar change in both directions.
  auto b = musical_tick(1000, 0, 4., 2.);
  b.musical_start_last_bar = 4.;
  b.musical_end_last_bar = 2.;
  b.signature = {4, 4};
  REQUIRE(b.unexpected_bar_change());

  b.musical_start_last_bar = 4.;
  b.musical_end_last_bar = 0.;
  REQUIRE(!b.unexpected_bar_change());
}

TEST_CASE("test_sound_raw_mode_ignores_tempo", "test_sound_raw_mode_ignores_tempo")
{
  // Stretch mode None plays the file at its own rate: nothing derived from the
  // tick may depend on the live tempo.
  ossia::execution_state e;
  e.bufferSize = 256;
  e.modelToSamplesRatio = unit_ratio;
  e.samplesToModelRatio = unit_ratio;
  ossia::exec_state_facade st{&e};

  ossia::sound_processing_info info;
  info.set_native_tempo(120.);
  info.set_loop_info(ossia::time_value{4000}, ossia::time_value{700}, true);

  auto at_tempo = [&](double tempo) {
    auto t = tick(0, 256, 0, 1.);
    t.tempo = tempo;
    const double stretch = info.update_stretch(t, st);
    return std::tuple{
        stretch, info.m_loop_duration_samples, info.m_start_offset_samples};
  };

  const auto [s120, l120, o120] = at_tempo(120.);
  REQUIRE(s120 == 1.);
  REQUIRE(l120 == 4000);
  REQUIRE(o120 == 700);

  for(double tempo : {60., 90., 140., 200., 240.})
  {
    CAPTURE(tempo);
    const auto [s, l, o] = at_tempo(tempo);
    REQUIRE(s == 1.);
    REQUIRE(l == l120);
    REQUIRE(o == o120);
  }
}

TEST_CASE("test_timings_direction_symmetric", "test_timings_direction_symmetric")
{
  ossia::execution_state e;
  e.bufferSize = 256;
  e.modelToSamplesRatio = unit_ratio;
  e.samplesToModelRatio = unit_ratio;
  ossia::exec_state_facade st{&e};

  struct
  {
    int64_t offset;
    int64_t dur;
  } cases[] = {{0, 256}, {0, 128}, {128, 128}, {64, 64}, {200, 56}};

  for(auto& c : cases)
  {
    CAPTURE(c.offset, c.dur);
    const auto f = st.timings(tick(0, c.dur, c.offset, 1.));
    const auto b = st.timings(tick(c.dur, 0, c.offset, -1.));

    REQUIRE(f.start_sample == c.offset);
    REQUIRE(f.length == c.dur);
    REQUIRE(b.start_sample == f.start_sample);
    REQUIRE(b.length == f.length);
    REQUIRE(b.start_sample + b.length <= e.bufferSize);
  }

  // Speed too close to zero: nothing to do, and no out-of-range values.
  {
    const auto z = st.timings(tick(0, 0, 0, 0.));
    REQUIRE(z.start_sample == 0);
    REQUIRE(z.length == 0);
  }
}
