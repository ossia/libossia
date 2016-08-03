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
            auto dev = new ossia_device{
                       std::make_unique<ossia::net::generic_device>(
                         std::move(protocol->protocol),
                         str_name)};

            devs.insert(std::make_pair(str_name, dev));

            delete protocol;
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
            return device->device->getProtocol().update(device->device->getRootNode());
        }
        else
        {
            return false;
        }
    });
}


ossia_node_t ossia_device_get_root_node(
        ossia_device_t device)
{
    return safe_function(__func__, [=] () -> ossia_node_t {
        if(!device)
            return {};

        return convert(&device->device->getRootNode());
    });
}
}
