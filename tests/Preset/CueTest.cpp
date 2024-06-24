
#include "../Network/TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/preset/cue.hpp>
#include <ossia/preset/preset.hpp>

#include "include_catch.hpp"

struct default_cue_device
{
  ossia::net::generic_device dev{"mydevice"};
  ossia::net::node_base& root = dev.get_root_node();
  ossia::net::node_base& n1 = ossia::net::find_or_create_node(root, "/foo/bar/baz");
  ossia::net::node_base& n2 = ossia::net::find_or_create_node(root, "/bim/bam");
  ossia::net::node_base& n3 = ossia::net::find_or_create_node(root, "/bim/boum");
  ossia::net::node_base& n4 = ossia::net::find_or_create_node(root, "/bim/boum.1");

  ossia::net::parameter_base& a1 = *n1.create_parameter(ossia::val_type::INT);
  ossia::net::parameter_base& a2 = *n2.create_parameter(ossia::val_type::FLOAT);
  ossia::net::parameter_base& a3 = *n3.create_parameter(ossia::val_type::STRING);
  ossia::net::parameter_base& a4 = *n4.create_parameter(ossia::val_type::STRING);

  default_cue_device()
  {
    using namespace std::literals;
    ossia::net::set_default_value(n1, 1234);
    ossia::net::set_default_value(n2, 5678.);
    ossia::net::set_default_value(n3, "hello"s);
    ossia::net::set_default_value(n4, "bye"s);

    a1.push_value(13579);
    a2.push_value(3.1415);
    a3.push_value("foo"s);
    a4.push_value("bar"s);
  }
};

auto at(const auto& preset, std::string_view name) -> const ossia::value&
{
  auto it = ossia::find_key(preset, name);
  REQUIRE(it != preset.end());
  return it->second;
};
TEST_CASE("test_cue", "test_cue")
{
  using namespace std::literals;

  default_cue_device device;
  ossia::cues c;
  ossia::namespace_selection sel;
  sel.set_device(&device.dev);

  WHEN("initial state")
  {
    THEN("one cue")
    {
      REQUIRE(c.size() == 1);
      REQUIRE(c.current_cue()->name == "Init");
    }
  }

  WHEN("creating a cue")
  {
    c.create("Cue");
    THEN("two cue")
    {
      REQUIRE(c.size() == 2);
      REQUIRE(c.current_cue()->name == "Cue");
      // REQUIRE(c.current_cue()->selection == std::vector<std::string>{"/"});
      REQUIRE(c.current_cue()->preset.empty());
    }
  }

  WHEN("updating the cue")
  {
    c.create("Cue");
    REQUIRE(c.size() == 2);
    REQUIRE(c.current_cue()->name == "Cue");
    c.update(device.dev.get_root_node(), sel, "Cue");
    THEN("entire tree is copied")
    {
      REQUIRE(c.current_cue()->name == "Cue");

      auto cue_ptr = c.current_cue();
      REQUIRE(cue_ptr);
      auto& cue = *cue_ptr;

      REQUIRE(cue.preset.size() == 4);

      REQUIRE(at(cue.preset, "/foo/bar/baz") == 13579);
      REQUIRE(at(cue.preset, "/bim/bam") == 3.1415);
      REQUIRE(at(cue.preset, "/bim/boum") == "foo"s);
      REQUIRE(at(cue.preset, "/bim/boum.1") == "bar"s);
    }
  }

  WHEN("recalling the cue")
  {
    // Check that the device is in the right state
    REQUIRE(device.a1.value() == 13579);
    REQUIRE(device.a2.value() == 3.1415);
    REQUIRE(device.a3.value() == "foo");
    REQUIRE(device.a4.value() == "bar");

    c.create("Cue 1");
    c.update(device.dev.get_root_node(), sel, "Cue 1");
    REQUIRE(c.m_cues[1].name == "Cue 1");
    REQUIRE(c.m_cues[1].preset.size() == 4);

    device.a1.set_value(10);
    device.a2.set_value(1.2);
    device.a3.set_value("a3");
    device.a4.set_value("a4");

    c.create("Cue 2");
    c.update(device.dev.get_root_node(), sel, "Cue 2");

    THEN("recalling sets the correct values")
    {
      c.recall(device.dev.get_root_node(), sel, "Cue 1");
      REQUIRE(device.a1.value() == 13579);
      REQUIRE(device.a2.value() == 3.1415);
      REQUIRE(device.a3.value() == "foo");
      REQUIRE(device.a4.value() == "bar");

      c.recall(device.dev.get_root_node(), sel, "Cue 2");
      REQUIRE(device.a1.value() == 10);
      REQUIRE(device.a2.value() == 1.2);
      REQUIRE(device.a3.value() == "a3");
      REQUIRE(device.a4.value() == "a4");
    }
  }
}

TEST_CASE("test_cue_ns", "test_cue_ns")
{
  using namespace std::literals;
  WHEN("deselecting the default /")
  {
    default_cue_device device;

    ossia::cues c;
    ossia::namespace_selection sel;
    sel.set_device(&device.dev);
    sel.namespace_deselect("/");

    THEN("updating does nothing")
    {
      REQUIRE(c.current_cue()->preset.empty());
      c.update(device.dev.get_root_node(), sel);
      REQUIRE(c.current_cue()->preset.empty());
    }
  }

  WHEN("selecting a node")
  {
    default_cue_device device;

    ossia::cues c;
    ossia::namespace_selection sel;
    sel.set_device(&device.dev);
    sel.namespace_deselect("/");
    REQUIRE(sel.m_selection.empty());

    sel.namespace_select("/bim");
    REQUIRE(sel.m_selection.size() == 4);

    c.update(device.dev.get_root_node(), sel);

    THEN("node now contains cues from /bim/bam and /bim/boum*")
    {
      auto* cue_ptr = c.current_cue();
      auto& cue = *cue_ptr;
      REQUIRE(cue.preset.size() == 3);
      REQUIRE(at(cue.preset, "/bim/bam") == 3.1415);
      REQUIRE(at(cue.preset, "/bim/boum") == "foo"s);
      REQUIRE(at(cue.preset, "/bim/boum.1") == "bar"s);
    }

    WHEN("deselecting a sub node")
    {
      sel.namespace_deselect("/bim/boum");

      c.clear();
      c.create("Cue");

      c.update(device.dev.get_root_node(), sel);

      THEN("node now contains cues from /bim/bam and /bim/boum.1 only")
      {
        auto* cue_ptr = c.current_cue();
        auto& cue = *cue_ptr;
        REQUIRE(cue.preset.size() == 2);
        REQUIRE(at(cue.preset, "/bim/bam") == 3.1415);
        REQUIRE(at(cue.preset, "/bim/boum.1") == "bar"s);
      }
    }
  }
}
