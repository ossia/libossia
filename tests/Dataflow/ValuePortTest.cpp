// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../Network/TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/value_port.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <catch2/catch_approx.hpp>

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
