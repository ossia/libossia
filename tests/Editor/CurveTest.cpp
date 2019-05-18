// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <ossia/editor/curve/curve_segment/power.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <iostream>

using namespace ossia;

TEST_CASE ("test_basic", "test_basic")
{
  //! \todo test clone()
}

TEST_CASE ("test_double_float", "test_double_float")
{
  // <double, float> curve
  auto c = std::make_shared<curve<double, float>>();
  REQUIRE(c != nullptr);

  curve_segment_linear<float> linearSegment;

  c->set_x0(0.);
  REQUIRE(c->get_x0() == 0.);

  c->set_y0(0.);
  REQUIRE(c->get_y0() == 0.);

  c->add_point(linearSegment, 1., 1.);
  REQUIRE(c->get_points().size() == 1);

  c->add_point(linearSegment, 2., 0.);
  REQUIRE(c->get_points().size() == 2);

  REQUIRE(c->value_at(0.) == 0.);
  REQUIRE(c->value_at(0.5) == 0.5);
  REQUIRE(c->value_at(1.) == 1.);
  REQUIRE(c->value_at(1.5) == 0.5);
  REQUIRE(c->value_at(2.) == 0.);

  c->set_y0(2.);
  REQUIRE(c->get_y0() == 2.);

  REQUIRE(c->value_at(0.) == 2.);
  REQUIRE(c->value_at(0.5) == 1.5);
  REQUIRE(c->value_at(1.) == 1.);
  REQUIRE(c->value_at(1.5) == 0.5);
  REQUIRE(c->value_at(2.) == 0.);

  c->remove_point(1.);
  REQUIRE(c->get_points().size() == 1);

  REQUIRE(c->value_at(0.) == 2.);
  REQUIRE(c->value_at(0.5) == 1.5);
  REQUIRE(c->value_at(1.) == 1.);
  REQUIRE(c->value_at(1.5) == 0.5);
  REQUIRE(c->value_at(2.) == 0.);
}

TEST_CASE ("test_float_float", "test_float_float")
{
  auto c = std::make_shared<curve<float, float>>();
  REQUIRE(c != nullptr);

  curve_segment_linear<float> linearSegment;

  c->set_x0(-100.);
  REQUIRE(c->get_x0() == -100.);

  c->set_y0(-100.);
  REQUIRE(c->get_y0() == -100.);

  c->add_point(linearSegment, 0., 0.);
  REQUIRE(c->get_points().size() == 1);

  REQUIRE(c->value_at(-110.) == Approx(float(-100.)));
  REQUIRE(c->value_at(-100.) == Approx(float(-100.)));
  REQUIRE(c->value_at(-80.) == Approx(float(-80.)));
  REQUIRE(c->value_at(-60.) == Approx(float(-60.)));
  REQUIRE(c->value_at(-40.) == Approx(float(-40.)));
  REQUIRE(c->value_at(-20.) == Approx(float(-20.)));
  REQUIRE(c->value_at(-0.) == Approx(float(0.)));
  REQUIRE(c->value_at(10.) == Approx(float(0.)));
}

TEST_CASE ("test_float_int", "test_float_int")
{
  auto c = std::make_shared<curve<float, int>>();
  curve_segment_linear<int> linearSegment;
  c->set_x0(-10.);
  c->set_y0(-10);
  c->add_point(linearSegment, 10., 10);

  REQUIRE(c->value_at(-10.) == -10);
  REQUIRE(c->value_at(-9.5) == -9);
  REQUIRE(c->value_at(-9.) == -9);
  REQUIRE(c->value_at(-8.5) == -8);
  REQUIRE(c->value_at(-8) == -8);
  REQUIRE(c->value_at(-7.5) == -7);
  REQUIRE(c->value_at(0.) == 0);
  REQUIRE(c->value_at(10.) == 10);
}

TEST_CASE ("test_destination", "test_destination")
{
  ossia::net::generic_device device{"test"};

  auto localTupleNode = device.create_child("my_tuple");
  auto localTupleAddress = localTupleNode->create_parameter(val_type::LIST);

  std::vector<ossia::value> t{float{-1.}, float{0.}, float{1.}};
  localTupleAddress->set_value(t);

  auto c = std::make_shared<curve<double, float>>();
  curve_segment_linear<float> linearSegment;

  c->set_x0(0.);

  destination d(*localTupleAddress, ossia::destination_index{1});
  c->set_y0_destination(d);

  REQUIRE(c->get_y0_destination() == d);

  c->add_point(linearSegment, 1., 1.);

  REQUIRE(c->value_at(0.) == 0.);
  REQUIRE(c->value_at(0.5) == 0.5);
  REQUIRE(c->value_at(1.) == 1.);
}
