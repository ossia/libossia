// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <iostream>
#include <memory>
#include <functional>


using namespace ossia;
using namespace ossia::net;
using namespace std;

rapidjson::Value explore(const ossia::net::node_base& node, rapidjson::Document& d);
std::string print_namespace(const node_base& deviceNode);

int main()
{
    // This program is named "A" and mirrors a remote device named "B".
    ossia::net::generic_device device{
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(), "B"
    };

    // explore the tree of B
    device.get_protocol().update(device);

    // display json tree in console
    std::cerr << print_namespace(device.get_root_node()) << "\n";
}

rapidjson::Value explore(
        const ossia::net::node_base& node,
        rapidjson::Document& d)
{
    rapidjson::Value v;
    v.SetObject();

    auto address = node.get_parameter();

    if(address)
    {
        switch (address->get_value_type())
        {
            case val_type::IMPULSE :
            {
                v.AddMember("valueType", "impulse", d.GetAllocator());
                break;
            }
            case val_type::BOOL :
            {
                v.AddMember("valueType", "boolean", d.GetAllocator());
                v.AddMember("value", address->value().get<bool>(), d.GetAllocator());
                break;
            }
            case val_type::INT :
            {
                v.AddMember("valueType", "integer", d.GetAllocator());
                v.AddMember("value", address->value().get<int>(), d.GetAllocator());
                break;
            }
            case val_type::FLOAT :
            {
                v.AddMember("valueType", "decimal", d.GetAllocator());
                v.AddMember("value", address->value().get<float>(), d.GetAllocator());
                break;
            }
            case val_type::CHAR :
            {
                v.AddMember("valueType", "char", d.GetAllocator());
                v.AddMember("value", address->value().get<char>(), d.GetAllocator());
                break;
            }
            case val_type::STRING :
            {
                v.AddMember("valueType", "string", d.GetAllocator());

                auto val = address->value().get<std::string>();
                rapidjson::Value s;
                s.SetString(val.c_str(), val.size(), d.GetAllocator());
                v.AddMember("value", s, d.GetAllocator());
                break;
            }
                // TODO Tuple, etc...
            default:
                break;
        }
    }

    for (const auto& child : node.children())
    {
        rapidjson::Value s;
        s.SetString(child->get_name().c_str(), child->get_name().size(), d.GetAllocator());
        v.AddMember(s, explore(*child, d), d.GetAllocator());
    }

    return v;
}

std::string print_namespace(const node_base& deviceNode)
{
    rapidjson::Document d;
    d.SetObject();
    d.AddMember("root", explore(deviceNode, d), d.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    const char* output = buffer.GetString();
    return output;
}
