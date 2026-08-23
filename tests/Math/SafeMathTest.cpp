// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// safe_isnan / safe_isinf / safe_isfinite exist because -ffast-math lets the
// compiler assume no NaN and no infinity ever occur, so it folds std::isnan and
// std::isinf to a constant 0 and std::isfinite to a constant 1. A check written
// with them then disappears entirely, and the value it was guarding against
// flows on. These assert the predicates keep answering truthfully, which is the
// whole reason the file exists - and they are worth running under the same
// flags the library ships with.

#include <ossia/detail/config.hpp>
#include <ossia/math/safe_math.hpp>

#include "include_catch.hpp"

#include <limits>

namespace
{
constexpr auto qnan = std::numeric_limits<double>::quiet_NaN();
constexpr auto snan = std::numeric_limits<double>::signaling_NaN();
constexpr auto inf = std::numeric_limits<double>::infinity();
constexpr auto dmax = std::numeric_limits<double>::max();
constexpr auto dmin = std::numeric_limits<double>::min();
constexpr auto denorm = std::numeric_limits<double>::denorm_min();
}

TEST_CASE("safe_isnan", "safe_math")
{
  REQUIRE(ossia::safe_isnan(qnan));
  REQUIRE(ossia::safe_isnan(snan));
  REQUIRE(ossia::safe_isnan(-qnan));

  REQUIRE(!ossia::safe_isnan(0.));
  REQUIRE(!ossia::safe_isnan(-0.));
  REQUIRE(!ossia::safe_isnan(1.));
  REQUIRE(!ossia::safe_isnan(-1.));
  REQUIRE(!ossia::safe_isnan(inf));
  REQUIRE(!ossia::safe_isnan(-inf));
  REQUIRE(!ossia::safe_isnan(dmax));
  REQUIRE(!ossia::safe_isnan(dmin));
  REQUIRE(!ossia::safe_isnan(denorm));
}

TEST_CASE("safe_isinf", "safe_math")
{
  REQUIRE(ossia::safe_isinf(inf));
  REQUIRE(ossia::safe_isinf(-inf));

  REQUIRE(!ossia::safe_isinf(0.));
  REQUIRE(!ossia::safe_isinf(-0.));
  REQUIRE(!ossia::safe_isinf(1.));
  REQUIRE(!ossia::safe_isinf(qnan));
  // The largest finite double is not infinity, and the bit patterns are
  // adjacent - the obvious off-by-one in an exponent test shows up here.
  REQUIRE(!ossia::safe_isinf(dmax));
  REQUIRE(!ossia::safe_isinf(-dmax));
  REQUIRE(!ossia::safe_isinf(denorm));
}

TEST_CASE("safe_isfinite", "safe_math")
{
  REQUIRE(ossia::safe_isfinite(0.));
  REQUIRE(ossia::safe_isfinite(-0.));
  REQUIRE(ossia::safe_isfinite(1.));
  REQUIRE(ossia::safe_isfinite(-1.));
  REQUIRE(ossia::safe_isfinite(dmax));
  REQUIRE(ossia::safe_isfinite(-dmax));
  REQUIRE(ossia::safe_isfinite(dmin));
  // Subnormals are finite. An implementation testing "exponent is zero" for
  // something other than what it means would get this wrong.
  REQUIRE(ossia::safe_isfinite(denorm));
  REQUIRE(ossia::safe_isfinite(-denorm));

  REQUIRE(!ossia::safe_isfinite(qnan));
  REQUIRE(!ossia::safe_isfinite(snan));
  REQUIRE(!ossia::safe_isfinite(-qnan));
  REQUIRE(!ossia::safe_isfinite(inf));
  REQUIRE(!ossia::safe_isfinite(-inf));
}

TEST_CASE("safe predicates agree with each other", "safe_math")
{
  // isfinite is exactly "neither NaN nor infinity". Asserting the identity
  // rather than a list of cases catches a divergence between the three
  // platform-specific branches.
  const double values[] = {0.,   -0.,   1.,   -1.,  0.5,  -0.5, dmax,   -dmax,
                           dmin, denorm, -denorm, inf, -inf, qnan, snan, -qnan};
  for(double v : values)
  {
    INFO("value: " << v);
    REQUIRE(
        ossia::safe_isfinite(v) == (!ossia::safe_isnan(v) && !ossia::safe_isinf(v)));
    // A value is never two of the three things at once.
    REQUIRE(!(ossia::safe_isnan(v) && ossia::safe_isinf(v)));
  }
}

TEST_CASE("safe predicates survive arithmetic that produces them", "safe_math")
{
  // Values the compiler cannot constant-fold away: these come out of real
  // arithmetic, which is how they arise in a running graph.
  volatile double zero = 0.;
  volatile double one = 1.;
  volatile double big = dmax;

  const double div = one / zero;
  REQUIRE(ossia::safe_isinf(div));
  REQUIRE(!ossia::safe_isfinite(div));

  const double nan_from_div = zero / zero;
  REQUIRE(ossia::safe_isnan(nan_from_div));
  REQUIRE(!ossia::safe_isfinite(nan_from_div));

  const double overflowed = big * big;
  REQUIRE(ossia::safe_isinf(overflowed));
  REQUIRE(!ossia::safe_isfinite(overflowed));

  const double ordinary = one / (one + one);
  REQUIRE(ossia::safe_isfinite(ordinary));
  REQUIRE(!ossia::safe_isnan(ordinary));
  REQUIRE(!ossia::safe_isinf(ordinary));
}
