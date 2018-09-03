// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <iostream>
#include <ossia/network/value/value.hpp>
#include <ossia/network/value/detail/value_parse_impl.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include "TestUtils.hpp"
using namespace ossia;

TEST_CASE ("test_parse", "test_parse")
{
  REQUIRE(ossia::parse_pretty_value("impulse") == ossia::impulse{});
  REQUIRE(ossia::parse_pretty_value("float: 12.3") == ossia::value{12.3});
  REQUIRE(ossia::parse_pretty_value("int: 123") == ossia::value{123});
  REQUIRE(ossia::parse_pretty_value("char: 'x'") == ossia::value{'x'});
  REQUIRE(ossia::parse_pretty_value("string: \"foo bar baz\"") == ossia::value{"foo bar baz"});
  REQUIRE(ossia::parse_pretty_value("string: \"foo \\\"bar\\\" baz\"") == ossia::value{"foo \"bar\" baz"});
  REQUIRE(ossia::parse_pretty_value("vec2f: [1.2, 3.4]") == ossia::make_vec(1.2, 3.4));
  REQUIRE(ossia::parse_pretty_value("vec3f: [1.2, 3.4, 5.6]") == ossia::make_vec(1.2, 3.4, 5.6));
  REQUIRE(ossia::parse_pretty_value("list: []") == ossia::value(std::vector<ossia::value>{}));
  REQUIRE(ossia::parse_pretty_value("list: [int: 123]") == ossia::value(std::vector<ossia::value>{123}));
  REQUIRE(ossia::parse_pretty_value("list: [float: 1.2, char: 'c', string: \"foo\"]") == ossia::value(std::vector<ossia::value>{1.2, 'c', "foo"}));
}
TEST_CASE ("test_wrapped", "test_wrapped")
{
  REQUIRE(impulse() == true);
  REQUIRE(ossia::value(impulse()) == ossia::value(true));
}

/*! test impulse */
TEST_CASE ("test_impulse", "test_impulse")
{
  impulse p1;
  impulse p2;
  REQUIRE(p1 == p2);
  REQUIRE(!(p1 != p2));
  REQUIRE(!(p1 > p2));
  REQUIRE(p1 >= p2);
  REQUIRE(!(p1 < p2));
  REQUIRE(p1 <= p2);

  REQUIRE(p1 == bool{});
  REQUIRE(p1 == int32_t{});
  REQUIRE(p1 == float{});
  REQUIRE(p1 == char{});
  REQUIRE(p1 == value::make<std::string>());
  REQUIRE(p1 == std::vector<ossia::value>{});
  //! \todo == comparison with generic
  //! \todo == comparison with destination
  //! \todo == comparison with behavior

  REQUIRE(!(p1 != bool{}));
  REQUIRE(!(p1 != int32_t{}));
  REQUIRE(!(p1 != float{}));
  REQUIRE(!(p1 != char{}));
  REQUIRE(!(p1 != value::make<std::string>()));
  REQUIRE(!(p1 != std::vector<ossia::value>{}));
  //! \todo != comparison with generic
  //! \todo != comparison with destination
  //! \todo != comparison with behavior

  REQUIRE(!(p1 > bool{}));
  REQUIRE(!(p1 > int32_t{}));
  REQUIRE(!(p1 > float{}));
  REQUIRE(!(p1 > char{}));
  REQUIRE(!(p1 > value::make<std::string>()));
  REQUIRE(!(p1 > std::vector<ossia::value>{}));
  //! \todo > comparison with generic
  //! \todo > comparison with destination
  //! \todo > comparison with behavior

  REQUIRE(p1 >= bool{});
  REQUIRE(p1 >= int32_t{});
  REQUIRE(p1 >= float{});
  REQUIRE(p1 >= char{});
  REQUIRE(p1 >= value::make<std::string>());
  REQUIRE(p1 >= std::vector<ossia::value>{});
  //! \todo >= comparison with generic
  //! \todo >= comparison with destination
  //! \todo >= comparison with behavior

  REQUIRE(!(p1 < bool{}));
  REQUIRE(!(p1 < int32_t{}));
  REQUIRE(!(p1 < float{}));
  REQUIRE(!(p1 < char{}));
  REQUIRE(!(p1 < value::make<std::string>()));
  REQUIRE(!(p1 < std::vector<ossia::value>{}));
  //! \todo < comparison with generic
  //! \todo < comparison with destination
  //! \todo < comparison with behavior

  REQUIRE(p1 <= bool{});
  REQUIRE(p1 <= int32_t{});
  REQUIRE(p1 <= float{});
  REQUIRE(p1 <= char{});
  REQUIRE(p1 <= value::make<std::string>());
  REQUIRE(p1 <= std::vector<ossia::value>{});
  //! \todo <= comparison with generic
  //! \todo <= comparison with destination
  //! \todo <= comparison with behavior
}

