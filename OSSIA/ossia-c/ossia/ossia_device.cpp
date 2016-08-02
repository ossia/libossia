#include "ossia_utils.hpp"
#include <iostream>
#include <map>


static std::map<std::string, ossia_device_t>& static_devices()
{
    static std::map<std::string, ossia_device_t> devs;
    return devs;
}

extern "C"
{
ossia_device_t ossia_device_create(
        ossia_protocol_t protocol,
        const char* name)
{
    return safe_function(__func__, [=] {
        std::string str_name(name);

        // Look in our cache
        auto& devs = static_devices();
        auto it = devs.find(str_name);
        if(it != devs.end())
        {
            // Found the device in the cache
            return it->second;
        }
        else
        {
            // Create a device and add it to the cache.
            auto dev = new ossia_device{ossia::Device::create(protocol->protocol, str_name)};

            //TODO Free protocol
            devs.insert(std::make_pair(str_name, dev));

            return dev;
        }

    });
}

const char* ossia_device_get_name(ossia_device_t node)
{
    return safe_function(__func__, [=] () -> const char* {
        if(!node)
            return nullptr;

        if(!node->device)
            return nullptr;

        return strdup(node->device->getName().c_str());
    });
}

void ossia_device_free(ossia_device_t device)
{
    return safe_function(__func__, [=] {
        if(device && device->device)
        {
            auto& devs = static_devices();
            auto it = devs.find(device->device->getName());
            if(it != devs.end())
            {
                devs.erase(it);
            }
        }

        delete device;
    });
}

bool ossia_device_update_namespace(ossia_device_t device)
{
    return safe_function(__func__, [=] {
        if(device)
        {
            return device->device->updateNamespace();
        }
        else
        {
            return false;
        }
    });
}


ossia_node_t ossia_device_add_child(
        ossia_device_t device,
        const char* name)
{
    return safe_function(__func__, [=] () -> ossia_node_t {
        if(!device)
            return nullptr;

        auto it = device->device->emplace(device->device->children().end(), name);
        auto child_node = *it;

        return new ossia_node{child_node};
    });
}

void ossia_device_remove_child(
        ossia_device_t device,
        ossia_node_t child)
{
    return safe_function(__func__, [=] {
        if(!device)
            return;
        if(!child)
            return;

        auto& cld = device->device->children();
        std::string node_name = child->node->getName();
        auto it = std::find_if(cld.begin(), cld.end(),
                               [&] (const auto& node) {
            return node->getName() == node_name;
        });

        if(it != cld.end())
        {
            device->device->erase(it);
        }
        delete child;
    });
}

int ossia_device_child_size(
        ossia_device_t device)
{
    if(!device)
        return {};

    return device->device->children().size();
}

ossia_node_t ossia_device_get_child(
        ossia_device_t device,
        int child_n)
{
    return safe_function(__func__, [=] () -> ossia_node_t {
        if(!device)
            return {};

        if(device->device->children().size() < child_n)
            return nullptr;

        return new ossia_node{device->device->children()[child_n]};
    });
}
}
