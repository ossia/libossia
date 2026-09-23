// The telemetry arena hands meter levels from the audio thread to the
// interface without losing a peak, whatever the reading rate.

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/telemetry.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <array>
#include <memory>

using namespace ossia::telemetry;

namespace
{
constexpr std::size_t frames = 8;

ossia::audio_vector constant(std::initializer_list<double> levels)
{
  ossia::audio_vector v;
  for(double l : levels)
  {
    ossia::audio_channel c;
    c.assign(frames, l);
    v.push_back(std::move(c));
  }
  return v;
}

struct Fixture
{
  arena a{4};
  std::shared_ptr<meter_tap> tap = std::make_shared<meter_tap>();
  std::weak_ptr<meter_tap> weak = tap;

  Fixture()
  {
    a.set_publish_interval(frames);
    a.attach(1, 7, tap_kind::node, tap);
    // The arena holds the tap now; the caller got the previous one back.
    REQUIRE(!tap);
    REQUIRE(!weak.expired());
  }

  // One tick in which the tap's writer carries `v`.
  void tick(const ossia::audio_vector& v)
  {
    weak.lock()->pending.accumulate(v);
    a.tick(frames, 48000);
  }

  const meter_slot& slot() { return a.latest().meters[1]; }
};
}

TEST_CASE("A published frame carries the tap's levels", "[telemetry]")
{
  Fixture f;
  f.tick(constant({0.5, -0.25}));

  REQUIRE(f.a.consume());
  const auto& s = f.slot();
  CHECK(s.generation == 7);
  CHECK(s.levels.channels == 2);
  CHECK(s.levels.ticks == 1);
  CHECK(s.levels.frames == frames);
  CHECK(s.levels.peak[0] == Catch::Approx(0.5));
  CHECK(s.levels.peak[1] == Catch::Approx(0.25));
  CHECK(s.levels.rms(0) == Catch::Approx(0.5));
  CHECK(!s.levels.is_clipped(0));

  CHECK(!f.a.consume());
}

TEST_CASE("Nothing is published before the interval has elapsed", "[telemetry]")
{
  Fixture f;
  f.a.set_publish_interval(3 * frames);
  f.tick(constant({0.1}));
  f.tick(constant({0.9}));
  CHECK(!f.a.consume());

  f.tick(constant({0.2}));
  REQUIRE(f.a.consume());
  CHECK(f.slot().levels.peak[0] == Catch::Approx(0.9));
  CHECK(f.slot().levels.ticks == 3);
}

TEST_CASE("A frame nobody read is folded into the next one", "[telemetry]")
{
  Fixture f;
  f.tick(constant({0.9}));
  f.tick(constant({0.1}));
  f.tick(constant({0.2}));

  REQUIRE(f.a.consume());
  CHECK(f.slot().levels.peak[0] == Catch::Approx(0.9));
  CHECK(f.slot().levels.ticks == 3);
}

TEST_CASE("A frame that was read is not counted again", "[telemetry]")
{
  Fixture f;
  f.tick(constant({0.9}));
  REQUIRE(f.a.consume());

  f.tick(constant({0.1}));
  REQUIRE(f.a.consume());
  CHECK(f.slot().levels.peak[0] == Catch::Approx(0.1));
  CHECK(f.slot().levels.ticks == 1);
}

TEST_CASE("A clip stays flagged until read", "[telemetry]")
{
  Fixture f;
  f.tick(constant({0.1, 1.5}));
  f.tick(constant({0.1, 0.1}));
  REQUIRE(f.a.consume());
  CHECK(!f.slot().levels.is_clipped(0));
  CHECK(f.slot().levels.is_clipped(1));
}

TEST_CASE("A signal that widens and narrows keeps its widest count", "[telemetry]")
{
  Fixture f;
  f.tick(constant({0.1}));
  f.tick(constant({0.1, 0.2, 0.3}));
  f.tick(constant({0.4}));
  REQUIRE(f.a.consume());
  CHECK(f.slot().levels.channels == 3);
  CHECK(f.slot().levels.peak[0] == Catch::Approx(0.4));
  CHECK(f.slot().levels.peak[2] == Catch::Approx(0.3));
}

