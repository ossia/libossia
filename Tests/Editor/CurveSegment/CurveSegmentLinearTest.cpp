// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <iostream>


TEST_CASE ("bool", "[bool]")
{
  ossia::curve_segment_linear<bool> curveSegment;

  REQUIRE(curveSegment(0., false, true) == false);
  REQUIRE(curveSegment(0.25, false, true) == true);
  REQUIRE(curveSegment(0.5, false, true) == true);
  REQUIRE(curveSegment(0.75, false, true) == true);
  REQUIRE(curveSegment(1., false, true) == true);
}

TEST_CASE ("int", "[int]")
{
  ossia::curve_segment_linear<int> curveSegment;

  REQUIRE(curveSegment(0., 0, 1) == 0);
  REQUIRE(curveSegment(0.25, 0, 1) == 0);
  REQUIRE(curveSegment(0.5, 0, 1) == 0);
  REQUIRE(curveSegment(0.75, 0, 1) == 0);
  REQUIRE(curveSegment(1., 0, 1) == 1);
}

TEST_CASE ("float", "[float]")
{
  ossia::curve_segment_linear<float> curveSegment;

  REQUIRE(curveSegment(0., 0., 1.) == 0.);
  REQUIRE(curveSegment(0.25, 0., 1.) == 0.25);
  REQUIRE(curveSegment(0.5, 0., 1.) == 0.5);
  REQUIRE(curveSegment(0.75, 0., 1.) == 0.75);
  REQUIRE(curveSegment(1., 0., 1.) == 1.);
}
