// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/generic/generic_device.hpp>

#include <ossia-c/ossia-c.h>
#include <ossia-c/ossia/ossia_utils.hpp>
#include <ossia-c/preset/preset.h>
#include <ossia-c/preset/result.h>

#include "include_catch.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

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

TEST_CASE("C API: a device is freed while its network threads apply values", "[network]")
{
  // The server applies what the mirror writes on its own threads, the mirror
  // what the server sends on its: each is stopped before its nodes go
  // (a data race, then a use after free, under the sanitizers before)
  for(int run = 0; run < 5; run++)
  {
    auto server = ossia_device_create(
        ossia_protocol_oscquery_server_create(1745, 5745), "c_free_server");
    REQUIRE(server);
    auto level = ossia_node_create_parameter(
        ossia_node_create(ossia_device_get_root_node(server), "/level"), FLOAT_T);

    auto mirror = ossia_device_create(
        ossia_protocol_oscquery_mirror_create("ws://127.0.0.1:5745"), "c_free_mirror");
    REQUIRE(mirror);
    ossia_device_update_namespace(mirror);
    auto node = ossia_node_find(ossia_device_get_root_node(mirror), "/level");
    REQUIRE(node);
    auto remote = ossia_node_get_parameter(node);
    REQUIRE(remote);

    // The server writes too, so that the mirror applies values, while the
    // mirror is freed first; the mirror alone, while the server is
    const bool mirror_first = run % 2 == 0;
    std::atomic_bool done{false};
    std::thread writer{[&] {
      for(int i = 0; !done; i++)
      {
        ossia_parameter_push_f(mirror_first ? level : remote, float(i));
        std::this_thread::sleep_for(std::chrono::microseconds(200));
      }
    }};
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(mirror_first)
    {
      ossia_device_free(mirror);
      done = true;
      writer.join();
      ossia_device_free(server);
    }
    else
    {
      ossia_device_free(server);
      done = true;
      writer.join();
      ossia_device_free(mirror);
    }
  }
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
