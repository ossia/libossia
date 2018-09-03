// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/network/dataspace/detail/dataspace_merge.hpp>
#include <ossia/network/dataspace/dataspace_parse.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/state/state_element.hpp>

#include "TestUtils.hpp"

void debug(const ossia::value& t) { std::cerr << ossia::value_to_pretty_string(t); }

template<std::size_t N>
bool fuzzy_compare(std::array<float, N> v1, std::array<float, N> v2)
{
  bool b = true;
  double eps = 1e-6;
  for(std::size_t i = 0; i < N; i++)
  {
    if(std::abs(v1[i] - v2[i]) > eps)
      b = false;
  }
  return b;
}

template<typename T>
void make_bitset_impl(T& bs, int pos)
{
}
template<typename T, typename Arg, typename... Args>
void make_bitset_impl(T& bs, int pos,  Arg&& arg, Args&&... args)
{
  if(arg)
    bs.set(pos);

  make_bitset_impl(bs, pos+1, args...);
}

template<typename... Args>
auto make_bitset(Args... args)
-> std::bitset<sizeof...(Args)>
{
  std::bitset<sizeof...(Args)> bs;
  make_bitset_impl(bs, 0, args...);
  return bs;
}

TEST_CASE ("static_test", "static_test")
{
  static_assert(ossia::detail::is_array<ossia::vec3f>::value, "");
  static_assert(ossia::detail::is_array<const ossia::vec3f>::value, "");
  static_assert(!ossia::detail::is_array<float>::value, "");
  static_assert(!ossia::detail::is_array<const float>::value, "");

  static_assert(!ossia::detail::is_array<decltype(ossia::centimeter::dataspace_value)>::value, "");
  static_assert(ossia::detail::is_array<decltype(ossia::rgb::dataspace_value)>::value, "RGB is not iterable");

  static_assert(!ossia::is_unit<int>::value, "");
  static_assert(ossia::is_unit<ossia::centimeter_u>::value, "");
  static_assert(ossia::is_unit<ossia::rgb_u>::value, "");
  static_assert(!ossia::is_unit<ossia::color_u>::value, "");
}

