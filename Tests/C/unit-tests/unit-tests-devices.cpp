#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <iostream>

#include <ossia-c/preset/preset.h>
#include <ossia-c/preset/preset.hpp>
#include <ossia/network/base/path.hpp>

#include <ossia/ossia.hpp>

TEST_CASE ("Device exceptions") {
  ossia::net::generic_device localDevice{
    std::make_unique<ossia::net::local_protocol>(), "test"};

  auto rootnode = localDevice.getRootNode().createChild("root");
  auto childnode = rootnode->createChild("child");
  auto childaddr = childnode->createAddress(ossia::val_type::BOOL);

  SECTION("Invalid address: target node has children") {
    ossia::presets::preset preset;
    bool b;
    preset.insert(std::make_pair("/root", b));

    REQUIRE_THROWS(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_off));
  }

  SECTION("Invalid address: keep_arch_type") {
    ossia::presets::preset preset;
    bool b;
    preset.insert(std::make_pair ("/root/child_that_does_not_exist", b));

    REQUIRE_THROWS(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_on));
    REQUIRE_NOTHROW(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_off));

  }
}

TEST_CASE ("Building device from preset") {
  ossia::net::generic_device localDevice{
    std::make_unique<ossia::net::local_protocol>(), "device"};

  ossia::presets::preset p;
  p.emplace("/a.0/b/c.0", 1);
  p.emplace("/a.0/b/c.1", 2);
  p.emplace("/a.1/b.0/c", 3);
  p.emplace("/a.1/b.0/d", 4);

  REQUIRE_NOTHROW(ossia::devices::apply_preset(localDevice, p, ossia::devices::keep_arch_off));
  auto& children = localDevice.children();

  using namespace ossia::devices;
  SECTION("Presence of nodes") {
    REQUIRE(get_node(localDevice, "/device/a.1") != nullptr);
    REQUIRE(get_node(localDevice, "/device/a.1/b") != nullptr);
    REQUIRE(get_node(localDevice, "/device/a.1/b/c.1") != nullptr);
    REQUIRE(get_node(localDevice, "/device/a.1/b/c.2") != nullptr);
    REQUIRE(get_node(localDevice, "/device/a.2/b.1") != nullptr);
    REQUIRE(get_node(localDevice, "/device/a.2/b.1/c") != nullptr);
    REQUIRE(get_node(localDevice, "/device/a.2/b.1/d") != nullptr);
  }

  SECTION("Value of nodes") {
    REQUIRE(get_node(localDevice, "/device")->children().size() == 2);
    REQUIRE(get_node(localDevice, "/device/a.1")->children().size() == 1);
    REQUIRE(get_node(localDevice, "/device/a.1/b")->children().size() == 2);
    REQUIRE(get_node(localDevice, "/device/a.1/b/c.1")->getAddress()->cloneValue().get<int32_t>() == 1);
    REQUIRE(get_node(localDevice, "/device/a.1/b/c.2")->getAddress()->cloneValue().get<int32_t>() == 2);
    REQUIRE(get_node(localDevice, "/device/a.2/b.1")->children().size() == 2);
    REQUIRE(get_node(localDevice, "/device/a.2/b.1/c")->getAddress()->cloneValue().get<int32_t>() == 3);
    REQUIRE(get_node(localDevice, "/device/a.2/b.1/d")->getAddress()->cloneValue().get<int32_t>() == 4);
  }
}

TEST_CASE ("Functions on instances") {
  ossia::net::generic_device localDevice{
    std::make_unique<ossia::net::local_protocol>(), "device"};

  ossia::presets::instance_functions funcs;
  using namespace ossia::regex_path;
  int number_of_a_nodes = 0;
  int number_of_a_nodes_regex = 0;
  int number_of_b_nodes = 0;
  int number_of_d_nodes = 0;

  funcs.emplace_back((device("device") / any_instance("a")).regex(),
                     [&] (const ossia::net::node_base& node) {
    std::cerr << "Case 1: "
              << ossia::net::address_string_from_node(node)
              << " was inserted" << std::endl;

    number_of_a_nodes++;
  });

  funcs.emplace_back(std::regex(R"(device:/a\.?[0-9]*$)"),
                     [&] (const ossia::net::node_base& node) {
    std::cerr << "Case 3: "
              << ossia::net::address_string_from_node(node)
              << " was inserted" << std::endl;

    number_of_a_nodes_regex++;
  });

  funcs.emplace_back((device("device") / any_instance("a") / any_instance("b") / stop()).regex(),
                     [&] (const ossia::net::node_base& node) {
    std::cerr << "Case 2: "
              << ossia::net::address_string_from_node(node)
              << " was inserted" << std::endl;

    number_of_b_nodes++;
  });

  funcs.emplace_back((device("device") / "a.2" / "b.1" / "d").regex(),
                     [&] (const ossia::net::node_base& node) {
    std::cerr << "Case 3: "
              << ossia::net::address_string_from_node(node)
              << " was inserted" << std::endl;

    number_of_d_nodes++;
  });

  ossia::presets::preset p;
  p.emplace("/a.0/b/c.0", 1);
  p.emplace("/a.0/b/c.1", 2);
  p.emplace("/a.1/b.0/c", 3);
  p.emplace("/a.1/b.0/d", 4);

  REQUIRE_NOTHROW(ossia::devices::apply_preset(localDevice, p, ossia::devices::keep_arch_off, funcs));

  REQUIRE(number_of_a_nodes == 2);
  REQUIRE(number_of_a_nodes_regex == 2);
  REQUIRE(number_of_b_nodes == 2);
  REQUIRE(number_of_d_nodes == 1);
}
