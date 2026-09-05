// The execution state watches a parameter for as long as any event port reads
// from it. Several ports routinely point at the same OSC address.

#include "../Network/TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <memory>

using namespace ossia;

namespace
{
// An event port, as score sets one up for a port bound to an address.
std::unique_ptr<ossia::value_inlet> bound_inlet(ossia::net::parameter_base& p)
{
  auto in = std::make_unique<ossia::value_inlet>();
  in->data.is_event = true;
  in->address = &p;
  return in;
}

// What the port sees for that parameter over one tick.
std::vector<ossia::value>
tick_values(ossia::execution_state& st, ossia::net::parameter_base& p, ossia::inlet& in)
{
  in.target<ossia::value_port>()->clear();
  st.copy_from_global(p, in);

  std::vector<ossia::value> res;
  for(const auto& v : in.target<ossia::value_port>()->get_data())
    res.push_back(v.value);
  return res;
}
}

TEST_CASE("Unbinding one port keeps the others fed", "[dataflow][port][registration]")
{
  TestDevice dev;

  ossia::execution_state st;
  st.register_device(&dev.device);
  st.apply_device_changes();

  auto a = bound_inlet(*dev.float_addr);
  auto b = bound_inlet(*dev.float_addr);

  GIVEN("Two ports bound to the same parameter")
  {
    st.register_port(*a);
    st.register_port(*b);

    dev.float_addr->push_value(1.f);
    st.begin_tick();

    THEN("Both of them receive its values")
    {
      CHECK(tick_values(st, *dev.float_addr, *a) == std::vector<ossia::value>{1.f});
      CHECK(tick_values(st, *dev.float_addr, *b) == std::vector<ossia::value>{1.f});
    }

    WHEN("One of them is unbound")
    {
      st.unregister_port(*a);

      dev.float_addr->push_value(2.f);
      st.begin_tick();

      THEN("The other one still receives")
      {
        CHECK(tick_values(st, *dev.float_addr, *b) == std::vector<ossia::value>{2.f});
      }

      AND_WHEN("The last one is unbound too")
      {
        st.unregister_port(*b);

        dev.float_addr->push_value(3.f);
        st.begin_tick();

        THEN("The parameter is no longer watched")
        {
          CHECK(tick_values(st, *dev.float_addr, *b).empty());
        }
      }
    }
  }

  st.unregister_device(&dev.device);
  st.apply_device_changes();
}

TEST_CASE("A port rebound to another parameter", "[dataflow][port][registration]")
{
  TestDevice dev;

  ossia::execution_state st;
  st.register_device(&dev.device);
  st.apply_device_changes();

  auto a = bound_inlet(*dev.float_addr);
  auto b = bound_inlet(*dev.float_addr);
  st.register_port(*a);
  st.register_port(*b);

  // What dropping an address on a port that already had one does.
  st.unregister_port(*a);
  a->address = dev.int_addr;
  st.register_port(*a);

  dev.float_addr->push_value(4.f);
  dev.int_addr->push_value(5);
  st.begin_tick();

  CHECK(tick_values(st, *dev.float_addr, *b) == std::vector<ossia::value>{4.f});
  CHECK(tick_values(st, *dev.int_addr, *a) == std::vector<ossia::value>{5});

  st.unregister_port(*a);
  st.unregister_port(*b);
  st.unregister_device(&dev.device);
  st.apply_device_changes();
}
