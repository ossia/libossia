// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/curve/curve_segment/empty.hpp>
#include <iostream>

/*! test life cycle and accessors functions for bool type */
TEST_CASE ("bool", "[bool]")
{
  ossia::curve_segment_empty<bool> curveSegment;

  REQUIRE(curveSegment(0., false, true) == false);
  REQUIRE(curveSegment(0.25, false, true) == false);
  REQUIRE(curveSegment(0.5, false, true) == false);
  REQUIRE(curveSegment(0.75, false, true) == false);
  REQUIRE(curveSegment(1., false, true) == false);
}

/*! test life cycle and accessors functions for int type */
TEST_CASE ("int", "[int]")
{
  ossia::curve_segment_empty<int> curveSegment;

  REQUIRE(curveSegment(0., 0, 1) == 0);
  REQUIRE(curveSegment(0.25, 0, 1) == 0);
  REQUIRE(curveSegment(0.5, 0, 1) == 0);
  REQUIRE(curveSegment(0.75, 0, 1) == 0);
  REQUIRE(curveSegment(1., 0, 1) == 0);
}

/*! test life cycle and accessors functions for float type */
TEST_CASE ("float", "[float]")
{
  ossia::curve_segment_empty<float> curveSegment;
  REQUIRE(curveSegment(0., 0., 1.) == 0.);
  REQUIRE(curveSegment(0.25, 0., 1.) == 0.);
  REQUIRE(curveSegment(0.5, 0., 1.) == 0.);
  REQUIRE(curveSegment(0.75, 0., 1.) == 0.);
  REQUIRE(curveSegment(1., 0., 1.) == 0.);
}
