// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <ossia-c/ossia-c.h>
#include <ossia-c/ossia/ossia_utils.hpp>
#include <ossia-c/preset/preset.h>
#include <ossia-c/preset/result.h>

#include "include_catch.hpp"

#include <chrono>
#include <iostream>

TEST_CASE("C API: Read JSON", "[read]")
{
  const char* json = R"_(
                     {"scene":
                     {"cubes":
                     [
                     {"scale": {"x":1, "y":1, "z":0.5},
                     "position": {"x":2, "y":0.5, "z":0},
                     "color": {"r":192, "g":255, "b":225, "alpha":255}
                     }
                     ],
                     "name": "Une scène"
                     }
                     }
                     )_";
  ossia_preset_t p;

  REQUIRE(ossia_presets_read_json(json, &p) == OSSIA_PRESETS_OK);

  int size;
  REQUIRE(ossia_presets_size(p, &size) == OSSIA_PRESETS_OK);
  REQUIRE(size == 11);

  ossia_presets_free(p);
}

TEST_CASE("C API: Apply preset", "[apply]")
{

  const char* json = R"_(
                     {"scene":
                     {"cubes":
                     [
                     {"scale": {"x":1, "y":1, "z":0.5},
                     "position": {"x":2, "y":0.5, "z":0},
                     "color": {"r":192, "g":255, "b":225, "alpha":255}
                     }
                     ],
                     "name": "Une scène"
                     }
                     }
                     )_";
  ossia_preset_t p;
  ossia_preset_result code;

  code = ossia_presets_read_json(json, &p);

  ossia_device dev_c{std::make_unique<ossia::net::generic_device>("scene")};

  code = ossia_devices_apply_preset(&dev_c, p, false);
  REQUIRE(code == OSSIA_PRESETS_OK);

  code = ossia_presets_free(p);
  REQUIRE(code == OSSIA_PRESETS_OK);
}

TEST_CASE("C API: create pattern", "[pattern]")
{
  auto proto = ossia_protocol_multiplex_create();
  auto dev = ossia_device_create(proto, "foo");
  ossia_node_t* n{};
  size_t sz;
  ossia_node_create_pattern(
      ossia_device_get_root_node(dev), "/{foo,bar}/baz[1-5]", &n, &sz);
  ossia_node_t* n2{};
  size_t sz2;
  ossia_node_find_pattern(
      ossia_device_get_root_node(dev), "/{foo,bar}/baz[1-5]", &n2, &sz2);
  REQUIRE(n != nullptr);
  REQUIRE(n2 != nullptr);
  REQUIRE(sz == 10);
  REQUIRE(sz == sz2);
  ossia_node_array_free(n);
  ossia_node_array_free(n2);
  ossia_device_free(dev);
  ossia_protocol_free(proto);
}

TEST_CASE("C API: list the OSCQuery devices announced over mDNS", "[zeroconf]")
{
  if(!ossia::net::zeroconf_supported())
    return;

  const std::string name
      = "ossia_test_c_list_"
        + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count() % 1000000);
  auto dev = ossia_device_create(
      ossia_protocol_oscquery_server_create(1244, 5694), name.c_str());
  REQUIRE(dev);

  char** names{};
  char** hosts{};
  int* ports{};
  size_t size{};
  ossia_oscquery_list_devices(1500, &names, &hosts, &ports, &size);
  bool found = false;
  for(size_t i = 0; i < size; i++)
  {
    REQUIRE(names[i]);
    REQUIRE(hosts[i]);
    if(names[i] == name)
    {
      found = true;
      REQUIRE(ports[i] == 5694);
    }
  }
  ossia_oscquery_devices_free(names, hosts, ports, size);
  ossia_device_free(dev);
  if(!found)
    SKIP("this machine does not see its own mDNS announcements");
}

// The C API gives its strings to be freed with ossia_string_free, which calls
// free(), and those of the preset API with ossia_free_string. Under
// AddressSanitizer with alloc_dealloc_mismatch=1, a string allocated with
// new[] fails here.
TEST_CASE("C API: free the strings it returns", "[strings]")
{
  ossia_value_t v = ossia_value_create_string("foo");
  const char* str = ossia_value_to_string(v);
  REQUIRE(std::string(str) == "foo");
  ossia_string_free((char*)str);
  ossia_value_free(v);

  ossia_preset_t p;
  REQUIRE(ossia_presets_read_json(R"_({"a": {"b": 1}})_", &p) == OSSIA_PRESETS_OK);
  const char* buffer = nullptr;
  REQUIRE(ossia_presets_to_string(p, &buffer) == OSSIA_PRESETS_OK);
  REQUIRE(buffer);
  ossia_free_string(buffer);
  ossia_presets_free(p);
}