TEST_CASE("A tap that did not run reads as not running", "[telemetry]")
{
  Fixture f;
  f.a.tick(frames, 48000);
  REQUIRE(f.a.consume());
  CHECK(f.slot().levels.ticks == 0);

  f.tick({});
  REQUIRE(f.a.consume());
  CHECK(f.slot().levels.ticks == 1);
  CHECK(f.slot().levels.channels == 0);
}

TEST_CASE("Detaching hands the tap back", "[telemetry]")
{
  Fixture f;
  std::shared_ptr<meter_tap> none;
  f.a.attach(1, 8, tap_kind::none, none);
  REQUIRE(none);
  none.reset();
  CHECK(f.weak.expired());

  f.a.tick(frames, 48000);
  REQUIRE(f.a.consume());
  CHECK(f.slot().generation == 8);
  CHECK(f.slot().levels.ticks == 0);
}

TEST_CASE("Hardware taps read the driver's buffers", "[telemetry]")
{
  arena a{2};
  a.set_publish_interval(frames);
  auto in = std::make_shared<meter_tap>();
  auto out = std::make_shared<meter_tap>();
  a.attach(0, 1, tap_kind::hardware_inputs, in);
  a.attach(1, 1, tap_kind::hardware_outputs, out);

  std::array<float, frames> l{}, r{};
  l.fill(0.5f);
  r.fill(-0.75f);
  std::array<float*, 2> chans{l.data(), r.data()};
  ossia::audio_tick_state st{
      .inputs = chans.data(), .outputs = chans.data(), .n_in = 1, .n_out = 2,
      .frames = frames};
  a.accumulate_hardware(st);
  a.tick(frames, 48000);

  REQUIRE(a.consume());
  CHECK(a.latest().meters[0].levels.channels == 1);
  CHECK(a.latest().meters[1].levels.channels == 2);
  CHECK(a.latest().meters[1].levels.peak[1] == Catch::Approx(0.75));
  CHECK(a.latest().sample_rate == 48000);
  CHECK(a.latest().frames == frames);
}

TEST_CASE("An outlet meters what it carries after its gain", "[telemetry][dataflow]")
{
  ossia::audio_outlet out;
  auto tap = std::make_shared<meter_tap>();
  out.meter = tap;
  out.gain = 0.5;

  out->set_channels(1);
  out->channel(0).assign(frames, 1.);
  out.post_process();
  CHECK(tap->pending.peak[0] == Catch::Approx(0.5));

  out->set_channels(0);
  out.post_process();
  CHECK(tap->pending.ticks == 2);
}

TEST_CASE("A timed node's share of the real time comes back", "[telemetry][bench]")
{
  arena a{0, 2};
  a.set_publish_interval(frames);
  auto tap = std::make_shared<bench_tap>();
  std::weak_ptr<bench_tap> weak = tap;
  a.attach_bench(1, 3, tap);
  REQUIRE(!tap);

  // Half of a buffer of `frames` frames at 48 kHz, in two runs.
  const int64_t buffer_ns = int64_t(1e9 * frames / 48000);
  weak.lock()->pending.add(buffer_ns / 4);
  weak.lock()->pending.add(buffer_ns / 4);
  a.tick(frames, 48000);

  REQUIRE(a.consume());
  const auto& f = a.latest();
  const auto& s = f.benches[1];
  CHECK(s.generation == 3);
  CHECK(s.levels.runs == 2);
  CHECK(s.levels.max_ns == uint64_t(buffer_ns / 4));
  CHECK(f.window_frames == frames);
  CHECK(f.load(s) == Catch::Approx(0.5).epsilon(0.01));
}

TEST_CASE("Time not read yet is folded into the next frame", "[telemetry][bench]")
{
  arena a{0, 1};
  a.set_publish_interval(frames);
  auto tap = std::make_shared<bench_tap>();
  auto* t = tap.get();
  a.attach_bench(0, 1, tap);

  t->pending.add(100);
  a.tick(frames, 48000);
  t->pending.add(300);
  a.tick(frames, 48000);

  REQUIRE(a.consume());
  CHECK(a.latest().benches[0].levels.ns == 400);
  CHECK(a.latest().benches[0].levels.max_ns == 300);
  CHECK(a.latest().window_frames == 2 * frames);
}
