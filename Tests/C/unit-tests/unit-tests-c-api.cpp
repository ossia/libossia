#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <ossia-c/preset/result.h>
#include <ossia-c/preset/preset.h>
#include <ossia-c/ossia-c.h>
#include <ossia-c/ossia/ossia_utils.hpp>

#include <ossia/ossia.hpp>

TEST_CASE ("C API: Read JSON") {
    const char* json = R"_(
                       {"scene":
                         {"cubes":
                           [
                             {"scale": {"x":1, "y":1, "z":0.5},
                              "position": {"x":2, "y":0.5, "z":0},
                              "color": {"r":192, "g":255, "b":225, "alpha":255}
                             }
                           ]
                         },
                         "name": "Une scène"
                       }
                       )_";
    ossia_preset_t p;

    REQUIRE(ossia_presets_read_json(json, &p) == OSSIA_PRESETS_OK);

    int size;
    REQUIRE(ossia_presets_size(p, &size) == OSSIA_PRESETS_OK);
    REQUIRE(size == 11);

    ossia_presets_free(p);
}

TEST_CASE ("C API: Apply preset") {

    const char* json = R"_(
                       {"scene":
                         {"cubes":
                           [
                             {"scale": {"x":1, "y":1, "z":0.5},
                              "position": {"x":2, "y":0.5, "z":0},
                              "color": {"r":192, "g":255, "b":225, "alpha":255}
                             }
                           ]
                         },
                         "name": "Une scène"
                       }
                       )_";
    ossia_preset_t p;
    ossia_preset_result code;

    code = ossia_presets_read_json(json, &p);

    ossia_device dev_c{
      std::make_unique<ossia::net::generic_device>(
            std::make_unique<ossia::net::multiplex_protocol>(),
            "test")};

    code = ossia_devices_apply_preset(&dev_c, p, false);
    REQUIRE(code == OSSIA_PRESETS_OK);

    code = ossia_presets_free(p);
    REQUIRE(code == OSSIA_PRESETS_OK);
}
