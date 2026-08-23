// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// The smoothing primitives. These are shared by every tracker and mapping in
// the tree, so the properties asserted here are the ones callers rely on
// without checking: that the first sample is not smoothed up from zero, that a
// filter converges to a constant input, that alpha = 1 is a pass-through, and
// that an angle filter does not lurch by a full turn at the wrap.

#include <ossia/detail/config.hpp>
#include <ossia/math/filters.hpp>

#include "include_catch.hpp"

#include <cmath>

TEST_CASE("lowpass_alpha and lag_alpha are in range and monotonic", "filters")
{
  // Both map a cutoff/time-constant and a dt to a coefficient in [0, 1].
  for(float dt : {1.f / 44100.f, 1.f / 60.f, 0.1f, 1.f})
  {
    for(float cutoff : {0.01f, 1.f, 20.f, 1000.f})
    {
      const float a = ossia::lowpass_alpha(cutoff, dt);
      INFO("cutoff " << cutoff << " dt " << dt << " -> " << a);
      REQUIRE(a >= 0.f);
      REQUIRE(a <= 1.f);
    }
    // A longer time constant smooths harder, i.e. a smaller coefficient.
    REQUIRE(ossia::lag_alpha(1.f, dt) < ossia::lag_alpha(0.01f, dt));
  }
}

TEST_CASE("one_pole passes the first sample through", "filters")
{
  // The point of the primed flag: starting from a default-constructed zero
  // would make every filter open with a ramp from silence that the signal
  // never contained.
  ossia::one_pole_filter<float> f;
  REQUIRE(f(5.f, 0.1f) == Catch::Approx(5.f));
  // The second sample is smoothed, so it lands between the two.
  const float y = f(10.f, 0.5f);
  REQUIRE(y > 5.f);
  REQUIRE(y < 10.f);
}

TEST_CASE("one_pole converges to a constant input", "filters")
{
  ossia::one_pole_filter<float> f;
  f(0.f, 0.2f);
  for(int i = 0; i < 500; i++)
    f(1.f, 0.2f);
  REQUIRE(f.y == Catch::Approx(1.f).margin(1e-4));
}

TEST_CASE("one_pole alpha extremes", "filters")
{
  // alpha = 1 is a pass-through; alpha = 0 freezes. Callers derive alpha from
  // dt, which can legitimately reach either end when a source stalls or fires
  // twice in a row.
  ossia::one_pole_filter<float> pass;
  pass(0.f, 1.f);
  REQUIRE(pass(7.f, 1.f) == Catch::Approx(7.f));

  ossia::one_pole_filter<float> frozen;
  frozen(3.f, 1.f);
  REQUIRE(frozen(99.f, 0.f) == Catch::Approx(3.f));
}

TEST_CASE("one_pole reset", "filters")
{
  ossia::one_pole_filter<float> f;
  f(1.f, 0.5f);
  f.reset();
  // Unprimed again: the next sample passes through rather than being smoothed
  // towards from the old state.
  REQUIRE(f(42.f, 0.01f) == Catch::Approx(42.f));

  f.reset(10.f);
  REQUIRE(f.y == Catch::Approx(10.f));
  REQUIRE(f(10.f, 0.5f) == Catch::Approx(10.f));
}

TEST_CASE("one_euro is smoother at rest than in motion", "filters")
{
  // The whole point of the filter: the cutoff rises with speed, so jitter at
  // rest is attenuated much more than a genuine fast movement is.
  const double dt = 1. / 60.;

  ossia::one_euro_filter<float> still;
  ossia::one_euro_filter<float> moving;

  float out_still = 0.f, out_moving = 0.f;
  for(int i = 0; i < 120; i++)
  {
    const float jitter = (i % 2) ? 0.01f : -0.01f;
    out_still = still(jitter, dt);
    out_moving = moving(float(i) * 0.1f + jitter, dt);
  }

  // At rest the alternating input is squashed towards its mean.
  REQUIRE(std::abs(out_still) < 0.01f);
  // While moving it tracks the ramp instead of lagging far behind it.
  REQUIRE(out_moving > 10.f);
}

TEST_CASE("one_euro tolerates a non-positive dt", "filters")
{
  // Sources do repeat a timestamp - two OSC bundles in one frame, a stalled
  // camera - and dividing by that dt would produce an infinity that then
  // poisons the state permanently.
  ossia::one_euro_filter<float> f;
  f(1.f, 1. / 60.);
  const float held = f(2.f, 0.);
  REQUIRE(std::isfinite(held));
  const float next = f(2.f, 1. / 60.);
  REQUIRE(std::isfinite(next));
}

TEST_CASE("moving_average over its window", "filters")
{
  ossia::moving_average_filter<float, 4> f;
  // Filled with a constant, the average is that constant - the O(1) running
  // sum must not drift as samples are evicted.
  for(int i = 0; i < 50; i++)
    f(2.f);
  REQUIRE(f(2.f) == Catch::Approx(2.f));

  // A step of the same width as the window is fully absorbed after N samples.
  ossia::moving_average_filter<float, 4> g;
  for(int i = 0; i < 8; i++)
    g(0.f);
  float y{};
  for(int i = 0; i < 4; i++)
    y = g(4.f);
  REQUIRE(y == Catch::Approx(4.f));
}

TEST_CASE("median rejects an isolated outlier", "filters")
{
  // The reason to use a median rather than an average: one bad sample from a
  // detector should not move the output at all.
  ossia::median_filter<float, 5> f;
  for(int i = 0; i < 5; i++)
    f(1.f);
  REQUIRE(f(1000.f) == Catch::Approx(1.f));
  REQUIRE(f(1.f) == Catch::Approx(1.f));
}

TEST_CASE("unwrap_angle does not lurch across the discontinuity", "filters")
{
  const auto pi_v = float(ossia::pi);

  // Just past +pi, the naive value jumps by -2pi; unwrapped against the
  // previous value it continues upward instead.
  const float prev = pi_v - 0.1f;
  const float wrapped = -pi_v + 0.1f; // same motion, wrapped
  const float unwrapped = ossia::unwrap_angle(wrapped, prev);
  REQUIRE(unwrapped > prev);
  REQUIRE(unwrapped == Catch::Approx(prev + 0.2f).margin(1e-4));

  // And it is idempotent for a value already near the reference.
  REQUIRE(ossia::unwrap_angle(0.5f, 0.4f) == Catch::Approx(0.5f).margin(1e-5));

  // Several turns away still lands one turn from the reference.
  // (not named `far`: that is still a macro in the Windows headers)
  const float many_turns = ossia::unwrap_angle(0.2f + 6.f * pi_v, 0.2f);
  REQUIRE(many_turns == Catch::Approx(0.2f).margin(1e-3));
}
