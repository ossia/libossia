#include <iostream>
#include <memory>
#include <functional>
#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol/Local.h"
#include "Network/Protocol/Minuit.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>

rapidjson::Value explore(
        const std::shared_ptr<OSSIA::Node> node,
        rapidjson::Document& d)
{
    rapidjson::Value v;
    v.SetObject();

    auto address = node->getAddress();
    if(address)
    {
        switch (address->getValueType())
        {
            case OSSIA::Value::Type::IMPULSE :
            {
                v.AddMember("valueType", "impulse", d.GetAllocator());
                break;
            }
            case OSSIA::Value::Type::BOOL :
            {
                v.AddMember("valueType", "boolean", d.GetAllocator());
                v.AddMember("value", ((OSSIA::Bool*)address->getValue())->value, d.GetAllocator());
                break;
            }
            case OSSIA::Value::Type::INT :
            {
                v.AddMember("valueType", "integer", d.GetAllocator());
                v.AddMember("value", ((OSSIA::Int*)address->getValue())->value, d.GetAllocator());
                break;
            }
            case OSSIA::Value::Type::FLOAT :
            {
                v.AddMember("valueType", "decimal", d.GetAllocator());
                v.AddMember("value", ((OSSIA::Float*)address->getValue())->value, d.GetAllocator());
                break;
            }
            case OSSIA::Value::Type::CHAR :
            {
                v.AddMember("valueType", "char", d.GetAllocator());
                v.AddMember("value", ((OSSIA::Char*)address->getValue())->value, d.GetAllocator());
                break;
            }
            case OSSIA::Value::Type::STRING :
            {
                v.AddMember("valueType", "string", d.GetAllocator());

                auto val = ((OSSIA::String*)address->getValue())->value;
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

    for (const auto& child : node->children())
    {
        rapidjson::Value s;
        s.SetString(child->getName().c_str(), child->getName().size(), d.GetAllocator());
        v.AddMember(s, explore(child, d), d.GetAllocator());
    }

    return v;
}

std::string print_namespace(std::shared_ptr<OSSIA::Node> device)
{
    rapidjson::Document d;
    d.SetObject();
    d.AddMember("root", explore(device, d), d.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    const char* output = buffer.GetString();
    return output;
}

int main()
{
    // declare this program "A" as Local device
    auto localProtocol = OSSIA::Local::create();
    auto localDevice = OSSIA::Device::create(localProtocol, "A");

    // declare a distant program "B" as a Minuit device
    auto minuitProtocol = OSSIA::Minuit::create("127.0.0.1", 6666, 9999);
    auto minuitDevice = OSSIA::Device::create(minuitProtocol, "B");
    {
        // explore the tree of B
        minuitDevice->updateNamespace();

        // display tree in console
        std::cerr << print_namespace(minuitDevice) << "\n";
    }
}

