// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <ossia/editor/curve/curve_segment/power.hpp>

#include <iostream>

TEST_CASE ("bool", "[bool]")
{
  {
    auto curveSegment = ossia::curve_segment_power<bool>{}(1.);
    REQUIRE(curveSegment(0., false, true) == false);
    REQUIRE(curveSegment(0.25, false, true) == true);
    REQUIRE(curveSegment(0.5, false, true) == true);
    REQUIRE(curveSegment(0.75, false, true) == true);
    REQUIRE(curveSegment(1., false, true) == true);
  }

  {
    auto curveSegment = ossia::curve_segment_power<bool>{}(2.);

    REQUIRE(curveSegment(0., false, true) == false);
    REQUIRE(curveSegment(0.25, false, true) == true);
    REQUIRE(curveSegment(0.5, false, true) == true);
    REQUIRE(curveSegment(0.75, false, true) == true);
    REQUIRE(curveSegment(1., false, true) == true);
  }
}


TEST_CASE ("int", "[int]")
{
  {
    auto curveSegment = ossia::curve_segment_power<int>{}(1.);
    REQUIRE(curveSegment(0., 0, 1) == 0);
    REQUIRE(curveSegment(0.25, 0, 1) == 0);
    REQUIRE(curveSegment(0.5, 0, 1) == 0);
    REQUIRE(curveSegment(0.75, 0, 1) == 0);
    REQUIRE(curveSegment(1., 0, 1) == 1);
  }

  {
    auto curveSegment = ossia::curve_segment_power<int>{}(2.);

    REQUIRE(curveSegment(0., 0, 1) == 0);
    REQUIRE(curveSegment(0.25, 0, 1) == 0);
    REQUIRE(curveSegment(0.5, 0, 1) == 0);
    REQUIRE(curveSegment(0.75, 0, 1) == 0);
    REQUIRE(curveSegment(1., 0, 1) == 1);
  }
}

TEST_CASE ("float", "[float]")
{
  {
    auto curveSegment = ossia::curve_segment_power<float>{}(1.);
    REQUIRE(curveSegment(0., 0., 1.) == 0.);
    REQUIRE(curveSegment(0.25, 0., 1.) == 0.25);
    REQUIRE(curveSegment(0.5, 0., 1.) == 0.5);
    REQUIRE(curveSegment(0.75, 0., 1.) == 0.75);
    REQUIRE(curveSegment(1., 0., 1.) == 1.);
  }

  {
    auto curveSegment = ossia::curve_segment_power<float>{}(2.);
    REQUIRE(curveSegment(0., 0., 1.) == 0.);
    REQUIRE(curveSegment(0.25, 0., 1.) == 0.0625);
    REQUIRE(curveSegment(0.5, 0., 1.) == 0.25);
    REQUIRE(curveSegment(0.75, 0., 1.) == 0.5625);
    REQUIRE(curveSegment(1., 0., 1.) == 1.);
  }
}
