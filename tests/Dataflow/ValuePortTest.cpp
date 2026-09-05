// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../Network/TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/value_port.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <catch2/catch_approx.hpp>

#include <cmath>

using namespace Catch::literals;
using namespace ossia;
TEMPLATE_TEST_CASE(
    "test_add_global_value", "test_add_global_value", ossia::value,
    ossia::value_vector<ossia::value>)
{
  using T = TestType;
  auto add_func
      = [](ossia::value_port& p, ossia::net::parameter_base& addr, auto&& val) {
    if constexpr(std::is_same_v<T, ossia::value>)
    {
      p.add_global_value(addr, std::move(val));
    }
    else
    {
      p.add_global_values(addr, {std::move(val)});
    }
  };
  GIVEN("A default value port")
  {
    ossia::value_port p;
    TestDevice dev;

    WHEN("A value is added through a parameter")
    {
      add_func(p, *dev.int_addr, 123);

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 123);
      }
    }

    WHEN("A value is added through a parameter with an unit")
    {
      add_func(p, *dev.meter, 123);

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 123);
      }
    }

    WHEN("A value is added through a parameter with a domain")
    {
      add_func(p, *dev.range1, 123);

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 123);
      }
    }

    WHEN("A value is added through a parameter with a domain and a unit")
    {
      add_func(p, *dev.range1_m, 123);

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 123);
      }
    }
  }

  GIVEN("A value port with an index")
  {
    ossia::value_port p;
    p.index = {1};
    TestDevice dev;

    WHEN("A value is added through a wrong parameter")
    {
      add_func(p, *dev.float_addr, 0.5);

      THEN("Invalid value")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == ossia::value{});
      }
    }

    WHEN("A value is added through a vec parameter")
    {
      add_func(p, *dev.vec3f_addr, ossia::vec3f{-0.2, 0.3, 0.7});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }

    WHEN("A value is added through a parameter with an unit")
    {
      add_func(p, *dev.rgb, ossia::vec3f{0.1, 0.3, 0.5});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }

    WHEN("A value is added through a parameter with a float domain")
    {
      add_func(p, *dev.vec3f_range_f, ossia::vec3f{0.1, 0.3, 0.5});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }
    WHEN("A value is added through a parameter with a vec domain")
    {
      add_func(p, *dev.vec3f_range_vec3f, ossia::vec3f{0.1, 0.3, 0.5});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }

    WHEN("A value is added through a parameter with a float domain and a unit")
    {
      add_func(p, *dev.vec3f_range_f_unit, ossia::vec3f{0.1, 0.3, 0.5});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }

    WHEN("A value is added through a parameter with a vec domain and a unit")
    {
      add_func(p, *dev.vec3f_range_vec3f_unit, ossia::vec3f{0.1, 0.3, 0.5});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }
  }

  GIVEN("A value port with a domain")
  {
    ossia::value_port p;
    p.domain = ossia::make_domain(-1., 1.);
    TestDevice dev;

    WHEN("A value is added through a parameter")
    {
      add_func(p, *dev.float_addr, 0.5);

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.5);
      }
    }

    WHEN("A value is added through a parameter outside the bounds")
    {
      add_func(p, *dev.float_addr, 1.5);

      THEN("It is in the data of the port, clamped")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        // TODO REQUIRE(p.get_data()[0].value == 1.);
      }
    }

    WHEN("A value is added through a parameter with an unit")
    {
      add_func(p, *dev.meter, 0.2);

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.2);
      }
    }

    WHEN("A value is added through a parameter with a domain")
    {
      add_func(p, *dev.range1, 3.);

      THEN("It is in the data of the port, scaled")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == (3. / 5.));
      }
    }

    WHEN("A value is added through a parameter with a domain and a unit")
    {
      add_func(p, *dev.range1_m, 3.);

      THEN("It is in the data of the port, scaled")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == (3. / 5.));
      }
    }
  }

  GIVEN("A value port with an index and a domain")
  {
    using Approx = Catch::Approx;
    ossia::value_port p;
    p.domain = ossia::make_domain(-1., 1.);
    p.index = {1};
    TestDevice dev;

    WHEN("A value is added through a wrong parameter")
    {
      add_func(p, *dev.float_addr, 0.5);

      THEN("Value is invalid")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == ossia::value{});
      }
    }

    WHEN("A value is added through a vec parameter")
    {
      add_func(p, *dev.vec3f_addr, ossia::vec3f{-0.2, 0.3, 0.7});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }

    WHEN("A value is added through a parameter with an unit")
    {
      add_func(p, *dev.rgb, ossia::vec3f{0.1, 0.3, 0.5});

      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 0.3);
      }
    }

    WHEN("A value is added through a parameter with a float domain")
    {
      add_func(p, *dev.vec3f_range_f, ossia::vec3f{1, 2, 3});

      // 2 between -5 ; 5, maps to 2/5 between -1; 1
      THEN("It is in the data of the port, scaled")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 2. / 5.);
      }
    }
    WHEN("A value is added through a parameter with a vec domain")
    {
      add_func(p, *dev.vec3f_range_vec3f, ossia::vec3f{1, 1, 1});

      // 1 between -3 ; 3, maps to 1/3 between -1; 1
      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value == 1. / 3.);
      }
    }

    WHEN("A value is added through a parameter with a float domain and a unit")
    {
      add_func(p, *dev.vec3f_range_f_unit, ossia::vec3f{0.1, 0.6, 0.3});

      // 0.6 between 0.2 and 0.8 -> -1 + (1 - -1) / (0.8 - 0.2) * (0.6 - 0.2)
      // => 0
      THEN("It is in the data of the port, unchanged")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value.target<float>());
        REQUIRE(*p.get_data()[0].value.target<float>() == Approx(1. / 3.));
      }
    }

    WHEN("A value is added through a parameter with a vec domain and a unit")
    {
      add_func(p, *dev.vec3f_range_vec3f_unit, ossia::vec3f{0.1, 0.5, 0.6});

      // 0.5 between 0.3 and 0.6 -> -1 + (1 - -1) / (0.6 - 0.3) * (0.5 - 0.3)
      // => 1/3

      THEN("It is in the data of the port, scaled")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[0].value.target<float>());
        REQUIRE(*p.get_data()[0].value.target<float>() == Approx(1. / 3.));
      }
    }
  }
}

