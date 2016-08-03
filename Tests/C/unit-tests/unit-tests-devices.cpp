#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <iostream>

#include <ossia-c/preset/preset.h>
#include <ossia-c/preset/preset.hpp>

#include <ossia/ossia.hpp>

TEST_CASE ("Device exceptions") {
    ossia::net::generic_device localDevice{
      std::make_unique<ossia::net::local_protocol>(), "test"};

    auto rootnode = localDevice.getRootNode().createChild("root");
    auto childnode = rootnode->createChild("child");
    auto childaddr = childnode->createAddress(ossia::val_type::BOOL);

    SECTION("Invalid address: target node has children") {
        ossia::presets::Preset preset;
        ossia::Bool b;
        preset.insert(std::make_pair("/root", b));

        REQUIRE_THROWS(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_off));
    }

    SECTION("Invalid address: keep_arch_type") {
        ossia::presets::Preset preset;
        ossia::Bool b;
        preset.insert(std::make_pair ("/root/child_that_does_not_exist", b));

        REQUIRE_THROWS(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_on));
        REQUIRE_NOTHROW(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_off));

    }
}

TEST_CASE ("Building device from preset") {
    ossia::net::generic_device localDevice{
    std::make_unique<ossia::net::local_protocol>(), "device"};

    ossia::presets::Preset p;
    ossia::Int i1 (1);
    ossia::Int i2 (2);
    ossia::Int i3 (3);
    ossia::Int i4 (4);
    p.insert(std::make_pair ("/a.0/b/c.0", i1));
    p.insert(std::make_pair ("/a.0/b/c.1", i2));
    p.insert(std::make_pair ("/a.1/b.0/c", i3));
    p.insert(std::make_pair ("/a.1/b.0/d", i4));

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
        REQUIRE(get_node(localDevice, "/device/a.1/b/c.1")->getAddress()->cloneValue().get<ossia::Int>().value == i1.value);
        REQUIRE(get_node(localDevice, "/device/a.1/b/c.2")->getAddress()->cloneValue().get<ossia::Int>().value == i2.value);
        REQUIRE(get_node(localDevice, "/device/a.2/b.1")->children().size() == 2);
        REQUIRE(get_node(localDevice, "/device/a.2/b.1/c")->getAddress()->cloneValue().get<ossia::Int>().value == i3.value);
        REQUIRE(get_node(localDevice, "/device/a.2/b.1/d")->getAddress()->cloneValue().get<ossia::Int>().value == i4.value);
    }
}