TEST_CASE ("convert_test", "convert_test")
{
  ossia::TestDevice t;

  // Cases :
  /////// Address float, no unit ///////
  t.float_addr->push_value(float(0.));

  // Message float, no unit
  {
    float expected(3.2);
    ossia::message m{*t.float_addr, expected};
    m.launch();
    REQUIRE(t.float_addr->value() == expected);
  }

  // Message float, unit
  {
    float expected(3.2);
    ossia::message m{{*t.float_addr, ossia::meter_per_second_u{}}, expected};
    m.launch();
    REQUIRE(t.float_addr->value() == expected);
  }


  /////// Address float, unit ///////
  t.float_addr->set_unit(ossia::meter_per_second_u{});

  // Message float, no unit
  {
    float expected(3.2);
    ossia::message m{*t.float_addr, expected};
    m.launch();
    REQUIRE(t.float_addr->value() == expected);
  }

  // Message float, same unit
  {
    float expected(3.2);
    ossia::message m{{*t.float_addr, ossia::meter_per_second_u{}}, expected};
    m.launch();
    REQUIRE(t.float_addr->value() == expected);
  }

  // Message float, different unit and same dataspace
  {
    ossia::message m{{*t.float_addr, ossia::kilometer_per_hour_u{}}, float(1.)};
    m.launch();
    REQUIRE(t.float_addr->value() == float(1. / 3.6)); // 1km/h -> 0.27777 m/s
  }

  // Message float, different dataspace
  {
    // The message is ignored, we keep the previous value
    ossia::message m{{*t.float_addr, ossia::meter_u{}}, float(1234.)};
    m.launch();
    REQUIRE(t.float_addr->value() == float(1. / 3.6));
  }


  /////// Address vec, no unit ///////
  t.vec3f_addr->push_value(ossia::make_vec(0., 0., 0.));

  // Message float, no index, no unit
  {
    ossia::message m{*t.vec3f_addr, float(1234.), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(1234., 1234., 1234.));
  }

  t.vec3f_addr->push_value(ossia::make_vec(0., 0., 0.));
  // Message float, no index, unit
  {
    // The message is ignored, we keep the previous value
    ossia::message m{*t.vec3f_addr, float(1234.), ossia::rgb_u{}}; // shouldn't be possible
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 0., 0.));
  }

  // Message float, correct index, no unit
  {
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, float(1234.), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 1234., 0.));
  }

  // Message float, correct index, unit
  {
    // The unit is ignored since there is no unit in the address
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, float(5678.), ossia::rgb_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 5678., 0.));
  }

  // Message float, incorrect index, no unit
  {
    // The message is ignored
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{12}}, float(2222.), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 5678., 0.));
  }

  // Message float, incorrect index, unit
  {
    // The message is also ignored
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{12}}, float(2222.), ossia::rgb_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 5678., 0.));
  }

  // TODO what about the case where the index is correct in the destination unit ?

  // Message vec, no index, no unit
  {
    ossia::message m{*t.vec3f_addr, ossia::make_vec(0., 1., 2.), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 1., 2.));
  }

  // Message vec, index, no unit
  {
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, ossia::make_vec(12., 1234., 27.), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 1234., 2.));
  }

  // Message vec, no index, unit
  {
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, ossia::make_vec(0., 1., 2.), ossia::rgb_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 1., 2.));
  }

  // Message vec, index, unit
  {
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, ossia::make_vec(12., 1234., 27.), ossia::rgb_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 1234., 2.));
  }

  /////// Address vec, unit ///////
  t.vec3f_addr->push_value(ossia::make_vec(0., 0., 0.));
  t.vec3f_addr->set_unit(ossia::rgb_u{});

  // Message float, no index, no unit
  {
    ossia::message m{*t.vec3f_addr, float(1234.), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(1234., 1234., 1234.));
  }

  t.vec3f_addr->push_value(ossia::make_vec(0., 0., 0.));
  // Message float, no index, unit
  {
    ossia::message m{*t.vec3f_addr, float(1234.), t.vec3f_addr->get_unit()};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(1234., 1234., 1234.));
  }

  t.vec3f_addr->push_value(ossia::make_vec(0., 0., 0.));
  // Message float, index, no unit
  {
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, float(1234.), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 1234., 0.));
  }

  // Message float, index, same unit
  {
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, float(5678.), t.vec3f_addr->get_unit()};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0., 5678., 0.));
  }

  // Message float, index, different unit and same dataspace
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{2}}, float(0.7), ossia::hsv_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0.7, 0.7, 0.7));
  }

  // Message float, index, different dataspace
  {
    // Ignored
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{2}}, float(0.7), ossia::axis_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0.5, 0.5, 0.5));
  }

  // Message vec, no index, no unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0.2, 0.3, 0.4));
  }

  // Message vec, no index, same unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), t.vec3f_addr->get_unit()};
    m.launch();
    REQUIRE(t.vec3f_addr->value() == ossia::make_vec(0.2, 0.3, 0.4));
  }

  // Message vec, no index, different unit and same dataspace
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    // hsv{.2, .3, .4} == hsv{72°, 30%, 40%}
    ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), ossia::hsv_u{}};
    m.launch();
    REQUIRE(fuzzy_compare(t.vec3f_addr->value().get<ossia::vec3f>(), ossia::make_vec(0.376, 0.4, 0.28)));
  }

  // Message vec, no index, different dataspace
  {
    // ignored
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), ossia::axis_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
  }


  // Message vec, index, no unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, ossia::make_vec(0.2, 0.3, 0.4), {}};
    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
  }

  // Message vec, index, same unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, ossia::make_vec(0.2, 0.3, 0.4), t.vec3f_addr->get_unit()};
    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
  }

  // Message vec, index, different unit and same dataspace
  {
    // The whole value may be affected :
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, ossia::make_vec(0.2, 0.3, 0.4), ossia::hsv_u{}};
    m.launch();
    // What is launched is a message looking like hsv{ 0., 0.3, 0.5 }:
    // - first the current color rgb{0.5, 0.5, 0.5} is converted to hsv{0., 0., 0.5}
    // - the value in the message is applied (here s = 0.3) : hsv{0, 0.3, 0.5}
    // - the *whole* vec is reconverted to rgb{0.5, 0.35, 0.35} and applied

    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.35, 0.35));
  }

  // Message vec, index, different dataspace
  {
    // Ignored
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::message m{{*t.vec3f_addr, ossia::destination_index{1}}, ossia::make_vec(0.2, 0.3, 0.4), ossia::axis_u{}};
    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
  }


  // Piecewise Vecf, 1 member, no unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), {}, make_bitset(false, true, false)};
    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
  }

  // Piecewise Vecf, 1 member, same unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          t.vec3f_addr->get_unit(),
          make_bitset(false, true, false)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
  }

  // Piecewise Vecf, 1 member, different unit and same dataspace
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          ossia::hsv_u{},
      make_bitset(false, true, false)};
    // rgb(0.5, 0.5, 0.5)
    // hsv(0, 0, 0.5)
    // hsv(0, 0.3, 0.5)
    // rgb(0.5, 0.35, 0.35)

    m.launch();
    REQUIRE(fuzzy_compare(t.vec3f_addr->value().get<ossia::vec3f>(), ossia::make_vec(0.5, 0.35, 0.35)));
  }

  // Piecewise Vecf, 1 member, different unit and same dataspace
  {
    t.vec3f_addr->push_value(ossia::make_vec(1., 0., 0.));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.5, 0., 0.),
          ossia::hsv_u{},
      make_bitset(true, false, false)};
    // rgb(1, 0, 0)
    // hsv(0, 1, 1)
    // hsv(0.5, 1, 1)
    // rgb(0, 1, 1)

    m.launch();
    REQUIRE(fuzzy_compare(t.vec3f_addr->value().get<ossia::vec3f>(), ossia::make_vec(0.0, 1., 1.)));
  }


  // Piecewise Vecf, 1 member, different dataspace
  {
    // ignore
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          ossia::axis_u{},
      make_bitset(false, true, false)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
  }


  // Piecewise Vecf, 2 member, no unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
      {},
      make_bitset(true, true, false)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.2, 0.3, 0.5));
  }

  // Piecewise Vecf, 2 member, same unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          t.vec3f_addr->get_unit(),
          make_bitset(true, true, false)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.2, 0.3, 0.5));
  }

  // Piecewise Vecf, 2 member, different unit and same dataspace
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          ossia::hsv_u{},
      make_bitset(true, true, false)};

    m.launch();
    REQUIRE(fuzzy_compare(t.vec3f_addr->value().get<ossia::vec3f>(), ossia::make_vec(0.47, 0.5, 0.35)));
  }

  // Piecewise Vecf, 2 member, different dataspace
  {
    // ignore
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          ossia::axis_u{},
      make_bitset(true, true, false)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
  }



  // Piecewise Vecf, all members, no unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
      {},
      make_bitset(true, true, true)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.2, 0.3, 0.4));
  }

  // Piecewise Vecf, all members, same unit
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          t.vec3f_addr->get_unit(),
          make_bitset(true, true, true)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.2, 0.3, 0.4));
  }

  // Piecewise Vecf, all members, different unit and same dataspace
  {
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          ossia::hsv_u{},
      make_bitset(true, true, true)};

    m.launch();
    debug(t.vec3f_addr->value());
    REQUIRE(fuzzy_compare(t.vec3f_addr->value().get<ossia::vec3f>(), ossia::make_vec(0.376, 0.40, 0.28)));
  }

  // Piecewise Vecf, all members, different dataspace
  {
    // ignore
    t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));
    ossia::piecewise_vec_message<3> m{
      *t.vec3f_addr,
          ossia::make_vec(0.2, 0.3, 0.4),
          ossia::axis_u{},
      make_bitset(true, true, true)};

    m.launch();
    REQUIRE(t.vec3f_addr->value().get<ossia::vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
  }
}