TEST_CASE("test_write_value_timestamp", "test_write_value_timestamp")
{
  GIVEN("A default value port")
  {
    ossia::value_port p;
    REQUIRE(p.mix_method == ossia::data_mix_method::mix_append);

    WHEN("A value is written with a timestamp")
    {
      p.write_value(123, 64);

      THEN("The timestamp is kept")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value == 123);
        REQUIRE(p.get_data()[0].timestamp == 64);
      }
    }

    WHEN("A moved-from value is written with a timestamp")
    {
      ossia::value v{456};
      p.write_value(std::move(v), 128);

      THEN("The timestamp is kept")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value == 456);
        REQUIRE(p.get_data()[0].timestamp == 128);
      }
    }

    WHEN("Several values are written at increasing timestamps")
    {
      p.write_value(1, 0);
      p.write_value(2, 10);
      p.write_value(3, 20);

      THEN("They are all kept, in order, with their own timestamp")
      {
        REQUIRE(p.get_data().size() == 3);
        for(int i = 0; i < 3; i++)
        {
          REQUIRE(p.get_data()[i].value == i + 1);
          REQUIRE(p.get_data()[i].timestamp == i * 10);
        }
      }
    }

    WHEN("Two values share a timestamp")
    {
      // A MIDI-to-array stream writes the note-off and the note-on of a step at
      // the same sample: appending must not collapse them.
      p.write_value(1, 42);
      p.write_value(2, 42);

      THEN("Both are kept, in order")
      {
        REQUIRE(p.get_data().size() == 2);
        REQUIRE(p.get_data()[0].value == 1);
        REQUIRE(p.get_data()[1].value == 2);
        REQUIRE(p.get_data()[0].timestamp == 42);
        REQUIRE(p.get_data()[1].timestamp == 42);
      }
    }
  }

  GIVEN("A value port that replaces")
  {
    ossia::value_port p;
    p.mix_method = ossia::data_mix_method::mix_replace;

    WHEN("Two values are written at the same timestamp")
    {
      p.write_value(1, 42);
      p.write_value(2, 42);

      THEN("The last one wins")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value == 2);
        REQUIRE(p.get_data()[0].timestamp == 42);
      }
    }

    WHEN("Values are written at distinct timestamps")
    {
      // Only reachable once the timestamp is actually stored: comparing against
      // entries that were all stamped 0 never matched anything else than 0.
      p.write_value(1, 0);
      p.write_value(2, 10);
      p.write_value(3, 10);

      THEN("Only the matching one is replaced")
      {
        REQUIRE(p.get_data().size() == 2);
        REQUIRE(p.get_data()[0].value == 1);
        REQUIRE(p.get_data()[0].timestamp == 0);
        REQUIRE(p.get_data()[1].value == 3);
        REQUIRE(p.get_data()[1].timestamp == 10);
      }
    }
  }
}