/*! test bool */
TEST_CASE ("test_bool", "test_bool")
{
  bool b1(true), b2(true);
  REQUIRE(b1 == b2);
  REQUIRE(!(b1 != b2));
  REQUIRE(!(b1 > b2));
  REQUIRE(b1 >= b2);
  REQUIRE(!(b1 < b2));
  REQUIRE(b1 <= b2);

  REQUIRE(b1 == impulse());
  REQUIRE(b1 == int32_t(1));
  REQUIRE(b1 == 1.);
  REQUIRE(b1 == char{0x01});
  REQUIRE(!(b1 == value::make<std::string>()));
  REQUIRE(b1 == std::vector<ossia::value>{int32_t(1)});
  //! \todo == comparison with generic
  //! \todo == comparison with destination
  //! \todo == comparison with behavior

  REQUIRE(!(b1 != impulse()));
  REQUIRE(b1 != int32_t{});
  REQUIRE(b1 != float{});
  REQUIRE(b1 != char{0x00});
  REQUIRE(b1 != value::make<std::string>());
  REQUIRE(b1 != std::vector<ossia::value>{0});
  //! \todo != comparison with generic
  //! \todo != comparison with destination
  //! \todo != comparison with behavior

  REQUIRE(!(b1 > impulse()));
  REQUIRE(b1 > int32_t(0));
  REQUIRE(!(b1 > float(2)));
  REQUIRE(b1 > char{0x00});
  REQUIRE(!(b1 > value::make<std::string>()));
  REQUIRE(!(b1 > std::vector<ossia::value>{0, 0}));
  //! \todo > comparison with generic
  //! \todo > comparison with destination
  //! \todo > comparison with behavior

  REQUIRE(b1 >= impulse());
  REQUIRE(b1 >= 1.);
  REQUIRE(!(b1 >= value::make<std::string>()));
  REQUIRE(!(b1 >= std::vector<ossia::value>{}));
  //! \todo >= comparison with generic
  //! \todo >= comparison with destination
  //! \todo >= comparison with behavior

  REQUIRE(!(b1 < impulse()));
  REQUIRE(!(b1 < 1.));
  REQUIRE(!(b1 < value::make<std::string>()));
  REQUIRE(b1 < std::vector<ossia::value>{float(3)});
  //! \todo < comparison with generic
  //! \todo < comparison with destination
  //! \todo < comparison with behavior

  REQUIRE(b1 <= impulse());
  REQUIRE(!(b1 <= int32_t{}));
  REQUIRE(b1 <= 1.);
  REQUIRE(!(b1 <= value::make<std::string>()));
  REQUIRE(!(b1 <= std::vector<ossia::value>{}));
  //! \todo <= comparison with generic
  //! \todo <= comparison with destination
  //! \todo <= comparison with behavior
}

