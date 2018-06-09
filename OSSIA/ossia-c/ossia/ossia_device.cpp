// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include "ossia_utils.hpp"
#include <iostream>
#include <map>
#include <ossia/detail/flat_map.hpp>
#include <ossia/network/generic/generic_device.hpp>

global_devices& static_devices()
{
  static global_devices devs;
  return devs;
}

extern "C" {
ossia_device_t ossia_device_create(ossia_protocol_t protocol, const char* name)
{
  return safe_function(__func__, [=]() -> ossia_device_t {
    std::string str_name(name);

    // Look in our cache
    auto& devs = static_devices();
    auto it = devs.devices.find(str_name);
    if (it != devs.devices.end())
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

      devs.devices.insert(std::make_pair(str_name, dev));

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
      auto it = devs.devices.find(device->device->get_name());
      if (it != devs.devices.end())
      {
        device->device->get_root_node().clear_children();
        devs.devices.erase(it);
      }
    }

    delete device;
  });
}

void ossia_device_reset_static()
{
  return safe_function(__func__, [=] {
    auto& devs = static_devices();
    for (auto& dev : devs.devices)
    {
      delete dev.second;
    }
    devs.devices.clear();
  });
}

int ossia_device_update_namespace(ossia_device_t device)
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


ossia_node_callback_idx_t ossia_device_add_node_created_callback(
        ossia_device_t device,
        ossia_node_callback_t callback,
        void* ctx)
{
  return safe_function(__func__, [=]() -> ossia_node_callback_idx_t {
    if (!device)
    {
      ossia_log_error("ossia_device_add_node_created_callback: device is null");
      return nullptr;
    }
    if (!callback)
    {
      ossia_log_error("ossia_device_add_node_created_callback: callback is null");
      return nullptr;
    }

    auto the_cb = new node_cb{callback, ctx};

    convert_device(device)->on_node_created.connect<node_cb>(the_cb);
    return reinterpret_cast<ossia_node_callback_idx_t>(the_cb);
  });
}

void ossia_device_remove_node_created_callback(
        ossia_device_t device,
        ossia_node_callback_idx_t index)
{
  return safe_function(__func__, [=] {
    auto idx = (node_cb*) index;
    if (!device)
    {
      ossia_log_error("ossia_device_remove_node_created_callback: device is null");
      return;
    }
    if (!idx)
    {
      ossia_log_error("ossia_device_remove_node_created_callback: index is null");
      return;
    }

    convert_device(device)->on_node_created.disconnect<node_cb>(idx);
    delete idx;
  });
}


ossia_node_callback_idx_t ossia_device_add_node_removing_callback(
        ossia_device_t device,
        ossia_node_callback_t callback,
        void* ctx)
{
    return safe_function(__func__, [=]() -> ossia_node_callback_idx_t {
      if (!device)
      {
        ossia_log_error("ossia_device_add_node_removing_callback: device is null");
        return nullptr;
      }
      if (!callback)
      {
        ossia_log_error("ossia_device_add_node_removing_callback: callback is null");
        return nullptr;
      }

      auto the_cb = new node_cb{callback, ctx};

      convert_device(device)->on_node_removing.connect<node_cb>(the_cb);
      return reinterpret_cast<ossia_node_callback_idx_t>(the_cb);
    });
}

void ossia_device_remove_node_removing_callback(
        ossia_device_t device,
        ossia_node_callback_idx_t index)
{
    return safe_function(__func__, [=] {
      auto idx = (node_cb*) index;
      if (!device)
      {
        ossia_log_error("ossia_device_remove_node_created_callback: device is null");
        return;
      }
      if (!idx)
      {
        ossia_log_error("ossia_device_remove_node_created_callback: index is null");
        return;
      }

      convert_device(device)->on_node_removing.disconnect<node_cb>(idx);
      delete idx;
    });
}


ossia_parameter_callback_idx_t ossia_device_add_parameter_deleting_callback(
        ossia_device_t device,
        ossia_node_callback_t callback,
        void* ctx)
{
    return safe_function(__func__, [=]() -> ossia_parameter_callback_idx_t {
      if (!device)
      {
        ossia_log_error("ossia_device_add_parameter_deleting_callback: device is null");
        return nullptr;
      }
      if (!callback)
      {
        ossia_log_error("ossia_device_add_parameter_deleting_callback: callback is null");
        return nullptr;
      }

      auto the_cb = new address_cb{callback, ctx};

      convert_device(device)->on_parameter_removing.connect<address_cb>(the_cb);
      return reinterpret_cast<ossia_parameter_callback_idx_t>(the_cb);
    });
}

void ossia_device_remove_parameter_deleting_callback(
        ossia_device_t device,
        ossia_parameter_callback_idx_t index)
{
    return safe_function(__func__, [=] {
      auto idx = (address_cb*) index;
      if (!device)
      {
        ossia_log_error("ossia_device_remove_parameter_deleting_callback: device is null");
        return;
      }
      if (!idx)
      {
        ossia_log_error("ossia_device_remove_parameter_deleting_callback: index is null");
        return;
      }

      convert_device(device)->on_parameter_removing.disconnect<address_cb>(idx);
      delete idx;
    });
}

}
