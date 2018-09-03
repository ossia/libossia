// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>
#include <ossia-c/preset/result.h>
#include <ossia-c/preset/preset.h>
#include <ossia-c/ossia-c.h>
#include <ossia-c/ossia/ossia_utils.hpp>

#include <ossia/ossia.hpp>

TEST_CASE ("C API: Read JSON", "[read]") {
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

TEST_CASE ("C API: Apply preset", "[apply]") {

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

  ossia_device dev_c{
    std::make_unique<ossia::net::generic_device>(
          "scene")};

  code = ossia_devices_apply_preset(&dev_c, p, false);
  REQUIRE(code == OSSIA_PRESETS_OK);

  code = ossia_presets_free(p);
  REQUIRE(code == OSSIA_PRESETS_OK);
}


TEST_CASE ("C API: create pattern", "[pattern]") {
  auto proto = ossia_protocol_multiplex_create();
  auto dev = ossia_device_create(proto, "foo");
  ossia_node_t* n{};
  size_t sz;
  ossia_node_create_pattern(ossia_device_get_root_node(dev), "/{foo,bar}/baz[1-5]", &n, &sz);
  ossia_node_t* n2{};
  size_t sz2;
  ossia_node_find_pattern(ossia_device_get_root_node(dev), "/{foo,bar}/baz[1-5]", &n2, &sz2);
  REQUIRE(n != nullptr);
  REQUIRE(n2 != nullptr);
  REQUIRE(sz == 10);
  REQUIRE(sz == sz2);
  ossia_node_array_free(n);
  ossia_node_array_free(n2);
  ossia_device_free(dev);
  ossia_protocol_free(proto);
}