/*! test int */
TEST_CASE ("test_int32_t", "test_int32_t")
{
  int32_t i1(5), i2(5);

  REQUIRE(i1 == i2);
  REQUIRE(!(i1 != i2));
  REQUIRE(!(i1 > i2));
  REQUIRE(i1 >= i2);
  REQUIRE(!(i1 < i2));
  REQUIRE(i1 <= i2);

  REQUIRE(i1 == impulse());
  REQUIRE(!(i1 == true));
  REQUIRE(!(i1 == 1.));
  REQUIRE(i1 == char(0x05));
  REQUIRE(!(i1 == value::make<std::string>()));
  REQUIRE(i1 == std::vector<ossia::value>{int32_t(5)});
  //! \todo == comparison with generic
  //! \todo == comparison with destination
  //! \todo == comparison with behavior

  REQUIRE(!(i1 != impulse()));
  REQUIRE(i1 != bool{});
  REQUIRE(i1 != float{});
  REQUIRE(i1 != char{0x00});
  REQUIRE(i1 != value::make<std::string>());
  REQUIRE(i1 != std::vector<ossia::value>{0});
  //! \todo != comparison with generic
  //! \todo != comparison with destination
  //! \todo != comparison with behavior

  REQUIRE(!(i1 > impulse()));
  REQUIRE(i1 > bool(false));
  REQUIRE(i1 > float(2));
  REQUIRE(i1 > char{0x00});
  REQUIRE(!(i1 > value::make<std::string>()));
  REQUIRE(!(i1 > std::vector<ossia::value>{0, 0}));
  //! \todo > comparison with generic
  //! \todo > comparison with destination
  //! \todo > comparison with behavior

  REQUIRE(i1 >= impulse());
  REQUIRE(i1 >= true);
  REQUIRE(i1 >= 1.);
  REQUIRE(i1 >= char(0x03));
  REQUIRE(!(i1 >= value::make<std::string>()));
  REQUIRE(!(i1 >= std::vector<ossia::value>{}));
  //! \todo >= comparison with generic
  //! \todo >= comparison with destination
  //! \todo >= comparison with behavior

  REQUIRE(!(i1 < impulse()));
  REQUIRE(!(i1 < true));
  REQUIRE(!(i1 < 1.));
  REQUIRE(!(i1 < char{}));
  REQUIRE(!(i1 < value::make<std::string>()));
  REQUIRE(!(i1 < std::vector<ossia::value>{float(3)}));
  //! \todo < comparison with generic
  //! \todo < comparison with destination
  //! \todo < comparison with behavior

  REQUIRE(i1 <= impulse());
  REQUIRE(!(i1 <= bool{}));
  REQUIRE(!(i1 <= 1.));
  REQUIRE(i1 <= char(0x32));
  REQUIRE(!(i1 <= value::make<std::string>()));
  REQUIRE(!(i1 <= std::vector<ossia::value>{}));
  //! \todo <= comparison with generic
  //! \todo <= comparison with destination
  //! \todo <= comparison with behavior
}

/*! test flaot */
TEST_CASE ("test_float", "test_float")
{
  float f1(5);
  float f2 = f1;
  REQUIRE(f1 == f2);
  REQUIRE(!(f1 != f2));
  REQUIRE(!(f1 > f2));
  REQUIRE(f1 >= f2);
  REQUIRE(!(f1 < f2));
  REQUIRE(f1 <= f2);

  REQUIRE(f1 == impulse());
  REQUIRE(!(f1 == true));
  REQUIRE(!(f1 == int32_t(1)));
  REQUIRE(f1 == char(0x05));
  REQUIRE(!(f1 == value::make<std::string>()));
  REQUIRE(f1 == std::vector<ossia::value>{int32_t(5)});
  //! \todo == comparison with generic
  //! \todo == comparison with destination
  //! \todo == comparison with behavior

  REQUIRE(!(f1 != impulse()));
  REQUIRE(f1 != bool{});
  REQUIRE(f1 != int32_t{});
  REQUIRE(f1 != char{0x00});
  REQUIRE(f1 != value::make<std::string>());
  REQUIRE(f1 != std::vector<ossia::value>{0});
  //! \todo != comparison with generic
  //! \todo != comparison with destination
  //! \todo != comparison with behavior

  REQUIRE(!(f1 > impulse()));
  REQUIRE(f1 > bool(false));
  REQUIRE(f1 > 2);
  REQUIRE(f1 > char{0x00});
  REQUIRE(!(f1 > value::make<std::string>()));
  REQUIRE(!(f1 > std::vector<ossia::value>{0, 0}));
  //! \todo > comparison with generic
  //! \todo > comparison with destination
  //! \todo > comparison with behavior

  REQUIRE(f1 >= impulse());
  REQUIRE(f1 >= true);
  REQUIRE(f1 >= int32_t(1));
  REQUIRE(f1 >= char(0x03));
  REQUIRE(!(f1 >= value::make<std::string>()));
  REQUIRE(!(f1 >= std::vector<ossia::value>{}));
  //! \todo >= comparison with generic
  //! \todo >= comparison with destination
  //! \todo >= comparison with behavior

  REQUIRE(!(f1 < impulse()));
  REQUIRE(!(f1 < true));
  REQUIRE(!(f1 < int32_t(1)));
  REQUIRE(!(f1 < char{}));
  REQUIRE(!(f1 < value::make<std::string>()));
  REQUIRE(!(f1 < std::vector<ossia::value>{3.}));
  //! \todo < comparison with generic
  //! \todo < comparison with destination
  //! \todo < comparison with behavior

  REQUIRE(f1 <= impulse());
  REQUIRE(!(f1 <= bool{}));
  REQUIRE(!(f1 <= int32_t(1)));
  REQUIRE(f1 <= char(0x32));
  REQUIRE(!(f1 <= value::make<std::string>()));
  REQUIRE(!(f1 <= std::vector<ossia::value>{}));
  //! \todo <= comparison with generic
  //! \todo <= comparison with destination
  //! \todo <= comparison with behavior
}