// A port's effective type is the unit its values are expected in: what the
// process declares, overridden by the @[unit] of its address.
TEST_CASE("test_port_unit_conversion", "test_port_unit_conversion")
{
  TestDevice dev;

  auto quat = dev.device.create_child("quat")->create_parameter(val_type::VEC4F);
  quat->set_unit(ossia::quaternion_u{});

  GIVEN("A port asking for another unit of the same dataspace")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::millimeter_u{}};

    WHEN("A value comes from a parameter in meters")
    {
      p.add_global_value(*dev.meter, 2.f);

      THEN("It arrives converted")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value == ossia::value{2000.f});
      }
    }
  }

  GIVEN("A port asking for the unit the parameter already has")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::quaternion_u{}};

    WHEN("A quaternion comes in")
    {
      p.add_global_value(*quat, ossia::vec4f{0.1f, 0.2f, 0.3f, 0.4f});

      THEN("It arrives untouched")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value == ossia::value{ossia::vec4f{0.1f, 0.2f, 0.3f, 0.4f}});
      }
    }
  }

  GIVEN("A port asking for euler angles")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::euler_u{}};

    WHEN("A quaternion comes in")
    {
      p.add_global_value(*quat, ossia::vec4f{1.f, 0.f, 0.f, 0.f});

      THEN("It arrives as the three angles of that orientation")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value.get_type() == ossia::val_type::VEC3F);
      }
    }
  }

  GIVEN("A port asking for euler angles, fed a quaternion that is not a unit one")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::euler_u{}};

    WHEN("The quaternion comes off the network as it is")
    {
      p.add_global_value(*quat, ossia::vec4f{0.9f, 0.8f, 0.7f, 0.6f});

      THEN("The three angles are finite")
      {
        REQUIRE(p.get_data().size() == 1);
        auto* v = p.get_data()[0].value.target<ossia::vec3f>();
        REQUIRE(v);
        for(float angle : *v)
          CHECK(std::isfinite(angle));
      }
    }
  }

  GIVEN("A port that declares a unit and whose address asks for another")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::meter_u{}};
    p.address_unit = ossia::millimeter_u{};

    WHEN("A value comes in")
    {
      THEN("The address's unit is the one that counts")
      {
        REQUIRE(p.effective_type() == ossia::complex_type{ossia::millimeter_u{}});
      }
    }

    WHEN("The address is cleared")
    {
      p.address_unit = {};

      THEN("The port is back to what it declared")
      {
        REQUIRE(p.effective_type() == ossia::complex_type{ossia::meter_u{}});
      }
    }
  }

  GIVEN("A port that only declares a unit")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::millimeter_u{}};

    WHEN("A value comes from a parameter in meters")
    {
      p.add_global_value(*dev.meter, 2.f);

      THEN("The declaration converts just as an address qualifier would")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value == ossia::value{2000.f});
      }
    }
  }

  GIVEN("A port asking for an axis-angle orientation, fed the same quaternion")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::axis_u{}};

    WHEN("The quaternion comes off the network as it is")
    {
      p.add_global_value(*quat, ossia::vec4f{0.9f, 0.8f, 0.7f, 0.6f});

      THEN("The axis and the angle are finite")
      {
        REQUIRE(p.get_data().size() == 1);
        auto* v = p.get_data()[0].value.target<ossia::vec4f>();
        REQUIRE(v);
        for(float c : *v)
          CHECK(std::isfinite(c));
      }
    }
  }

  GIVEN("A port in millimetres, ranged [0;1], fed a parameter in metres ranged [0;2]")
  {
    auto metres = dev.device.create_child("height")->create_parameter(val_type::FLOAT);
    metres->set_unit(ossia::meter_u{});
    metres->set_domain(ossia::make_domain(0.f, 2.f));

    ossia::value_port p;
    p.type = ossia::unit_t{ossia::millimeter_u{}};
    p.domain = ossia::make_domain(0.f, 1.f);

    WHEN("One metre arrives")
    {
      p.add_global_value(*metres, 1.f);

      THEN("It is a thousand millimetres, clipped to the range - never rescaled")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1.f});
      }
    }
  }

  GIVEN("A port in millimetres with room for the converted value")
  {
    auto metres = dev.device.create_child("h2")->create_parameter(val_type::FLOAT);
    metres->set_unit(ossia::meter_u{});
    metres->set_domain(ossia::make_domain(0.f, 2.f));

    ossia::value_port p;
    p.type = ossia::unit_t{ossia::millimeter_u{}};
    p.domain = ossia::make_domain(0.f, 5000.f);

    WHEN("One metre arrives")
    {
      p.add_global_value(*metres, 1.f);

      THEN("It arrives as the quantity it is")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1000.f});
      }
    }
  }

  GIVEN("A port asking for a unit of another dataspace")
  {
    ossia::value_port p;
    p.type = ossia::unit_t{ossia::rgb_u{}};

    WHEN("A quaternion comes in")
    {
      p.add_global_value(*quat, ossia::vec4f{0.1f, 0.2f, 0.3f, 0.4f});

      THEN("There is nothing to convert and it goes through as it is")
      {
        REQUIRE(p.get_data().size() == 1);
        REQUIRE(p.get_data()[0].value == ossia::value{ossia::vec4f{0.1f, 0.2f, 0.3f, 0.4f}});
      }
    }
  }
}