TEST_CASE ("flatten_various", "flatten_various")
{
  ossia::TestDevice t;

  {
    ossia::state s;

    ossia::message m1{*t.float_addr, 123.f, {}};
    ossia::message m2{*t.float_addr, 456.f, {}};

    ossia::flatten_and_filter(s, m1);
    REQUIRE(s.size() == 1);
    ossia::flatten_and_filter(s, m2);
    REQUIRE(s.size() == 2);
  }
  {
    ossia::state s;

    ossia::message m1{*t.int_addr, 123, {}};
    ossia::message m2{*t.int_addr, 456, {}};

    ossia::flatten_and_filter(s, m1);
    REQUIRE(s.size() == 1);
    ossia::flatten_and_filter(s, m2);
    REQUIRE(s.size() == 2);
  }
  {
    ossia::state s;

    ossia::message m1{*t.char_addr, 'h', {}};
    ossia::message m2{*t.char_addr, 'c', {}};

    ossia::flatten_and_filter(s, m1);
    REQUIRE(s.size() == 1);
    ossia::flatten_and_filter(s, m2);
    REQUIRE(s.size() == 2);
  }
  {
    ossia::state s;

    ossia::message m1{*t.string_addr, "bar", {}};
    ossia::message m2{*t.string_addr, "foo", {}};

    ossia::flatten_and_filter(s, m1);
    REQUIRE(s.size() == 1);
    ossia::flatten_and_filter(s, m2);
    REQUIRE(s.size() == 2);
  }
  {
    ossia::state s;

    ossia::message m1{*t.impulse_addr, ossia::impulse{}, {}};
    ossia::message m2{*t.impulse_addr, ossia::impulse{}, {}};

    ossia::flatten_and_filter(s, m1);
    REQUIRE(s.size() == 1);
    ossia::flatten_and_filter(s, m2);
    REQUIRE(s.size() == 2);
  }
  {
    ossia::state s;

    ossia::message m1{*t.bool_addr, true, {}};
    ossia::message m2{*t.bool_addr, true, {}};

    ossia::flatten_and_filter(s, m1);
    REQUIRE(s.size() == 1);
    ossia::flatten_and_filter(s, m2);
    REQUIRE(s.size() == 2);
  }
  {
    ossia::state s;

    ossia::message m1{*t.bool_addr, true, {}};
    ossia::message m2{*t.bool_addr, false, {}};

    ossia::flatten_and_filter(s, m1);
    REQUIRE(s.size() == 1);
    ossia::flatten_and_filter(s, m2);
    REQUIRE(s.size() == 2);
  }


}