/*! test char */
TEST_CASE ("test_char", "test_char")
{
  char c1(5);
  char c2 = c1;
  REQUIRE(c1 == c2);
  REQUIRE(!(c1 != c2));
  REQUIRE(!(c1 > c2));
  REQUIRE(c1 >= c2);
  REQUIRE(!(c1 < c2));
  REQUIRE(c1 <= c2);

  REQUIRE(c1 == impulse());
  REQUIRE(!(c1 == true));
  REQUIRE(!(c1 == int32_t(1)));
  REQUIRE(c1 == char(0x05));
  REQUIRE(!(c1 == value::make<std::string>()));
  REQUIRE(c1 == std::vector<ossia::value>{5});
  //! \todo == comparison with generic
  //! \todo == comparison with destination
  //! \todo == comparison with behavior

  REQUIRE(!(c1 != impulse()));
  REQUIRE(c1 != bool{});
  REQUIRE(c1 != int32_t{});
  REQUIRE(c1 != char{0x00});
  REQUIRE(c1 != value::make<std::string>());
  REQUIRE(c1 != std::vector<ossia::value>{0});
  //! \todo != comparison with generic
  //! \todo != comparison with destination
  //! \todo != comparison with behavior

  REQUIRE(!(c1 > impulse()));
  REQUIRE(c1 > bool(false));
  REQUIRE(c1 > 2);
  REQUIRE(c1 > char{0x00});
  REQUIRE(!(c1 > value::make<std::string>()));
  REQUIRE(!(c1 > std::vector<ossia::value>({int32_t(0), int32_t(0)})));
  //! \todo > comparison with generic
  //! \todo > comparison with destination
  //! \todo > comparison with behavior

  REQUIRE(c1 >= impulse());
  REQUIRE(c1 >= true);
  REQUIRE(c1 >= int32_t(1));
  REQUIRE(c1 >= char(0x03));
  REQUIRE(!(c1 >= value::make<std::string>()));
  REQUIRE(!(c1 >= std::vector<ossia::value>{}));
  //! \todo >= comparison with generic
  //! \todo >= comparison with destination
  //! \todo >= comparison with behavior

  REQUIRE(!(c1 < impulse()));
  REQUIRE(!(c1 < true));
  REQUIRE(!(c1 < int32_t(1)));
  REQUIRE(!(c1 < char{}));
  REQUIRE(!(c1 < value::make<std::string>()));
  REQUIRE(!(c1 < std::vector<ossia::value>{3.}));
  //! \todo < comparison with generic
  //! \todo < comparison with destination
  //! \todo < comparison with behavior

  REQUIRE(c1 <= impulse());
  REQUIRE(!(c1 <= bool{}));
  REQUIRE(!(c1 <= int32_t(1)));
  REQUIRE(c1 <= char(0x32));
  REQUIRE(!(c1 <= value::make<std::string>()));
  REQUIRE(!(c1 <= std::vector<ossia::value>{}));
  //! \todo <= comparison with generic
  //! \todo <= comparison with destination
  //! \todo <= comparison with behavior
}