// The orientation conversions in isolation: nothing normalises a quaternion that
// arrives over the network.
TEST_CASE("test_orientation_conversion_of_a_non_unit_quaternion", "test_orientation")
{
  const ossia::vec4f q{0.9f, 0.8f, 0.7f, 0.6f};

  GIVEN("A quaternion whose norm is not one")
  {
    WHEN("It is converted to euler angles")
    {
      auto v = ossia::convert(
          ossia::value{q}, ossia::unit_t{ossia::quaternion_u{}},
          ossia::unit_t{ossia::euler_u{}});

      THEN("The three angles are finite")
      {
        auto* e = v.target<ossia::vec3f>();
        REQUIRE(e);
        for(float angle : *e)
          CHECK(std::isfinite(angle));
      }
    }

    WHEN("It is converted to an axis and an angle")
    {
      auto v = ossia::convert(
          ossia::value{q}, ossia::unit_t{ossia::quaternion_u{}},
          ossia::unit_t{ossia::axis_u{}});

      THEN("The four components are finite")
      {
        auto* a = v.target<ossia::vec4f>();
        REQUIRE(a);
        for(float c : *a)
          CHECK(std::isfinite(c));
      }
    }
  }

  GIVEN("A unit quaternion")
  {
    const ossia::vec4f id{0.f, 0.f, 0.f, 1.f};

    WHEN("It is converted to euler angles")
    {
      auto v = ossia::convert(
          ossia::value{id}, ossia::unit_t{ossia::quaternion_u{}},
          ossia::unit_t{ossia::euler_u{}});

      THEN("The identity orientation is all zeroes")
      {
        auto* e = v.target<ossia::vec3f>();
        REQUIRE(e);
        for(float angle : *e)
          CHECK(angle == Catch::Approx(0.).margin(1e-4));
      }
    }
  }
}

