#include <iostream>
#include <memory>
#include <functional>

#include <ossia/ossia.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia-c/preset/preset.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>

using namespace ossia;
using namespace ossia::net;
using namespace std;

rapidjson::Value explore(const ossia::net::node_base& node, rapidjson::Document& d);
std::string print_namespace(const node_base& deviceNode);

int main()
{
    auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
    // declare this program "B" as Local device
    ossia::net::generic_device device{std::move(local_proto_ptr), "json_export_test_app"};

    /* publish each feature of program "B" as address into a tree
       /parameter
       /parameter/my_bang
       /parameter/my_bool
       /parameter/my_int
       /parameter/my_float
       /parameter/my_string
       /parameter/my_destination
       /parameter/my_tuple
       */

    // add app extended information attributes
    device.getRootNode().set(app_name_attribute{}, "json_export_test_app"s);
    device.getRootNode().set(app_version_attribute{}, "0.1"s);
    device.getRootNode().set(app_creator_attribute{}, "toto"s);

    auto TestNode = device.createChild("parameter");

    auto ImpulseNode = TestNode->createChild("my_impulse");
    auto ImpulseAddress = ImpulseNode->createAddress(val_type::IMPULSE);

    auto BoolNode = TestNode->createChild("my_bool");
    auto BoolAddress = BoolNode->createAddress(val_type::BOOL);
    BoolNode->set(default_value_attribute{}, true);
    BoolNode->set(description_attribute{}, "test description"s);

    auto IntNode = TestNode->createChild("my_int");
    auto IntAddress = IntNode->createAddress(val_type::INT);
    IntNode->set(default_value_attribute{}, 3);
    IntNode->set(domain_attribute{}, make_domain(0, 10));
    IntNode->set(value_step_size_attribute{}, 1);

    auto FloatNode = TestNode->createChild("my_float");
    auto FloatAddress = FloatNode->createAddress(val_type::FLOAT);
    FloatNode->set(default_value_attribute{}, 0.5);
    FloatNode->set(domain_attribute{}, make_domain(0.0, 1.0));
    FloatNode->set(value_step_size_attribute{}, 0.1);


    auto StringNode1 = TestNode->createChild("my_string");
    auto StringAddress1 = StringNode1->createAddress(val_type::STRING);
    StringNode1->set(default_value_attribute{}, "test"s);


    auto StringNode2 = TestNode->createChild("my_string");
    auto StringAddress2 = StringNode2->createAddress(val_type::STRING);
    StringNode2->set(default_value_attribute{}, "test"s);


    // update tree value
    ImpulseAddress->pushValue(impulse{});
    BoolAddress->pushValue(true);
    IntAddress->pushValue(123);
    FloatAddress->pushValue(0.5);
    StringAddress1->pushValue("hello world !"s);
    StringAddress2->pushValue("goodbye world !"s);

    // display json tree in console
    std::cerr << "== DEVICE ==\n";
    auto json = ossia::devices::write_json(device);
    std::cerr << json << "\n";

    // write it as a file
    ossia::devices::write_file(json, "namespace.json");

    // Preset
    std::cerr << "\n\n== PRESET ==\n";
    auto preset = ossia::devices::make_preset(device);
    for(auto& p : preset)
    {
      std::cerr << p.first << " => " << p.second << "\n";

      // Modify it a bit
      if(p.first == "/parameter/my_int")
        p.second = ossia::value{456};
      if(p.first == "/parameter/my_string.0")
        p.second = "foo";
      if(p.first == "/parameter/my_string.1")
        p.second = "bar";
    }

    // Save the preset to disk
    std::cerr << "\n\n== PRESET JSON ==\n";
    auto json_preset = ossia::presets::write_json(preset);
    std::cerr << json_preset << "\n";

    // Reapply the preset
    auto new_preset = ossia::presets::read_json(json_preset);
    ossia::devices::apply_preset(device, new_preset);

    std::cerr << IntAddress->cloneValue() << "\n";
    std::cerr << StringAddress1->cloneValue() << "\n";
    std::cerr << StringAddress2->cloneValue() << "\n";

}