/*! test string */
TEST_CASE ("test_value_string", "test_value_string")
{
  auto s1 = value::make<std::string>("qsd");
  auto s2 = s1;

  REQUIRE(s1 == s2);
  REQUIRE(!(s1 != s2));
  REQUIRE(!(s1 > s2));
  REQUIRE(s1 >= s2);
  REQUIRE(!(s1 < s2));
  REQUIRE(s1 <= s2);

  REQUIRE(s1 == impulse());
  REQUIRE(!(s1 == true));
  REQUIRE(!(s1 == int32_t(1)));
  REQUIRE(!(s1 == float(0)));
  REQUIRE(!(s1 == char(0x05)));
  REQUIRE(!(s1 == std::vector<ossia::value>{5}));
  //! \todo == comparison with generic
  //! \todo == comparison with destination
  //! \todo == comparison with behavior

  REQUIRE(!(s1 != impulse()));
  REQUIRE(s1 != bool{});
  REQUIRE(s1 != int32_t{});
  REQUIRE(s1 != 1.);
  REQUIRE(s1 != char{0x00});
  REQUIRE(!(s1 != std::vector<ossia::value>{std::string("qsd")}));
  //! \todo != comparison with generic
  //! \todo != comparison with destination
  //! \todo != comparison with behavior

  REQUIRE(!(s1 > impulse()));
  REQUIRE(!(s1 > bool(false)));
  REQUIRE(!(s1 > 2));
  REQUIRE(!(s1 > float(0.1)));
  REQUIRE(!(s1 > char{0x00}));
  REQUIRE(!(s1 > std::vector<ossia::value>{0, 0}));
  //! \todo > comparison with generic
  //! \todo > comparison with destination
  //! \todo > comparison with behavior

  REQUIRE(s1 >= impulse());
  REQUIRE(!(s1 >= true));
  REQUIRE(!(s1 >= int32_t(1)));
  REQUIRE(!(s1 >= float(5)));
  REQUIRE(!(s1 >= char(0x03)));
  REQUIRE(!(s1 >= std::vector<ossia::value>{}));
  //! \todo >= comparison with generic
  //! \todo >= comparison with destination
  //! \todo >= comparison with behavior

  REQUIRE(!(s1 < impulse()));
  REQUIRE(!(s1 < true));
  REQUIRE(!(s1 < int32_t(1)));
  REQUIRE(!(s1 < float(3)));
  REQUIRE(!(s1 < char{}));

  REQUIRE(s1 < std::vector<ossia::value>{std::string("wxc")});
  //! \todo < comparison with generic
  //! \todo < comparison with destination
  //! \todo < comparison with behavior

  REQUIRE(s1 <= impulse());
  REQUIRE(!(s1 <= bool{}));
  REQUIRE(!(s1 <= int32_t(1)));
  REQUIRE(!(s1 <= float{}));
  REQUIRE(!(s1 <= char(0x32)));
  REQUIRE(!(s1 <= std::vector<ossia::value>{}));
  //! \todo <= comparison with generic
  //! \todo <= comparison with destination
  //! \todo <= comparison with behavior
}

