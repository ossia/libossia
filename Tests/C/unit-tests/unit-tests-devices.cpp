#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <iostream>

#include <preset/preset.h>
#include <preset/preset.hpp>

#include "Network/Protocol.h"
#include "Network/Protocol/Local.h"
#include "Network/Node.h"
#include "Network/Device.h"
#include "Editor/Value.h"

TEST_CASE ("Device exceptions") {
    auto localProtocol = OSSIA::Local::create();
    auto localDevice = OSSIA::Device::create(localProtocol, "device");

    auto rootnode = *(localDevice->emplace(localDevice->children().cend(), "root"));
    auto childnode = *(rootnode->emplace(rootnode->children().cend(), "child"));
    auto childaddr = childnode->createAddress(OSSIA::Value::Type::BOOL);

    SECTION("Invalid address: target node has children") {
        ossia::presets::Preset preset;
        OSSIA::Bool b;
        preset.insert(ossia::presets::PresetPair ("/root", &b));

        REQUIRE_THROWS(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_off));
    }

    SECTION("Invalid address: keep_arch_type") {
        ossia::presets::Preset preset;
        OSSIA::Bool b;
        preset.insert(ossia::presets::PresetPair ("/root/child_that_does_not_exist", &b));

        REQUIRE_THROWS(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_on));
        REQUIRE_NOTHROW(ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_off));

    }
}

TEST_CASE ("Building device from preset") {
    auto localProtocol = OSSIA::Local::create();
    auto localDevice = OSSIA::Device::create(localProtocol, "device");

    ossia::presets::Preset p;
    OSSIA::Int i1 (1);
    OSSIA::Int i2 (2);
    OSSIA::Int i3 (3);
    OSSIA::Int i4 (4);
    p.insert(ossia::presets::PresetPair ("/a.0/b/c.0", &i1));
    p.insert(ossia::presets::PresetPair ("/a.0/b/c.1", &i2));
    p.insert(ossia::presets::PresetPair ("/a.1/b.0/c", &i3));
    p.insert(ossia::presets::PresetPair ("/a.1/b.0/d", &i4));

    REQUIRE_NOTHROW(ossia::devices::apply_preset(localDevice, p, ossia::devices::keep_arch_off));

    auto children = localDevice->children();

    SECTION("Presence of nodes") {
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.1") != nullptr);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.1/b") != nullptr);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.1/b/c.1") != nullptr);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.1/b/c.2") != nullptr);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.2/b.1") != nullptr);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.2/b.1/c") != nullptr);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.2/b.1/d") != nullptr);
    }

    SECTION("Value of nodes") {
        REQUIRE(ossia::devices::get_node(localDevice, "/device")->children().size() == 2);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.1")->children().size() == 1);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.1/b")->children().size() == 2);
        REQUIRE(((OSSIA::Int*)(ossia::devices::get_node(localDevice, "/device/a.1/b/c.1")->getAddress()->getValue()))->value == i1.value);
        REQUIRE(((OSSIA::Int*)(ossia::devices::get_node(localDevice, "/device/a.1/b/c.2")->getAddress()->getValue()))->value == i2.value);
        REQUIRE(ossia::devices::get_node(localDevice, "/device/a.2/b.1")->children().size() == 2);
        REQUIRE(((OSSIA::Int*)(ossia::devices::get_node(localDevice, "/device/a.2/b.1/c")->getAddress()->getValue()))->value == i3.value);
        REQUIRE(((OSSIA::Int*)(ossia::devices::get_node(localDevice, "/device/a.2/b.1/d")->getAddress()->getValue()))->value == i4.value);
    }
}
