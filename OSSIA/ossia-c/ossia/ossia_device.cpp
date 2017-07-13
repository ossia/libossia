#include "ossia_utils.hpp"
#include <iostream>
#include <map>

static std::map<std::string, ossia_device_t>& static_devices()
{
  static std::map<std::string, ossia_device_t> devs;
  return devs;
}

extern "C" {
ossia_device_t ossia_device_create(ossia_protocol_t protocol, const char* name)
{
  return safe_function(__func__, [=]() -> ossia_device_t {
    std::string str_name(name);

    // Look in our cache
    auto& devs = static_devices();
    auto it = devs.find(str_name);
    if (it != devs.end())
    {
      // Found the device in the cache
      if (!it->second)
      {
        ossia_log_error("ossia_device_create: it->second is null");
        return nullptr;
      }
      return it->second;
    }
    else
    {
      if (!protocol)
      {
        ossia_log_error("ossia_device_create: protocol is null");
        return nullptr;
      }
      // Create a device and add it to the cache.
      auto dev = new ossia_device{std::make_unique<ossia::net::generic_device>(
          std::unique_ptr<ossia::net::protocol_base>(protocol->protocol),
          str_name)};

      devs.insert(std::make_pair(str_name, dev));

      return dev;
    }

  });
}

const char* ossia_device_get_name(ossia_device_t node)
{
  return safe_function(__func__, [=]() -> const char* {
    if (!node)
    {
      ossia_log_error("ossia_device_get_name: node is null");
      return nullptr;
    }
    if (!node->device)
    {
      ossia_log_error("ossia_device_get_name: node->device");
      return nullptr;
    }

    return strdup(node->device->get_name().c_str());
  });
}

void ossia_device_free(ossia_device_t device)
{
  return safe_function(__func__, [=] {
    if (device && device->device)
    {
      auto& devs = static_devices();
      auto it = devs.find(device->device->get_name());
      if (it != devs.end())
      {
        devs.erase(it);
      }
    }

    delete device;
  });
}

void ossia_device_reset_static()
{
  return safe_function(__func__, [=] {
    auto& devs = static_devices();
    for (auto& dev : devs)
    {
      delete dev.second;
    }
    devs.clear();
  });
}

bool ossia_device_update_namespace(ossia_device_t device)
{
  return safe_function(__func__, [=] {
    if (device)
    {
      assert(device->device);
      return device->device->get_protocol().update(
          device->device->get_root_node());
    }
    else
    {
      return false;
    }
  });
}

ossia_node_t ossia_device_get_root_node(ossia_device_t device)
{
  return safe_function(__func__, [=]() -> ossia_node_t {
    if (!device)
      return {};

    assert(device->device);
    return convert(&device->device->get_root_node());
  });
}
}