TEST_CASE ("flatten_same_vec_message_on_vec_address", "flatten_same_vec_message_on_vec_address")
{
  ossia::TestDevice t;
  t.vec3f_addr->set_unit(ossia::rgb_u{});
  t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));


  { // cref
    ossia::state s;

    ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);
  }
}


TEST_CASE ("flatten_different_vec_message_on_vec_address", "flatten_different_vec_message_on_vec_address")
{
  ossia::TestDevice t;
  t.vec3f_addr->set_unit(ossia::rgb_u{});
  t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));

  { // cref
    ossia::state s;

    ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};
    ossia::message m2{*t.vec3f_addr, ossia::make_vec(0.4, 0.5, 0.6), {}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m2);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m2);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};
    ossia::message m2{*t.vec3f_addr, ossia::make_vec(0.4, 0.5, 0.6), {}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m2});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m2);
  }
}

TEST_CASE ("flatten_different_float_message_on_vec_parameter_without_unit", "flatten_different_float_message_on_vec_parameter_without_unit")
{
  ossia::TestDevice t;
  t.vec3f_addr->set_unit(ossia::rgb_u{});
  t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));

  { // cref
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, {}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, {}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m2);

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, {}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, {}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m2});

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }

  // Same but without an unit on the address
  t.vec3f_addr->set_unit({});
  t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));

  { // cref
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, {}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, {}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m2);

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, {}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, {}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m2});

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }
}


