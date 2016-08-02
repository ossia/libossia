#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <preset/result.cs>
#include <preset/preset.h>
#include <ossia/ossia_utils.hpp>

#include "Network/Protocol.h"
#include "Network/Protocol/Local.h"
#include "Network/Node.h"
#include "Network/Device.h"
#include "Editor/Value.h"



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

    auto lp = OSSIA::Local::create();
    auto dev = OSSIA::Device::create(lp, "dev");

    ossia_device_t o = new ossia_device ();
    o->device = dev;

    code = ossia_devices_apply_preset(o, p, false);
    REQUIRE(code == OSSIA_PRESETS_OK);

    code = ossia_presets_free(p);
    REQUIRE(code == OSSIA_PRESETS_OK);
}