// When both sides declare a unit the conversion decides what the numbers are, so
// the sink's domain bounds the result rather than rescaling it.
TEST_CASE("test_unit_and_domain_ordering", "test_unit_and_domain_ordering")
{
  TestDevice dev;

  auto ranged = [&](const char* name, ossia::unit_t u, float min, float max) {
    auto p = dev.device.create_child(name)->create_parameter(val_type::FLOAT);
    if(u)
      p->set_unit(u);
    p->set_domain(ossia::make_domain(min, max));
    return p;
  };

  GIVEN("A source and a sink in the same unit, with different ranges")
  {
    auto metres = ranged("same_u", ossia::meter_u{}, 0.f, 2.f);

    ossia::value_port p;
    p.type = ossia::unit_t{ossia::meter_u{}};
    p.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value arrives")
    {
      p.add_global_value(*metres, 1.5f);

      THEN("It is bounded, not rescaled - 1.5 m stays 1.5 m and clips, it is "
           "not three quarters of the way along")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1.f});
      }
    }
  }

  GIVEN("A source with a unit and a sink without one")
  {
    auto metres = ranged("half_u_src", ossia::meter_u{}, 0.f, 2.f);

    ossia::value_port p;
    p.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value arrives")
    {
      p.add_global_value(*metres, 1.f);

      THEN("Nothing established what the sink's numbers are, so they are still "
           "rescaled the way they always were")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{0.5f});
      }
    }
  }

  GIVEN("A source without a unit and a sink with one")
  {
    auto plain = ranged("half_u_snk", {}, 0.f, 2.f);

    ossia::value_port p;
    p.type = ossia::unit_t{ossia::meter_u{}};
    p.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value arrives")
    {
      p.add_global_value(*plain, 1.f);

      THEN("There was no conversion to preserve, so it is rescaled")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{0.5f});
      }
    }
  }

  GIVEN("Two units that belong to different dataspaces, both with ranges")
  {
    auto metres = ranged("cross_ds", ossia::meter_u{}, 0.f, 2.f);

    ossia::value_port p;
    p.type = ossia::unit_t{ossia::rgb_u{}};
    p.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value arrives")
    {
      p.add_global_value(*metres, 1.f);

      THEN("Nothing converts, so the units say nothing and the ranges map as "
           "they would without them")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{0.5f});
      }
    }
  }

  GIVEN("A source and a sink in the same unit, read as events")
  {
    auto metres = ranged("same_u_ev", ossia::meter_u{}, 0.f, 2.f);

    ossia::value_port p;
    p.type = ossia::unit_t{ossia::meter_u{}};
    p.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value arrives")
    {
      p.add_global_values(*metres, {1.5f});

      THEN("It is bounded, like on any other inlet")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1.f});
      }
    }
  }

  GIVEN("A sink with a range whose source has none")
  {
    auto metres = dev.device.create_child("no_src_dom")->create_parameter(val_type::FLOAT);
    metres->set_unit(ossia::meter_u{});

    ossia::value_port p;
    p.type = ossia::unit_t{ossia::millimeter_u{}};
    p.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value arrives")
    {
      p.add_global_value(*metres, 1.f);

      THEN("It is converted, then bounded: a bound is the sink's alone to set")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1.f});
      }
    }
  }
}