/*! test std::vector<ossia::value> */
TEST_CASE ("test_tuple", "test_tuple")
{
  std::vector<ossia::value> t1{int32_t(5), float(0.2), std::string("abc")};
  REQUIRE(t1[0].get_type() == val_type::INT);
  REQUIRE(t1[1].get_type() == val_type::FLOAT);
  REQUIRE(t1[2].get_type() == val_type::STRING);

  std::vector<ossia::value> t2 = t1;
  REQUIRE(t2[0].get_type() == val_type::INT);
  REQUIRE(t2[1].get_type() == val_type::FLOAT);
  REQUIRE(t2[2].get_type() == val_type::STRING);

  REQUIRE(t1 == t2);
  REQUIRE(!(t1 != t2));
  REQUIRE(!(t1 > t2));
  REQUIRE(t1 >= t2);
  REQUIRE(!(t1 < t2));
  REQUIRE(t1 <= t2);

  REQUIRE(t1 == impulse());
  REQUIRE(!(t1 == true));
  REQUIRE(!(t1 == int32_t(1)));
  REQUIRE(!(t1 == float(5)));
  REQUIRE(!(t1 == char(0x05)));
  REQUIRE(!(t1 == value::make<std::string>()));
  //! \todo == comparison with generic
  //! \todo == comparison with destination
  //! \todo == comparison with behavior

  REQUIRE(!(t1 != impulse()));
  REQUIRE(t1 != bool{});
  REQUIRE(t1 != int32_t{});
  REQUIRE(t1 != float(5));
  REQUIRE(t1 != char{0x00});
  REQUIRE(t1 != value::make<std::string>());
  //! \todo != comparison with generic
  //! \todo != comparison with destination
  //! \todo != comparison with behavior

  REQUIRE(!(t1 > impulse()));
  REQUIRE(!(t1 > bool(false)));
  REQUIRE(!(t1 > 2));
  REQUIRE(!(t1 > float(15)));
  REQUIRE(!(t1 > char{0x00}));
  REQUIRE(!(t1 > std::string("abc")));
  //! \todo > comparison with generic
  //! \todo > comparison with destination
  //! \todo > comparison with behavior

  REQUIRE(t1 >= impulse());
  REQUIRE(!(t1 >= true));
  REQUIRE(!(t1 >= int32_t(1)));
  REQUIRE(!(t1 >= float(-23)));
  REQUIRE(!(t1 >= char(0x03)));
  REQUIRE(!(t1 >= std::string("wxc")));
  //! \todo >= comparison with generic
  //! \todo >= comparison with destination
  //! \todo >= comparison with behavior

  REQUIRE(!(t1 < impulse()));
  REQUIRE(!(t1 < true));
  REQUIRE(!(t1 < int32_t(1)));
  REQUIRE(!(t1 < float(0.098)));
  REQUIRE(!(t1 < char{}));
  REQUIRE(!(t1 < std::string("wxc")));
  //! \todo < comparison with generic
  //! \todo < comparison with destination
  //! \todo < comparison with behavior

  REQUIRE(t1 <= impulse());
  REQUIRE(!(t1 <= bool{}));
  REQUIRE(!(t1 <= int32_t(1)));
  REQUIRE(!(t1 <= float{}));
  REQUIRE(!(t1 <= char(0x32)));
  REQUIRE(!(t1 <= value::make<std::string>()));
  //! \todo <= comparison with generic
  //! \todo <= comparison with destination
  //! \todo <= comparison with behavior
}

TEST_CASE ("test_link", "test_link")
{
  {
    ossia::value v;
    v.target<const float>();
    v.target<const std::array<float, 3>>();
  }

  {
    const ossia::value v;
    v.target<const float>();
    v.target<const std::array<float, 3>>();
  }

}

/*! test generic */
TEST_CASE ("test_generic", "test_generic")
{
  //! \todo test clone()
}

/*! test destination */
TEST_CASE ("test_destination", "test_destination")
{
  //! \todo test clone()

  // Local device
  ossia::net::generic_device device{"test"};

  auto localTupleNode = device.create_child("my_tuple");
  auto localTupleAddress = localTupleNode->create_parameter(val_type::LIST);

  std::vector<ossia::value> t{float(-1.), float(0.), float(1.)};
  localTupleAddress->set_value(t);

  destination d1(*localTupleAddress);
  REQUIRE(d1.index.size() == 0);

  destination d2(*localTupleAddress, ossia::destination_index{1});
  REQUIRE(d2.index.size() == 1);
  REQUIRE(d2.index[0] == 1);

  destination d3 = d1;
  REQUIRE(&d3.value.get() == &d1.value.get());
  destination d4 = std::move(d1);
  REQUIRE(&d4.value.get() == &d3.value.get());
}


/*! test behavior */
TEST_CASE ("test_behavior", "test_behavior")
{
  //! \todo test clone()
}