TEST_CASE ("flatten_different_float_message_on_vec_parameter_with_same_unit", "flatten_different_float_message_on_vec_parameter_with_same_unit")
{
  ossia::TestDevice t;
  t.vec3f_addr->set_unit(ossia::rgb_u{});
  t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));

  { // cref
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, ossia::rgb_u{}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, ossia::rgb_u{}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m2);

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::rgb_u{}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, ossia::rgb_u{}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, ossia::rgb_u{}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m2});

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::rgb_u{}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }

}

TEST_CASE ("flatten_different_float_message_on_vec_parameter_with_different_unit", "flatten_different_float_message_on_vec_parameter_with_different_unit")
{
  ossia::TestDevice t;
  t.vec3f_addr->set_unit(ossia::rgb_u{});
  t.vec3f_addr->push_value(ossia::make_vec(0.5, 0.5, 0.5));

  { // cref
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, ossia::hsv_u{}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, ossia::hsv_u{}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m2);

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::hsv_u{}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{{*t.vec3f_addr, ossia::destination_index{0}}, float{1.}, ossia::hsv_u{}};
    ossia::message m2{{*t.vec3f_addr, ossia::destination_index{2}}, float{5.}, ossia::hsv_u{}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m2});

    REQUIRE(s.size() == 1);
    ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::hsv_u{}, make_bitset(true, false, true)};
    REQUIRE(*s.begin() == expected);
  }
}

TEST_CASE ("flatten_same_tuple_message_on_tuple_address", "flatten_same_tuple_message_on_tuple_address")
{
  ossia::TestDevice t;
  t.tuple_addr->set_unit({});

  { // cref
    ossia::state s;

    ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, {}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m1);

    // No unit: no flattening
    REQUIRE(s.size() == 2);
    REQUIRE(*s.begin() == m1);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, {}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 2);
    REQUIRE(*s.begin() == m1);
  }


  t.tuple_addr->set_unit(ossia::rgb_u{});

  { // cref
    ossia::state s;

    ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, ossia::rgb_u{}};

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, m1);

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);
  }

  { // rvalue
    ossia::state s;

    ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, ossia::rgb_u{}};

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);

    ossia::flatten_and_filter(s, ossia::message{m1});

    REQUIRE(s.size() == 1);
    REQUIRE(*s.begin() == m1);
  }
}

TEST_CASE ("test_merge_vec_in_tuple", "test_merge_vec_in_tuple")
{
  ossia::TestDevice t;
  ossia::state s;

  ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, {}};
  ossia::flatten_and_filter(s, ossia::message{m1});

  ossia::message m2{*t.tuple_addr, ossia::vec3f{0., 0., 0.}, {}};
  ossia::flatten_and_filter(s, ossia::message{m2});

  REQUIRE(s.size() == 2);
  /*
    ossia::message m3{*t.tuple_addr, std::vector<ossia::value>{0., 0., 0.}, {}};
    ossia::print(std::cerr, m3);
    std::cerr << std::endl;
    ossia::print(std::cerr, *s.begin());
    REQUIRE(*s.begin() == m1);
*/
}
TEST_CASE ("test_merge_tuple_in_vec", "test_merge_tuple_in_vec")
{
  ossia::TestDevice t;
  ossia::state s;

  ossia::message m1{*t.vec3f_addr, ossia::vec3f{0.f, 0.5f, 0.2f}, {}};
  ossia::flatten_and_filter(s, ossia::message{m1});

  ossia::message m2{*t.vec3f_addr, std::vector<ossia::value>{0.f, 0.f, 0.f}, {}};
  ossia::flatten_and_filter(s, ossia::message{m2});

  REQUIRE(s.size() == 1);

  ossia::message m3{*t.vec3f_addr, ossia::vec3f{0.f, 0.f, 0.f}, {}};
  ossia::print(std::cerr, m3);
  std::cerr << std::endl;
  ossia::print(std::cerr, *s.begin());
  REQUIRE(*s.begin() == m3);

}
