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
    ossia::net::set_app_name(device.getRootNode(), "json_export_test_app"s);
    ossia::net::set_app_version(device.getRootNode(), "0.1"s);
    ossia::net::set_app_creator(device.getRootNode(), "toto"s);

    auto localTestNode = device.createChild("parameter");

    auto localImpulseNode = localTestNode->createChild("my_impulse");
    auto localImpulseAddress = localImpulseNode->createAddress(val_type::IMPULSE);

    auto localBoolNode = localTestNode->createChild("my_bool");
    auto localBoolAddress = localBoolNode->createAddress(val_type::BOOL);
    localBoolAddress->setDefaultValue(true);
    ossia::net::set_description(*localBoolNode, "test description"s);

    auto localIntNode = localTestNode->createChild("my_int");
    auto localIntAddress = localIntNode->createAddress(val_type::INT);
    localIntAddress->setDefaultValue(3);
    localIntAddress->setDomain(make_domain(0, 10));
    ossia::net::set_value_step_size(*localIntNode, 1);

    auto localFloatNode = localTestNode->createChild("my_float");
    auto localFloatAddress = localFloatNode->createAddress(val_type::FLOAT);
    localFloatAddress->setDefaultValue(0.5);
    localFloatAddress->setDomain(make_domain(0.0, 1.0));
    ossia::net::set_value_step_size(*localFloatNode, 0.1);

    auto localStringNode = localTestNode->createChild("my_string");
    auto localStringAddress = localStringNode->createAddress(val_type::STRING);
    localStringAddress->setDefaultValue(std::string("test"));

    // update tree value
    localImpulseAddress->pushValue(impulse{});
    localBoolAddress->pushValue(true);
    localIntAddress->pushValue(123);
    localFloatAddress->pushValue(0.5);
    localStringAddress->pushValue("hello world !"s);

    // display json tree in console
    auto json = ossia::devices::write_json(device);
    std::cerr << json << "\n";

    // write it as a file
    ossia::devices::write_file(json, "namespace.json");

}