// The same rule over a cable rather than an address.
TEST_CASE("test_cable_unit_and_domain_ordering", "test_cable_unit_and_domain")
{
  auto cable = [](ossia::value_port& src, ossia::value_port& snk, ossia::value v) {
    src.write_value(std::move(v), 0);
    snk.add_port_values(src);
  };

  GIVEN("A metre port cabled into a millimetre port, both with ranges")
  {
    ossia::value_port src, snk;
    src.type = ossia::unit_t{ossia::meter_u{}};
    src.domain = ossia::make_domain(0.f, 2.f);
    snk.type = ossia::unit_t{ossia::millimeter_u{}};
    snk.domain = ossia::make_domain(0.f, 5000.f);

    WHEN("One metre goes down the cable")
    {
      cable(src, snk, 1.f);

      THEN("The sink sees a thousand millimetres")
      {
        REQUIRE(snk.get_data().size() == 1);
        CHECK(snk.get_data()[0].value == ossia::value{1000.f});
      }
    }
  }

  GIVEN("The same cable into a port with no room for the converted value")
  {
    ossia::value_port src, snk;
    src.type = ossia::unit_t{ossia::meter_u{}};
    src.domain = ossia::make_domain(0.f, 2.f);
    snk.type = ossia::unit_t{ossia::millimeter_u{}};
    snk.domain = ossia::make_domain(0.f, 1.f);

    WHEN("One metre goes down the cable")
    {
      cable(src, snk, 1.f);

      THEN("It is clipped, not rescaled")
      {
        REQUIRE(snk.get_data().size() == 1);
        CHECK(snk.get_data()[0].value == ossia::value{1.f});
      }
    }
  }

  GIVEN("A cable between two units with no ranges at all")
  {
    ossia::value_port src, snk;
    src.type = ossia::unit_t{ossia::meter_u{}};
    snk.type = ossia::unit_t{ossia::millimeter_u{}};

    WHEN("One metre goes down the cable")
    {
      cable(src, snk, 1.f);

      THEN("It still converts")
      {
        REQUIRE(snk.get_data().size() == 1);
        CHECK(snk.get_data()[0].value == ossia::value{1000.f});
      }
    }
  }

  GIVEN("A cable between two units of different dataspaces, with no ranges")
  {
    ossia::value_port src, snk;
    src.type = ossia::unit_t{ossia::meter_u{}};
    snk.type = ossia::unit_t{ossia::rgb_u{}};

    WHEN("A value goes down the cable")
    {
      cable(src, snk, 0.5f);

      THEN("It goes through rather than being blanked by a conversion that "
           "cannot mean anything")
      {
        REQUIRE(snk.get_data().size() == 1);
        CHECK(snk.get_data()[0].value == ossia::value{0.5f});
      }
    }
  }

  GIVEN("A cable from an unranged metre port into a ranged millimetre port")
  {
    ossia::value_port src, snk;
    src.type = ossia::unit_t{ossia::meter_u{}};
    snk.type = ossia::unit_t{ossia::millimeter_u{}};
    snk.domain = ossia::make_domain(0.f, 1.f);

    WHEN("One metre goes down the cable")
    {
      cable(src, snk, 1.f);

      THEN("It is converted, then bounded by the sink's range")
      {
        REQUIRE(snk.get_data().size() == 1);
        CHECK(snk.get_data()[0].value == ossia::value{1.f});
      }
    }
  }

  GIVEN("A cable between two units of different dataspaces, both with ranges")
  {
    ossia::value_port src, snk;
    src.type = ossia::unit_t{ossia::meter_u{}};
    src.domain = ossia::make_domain(0.f, 2.f);
    snk.type = ossia::unit_t{ossia::rgb_u{}};
    snk.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value goes down the cable")
    {
      cable(src, snk, 1.f);

      THEN("The units say nothing to each other and the ranges map")
      {
        REQUIRE(snk.get_data().size() == 1);
        CHECK(snk.get_data()[0].value == ossia::value{0.5f});
      }
    }
  }

  GIVEN("A cable between two ports that only have ranges")
  {
    ossia::value_port src, snk;
    src.domain = ossia::make_domain(0.f, 2.f);
    snk.domain = ossia::make_domain(0.f, 1.f);

    WHEN("A value goes down the cable")
    {
      cable(src, snk, 1.f);

      THEN("It is rescaled, exactly as it always was")
      {
        REQUIRE(snk.get_data().size() == 1);
        CHECK(snk.get_data()[0].value == ossia::value{0.5f});
      }
    }
  }
}

// A value out of the local state may already be the component the port asks for.
TEST_CASE("test_local_value_index", "test_local_value_index")
{
  const ossia::complex_type cart{ossia::cartesian_3d_u{}};
  const ossia::vec3f whole{1.5f, 2.5f, 3.5f};

  GIVEN("A port asking for the first component, without a unit")
  {
    ossia::value_port p;
    p.index = {0};

    WHEN("The first component arrives, in a unit")
    {
      p.add_local_value(
          ossia::typed_value{ossia::value{1.5f}, ossia::destination_index{0}, cart});

      THEN("It is written as it is")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1.5f});
      }
    }

    WHEN("The whole value arrives, in a unit")
    {
      p.add_local_value(
          ossia::typed_value{ossia::value{whole}, ossia::destination_index{}, cart});

      THEN("The component is extracted")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1.5f});
      }
    }
  }

  GIVEN("A port asking for the first component, in a unit")
  {
    ossia::value_port p;
    p.index = {0};
    p.type = cart;

    WHEN("The first component arrives, without a unit")
    {
      p.add_local_value(
          ossia::typed_value{ossia::value{1.5f}, ossia::destination_index{0}, {}});

      THEN("It is written as it is")
      {
        REQUIRE(p.get_data().size() == 1);
        CHECK(p.get_data()[0].value == ossia::value{1.5f});
      }
    }
  }
}
