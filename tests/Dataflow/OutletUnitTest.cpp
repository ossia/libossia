// An outlet carries the unit its values are in; the commit turns it into the
// unit of the parameter it writes to.

#include "../Network/TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <catch2/catch_approx.hpp>

#include <memory>

using namespace ossia;

namespace
{
struct Fixture
{
  TestDevice dev;
  ossia::execution_state st;

  Fixture()
  {
    st.register_device(&dev.device);
    st.apply_device_changes();
  }

  ~Fixture()
  {
    st.unregister_device(&dev.device);
    st.apply_device_changes();
  }

  // Write one value out of a port in `port_unit`, addressed to `p`.
  void push(
      ossia::net::parameter_base& p, const ossia::complex_type& port_unit,
      const ossia::value& v, const ossia::destination_index& idx = {},
      const ossia::domain& dom = {})
  {
    ossia::value_outlet out;
    out.address = &p;
    out->type = port_unit;
    out->index = idx;
    out->domain = dom;
    out->write_value(v, 0);

    out.write(st);
    st.commit();
  }
};
}

TEST_CASE("An outlet's unit reaches the address", "[dataflow][port][unit]")
{
  Fixture f;

  GIVEN("A port in metres addressed to a parameter in millimetres")
  {
    WHEN("It writes two metres")
    {
      f.push(*f.dev.millimeter, ossia::unit_t{ossia::meter_u{}}, 2.f);

      THEN("The address receives two thousand millimetres")
      {
        CHECK(f.dev.millimeter->value() == ossia::value{2000.f});
      }
    }
  }

  GIVEN("A port whose unit is the one the address is already in")
  {
    WHEN("It writes two metres")
    {
      f.push(*f.dev.meter, ossia::unit_t{ossia::meter_u{}}, 2.f);

      THEN("The address receives them as they are")
      {
        CHECK(f.dev.meter->value() == ossia::value{2.f});
      }
    }
  }

  GIVEN("A port with a unit addressed to a parameter that has none")
  {
    WHEN("It writes a value")
    {
      f.push(*f.dev.float_addr, ossia::unit_t{ossia::meter_u{}}, 2.f);

      THEN("There is nothing to convert into and the value goes through")
      {
        CHECK(f.dev.float_addr->value() == ossia::value{2.f});
      }
    }
  }

  GIVEN("A port whose unit belongs to another dataspace than the address's")
  {
    f.dev.meter->push_value(7.f);

    WHEN("It writes a value")
    {
      f.push(*f.dev.meter, ossia::unit_t{ossia::rgb_u{}}, 2.f);

      THEN("The unit says nothing to the address and the value goes through")
      {
        CHECK(f.dev.meter->value() == ossia::value{2.f});
      }
    }
  }

  GIVEN("A ranged port in metres addressed to a ranged parameter in metres")
  {
    auto p = f.dev.device.create_child("m_ranged")->create_parameter(val_type::FLOAT);
    p->set_unit(ossia::meter_u{});
    p->set_domain(ossia::make_domain(0.f, 10.f));

    WHEN("It writes a value")
    {
      f.push(*p, ossia::unit_t{ossia::meter_u{}}, 1.5f, {}, ossia::make_domain(0.f, 2.f));

      THEN("The quantity is pushed as it is: bounding is the parameter's business")
      {
        CHECK(p->value() == ossia::value{1.5f});
      }
    }
  }

  GIVEN("A ranged port in metres addressed to a ranged parameter without a unit")
  {
    auto p = f.dev.device.create_child("f_ranged")->create_parameter(val_type::FLOAT);
    p->set_domain(ossia::make_domain(0.f, 10.f));

    WHEN("It writes a value")
    {
      f.push(*p, ossia::unit_t{ossia::meter_u{}}, 1.5f, {}, ossia::make_domain(0.f, 2.f));

      THEN("A unit on one side alone is inert and the ranges map")
      {
        CHECK(p->value() == ossia::value{7.5f});
      }
    }
  }

  GIVEN("A ranged port in one dataspace addressed to a ranged parameter in another")
  {
    auto p = f.dev.device.create_child("m_ranged2")->create_parameter(val_type::FLOAT);
    p->set_unit(ossia::meter_u{});
    p->set_domain(ossia::make_domain(0.f, 10.f));

    WHEN("It writes a value")
    {
      f.push(*p, ossia::unit_t{ossia::rgb_u{}}, 1.5f, {}, ossia::make_domain(0.f, 2.f));

      THEN("The units say nothing to each other and the ranges map")
      {
        CHECK(p->value() == ossia::value{7.5f});
      }
    }
  }

  GIVEN("A port with no unit at all")
  {
    WHEN("It writes to a parameter that has one")
    {
      f.push(*f.dev.millimeter, {}, 2.f);

      THEN("The value goes through untouched")
      {
        CHECK(f.dev.millimeter->value() == ossia::value{2.f});
      }
    }
  }
}

TEST_CASE("An outlet's index reaches the address", "[dataflow][port][unit]")
{
  Fixture f;

  f.dev.vec3f_addr->push_value(ossia::vec3f{1.f, 2.f, 3.f});

  GIVEN("A port addressing one component")
  {
    WHEN("It writes a value")
    {
      f.push(*f.dev.vec3f_addr, {}, 9.f, ossia::destination_index{1});

      THEN("Only that component changes")
      {
        CHECK(f.dev.vec3f_addr->value() == ossia::value{ossia::vec3f{1.f, 9.f, 3.f}});
      }
    }
  }

  GIVEN("A port addressing one component, in a unit the address is not in")
  {
    // The unit has to differ from the address's: with equal units, or none,
    // launch() merges the component raw either way and the case proves nothing.
    // Cyan is rgb(0, 1, 1), so setting red to 1 really moves it.
    const ossia::vec3f before{0.5f, 1.f, 1.f};
    f.dev.hsv->push_value(before);

    WHEN("It writes the first component as rgb into an hsv address")
    {
      f.push(
          *f.dev.hsv, ossia::unit_t{ossia::rgb_u{}}, 1.f, ossia::destination_index{0});

      THEN("The component is set in rgb, and the colour comes back as hsv")
      {
        const ossia::value after = f.dev.hsv->value();
        auto* v = after.target<ossia::vec3f>();
        REQUIRE(v);
        // rgb(0,1,1) with red set to 1 is white, which is hsv(_, 0, 1)
        CHECK((*v)[1] == Catch::Approx(0.).margin(1e-5));
        CHECK((*v)[2] == Catch::Approx(1.).margin(1e-5));
        // and not the raw merge into hue that dropping the unit produced
        CHECK(*v != ossia::vec3f{1.f, 1.f, 1.f});
      }
    }
  }
}

// mix_replace matches entries in the local state by timestamp, so every
// constructor taking a timed_value has to keep it.
TEST_CASE("A typed value keeps the timestamp it came with", "[dataflow][port]")
{
  const ossia::timed_value tv{ossia::value{1.f}, 42};
  const ossia::destination_index idx{1};
  const ossia::complex_type u{ossia::meter_u{}};

  CHECK(ossia::typed_value{tv}.timestamp == 42);
  CHECK(ossia::typed_value{tv, idx}.timestamp == 42);
  CHECK(ossia::typed_value{tv, u}.timestamp == 42);
  CHECK(ossia::typed_value{tv, idx, u}.timestamp == 42);

  CHECK(ossia::typed_value{ossia::timed_value{tv}, u}.timestamp == 42);
  CHECK(ossia::typed_value{ossia::timed_value{tv}, idx, u}.timestamp == 42);
}
