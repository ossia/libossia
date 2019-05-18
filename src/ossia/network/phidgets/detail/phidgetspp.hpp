#pragma once
#include <ossia/network/phidgets/detail/sensors.hpp>

#include <eggs/variant.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
namespace ppp
{
class phidgets_manager;

struct phidget_device
{
  friend class phidgets_manager;

public:
  ~phidget_device();

  std::function<void(int, const char*)> onError;

  PhidgetHandle handle() const;
  const std::string& name() const;
  const std::string& type() const;
  const std::string& label() const;
  int serial() const;

  /*
  bool is_channel() const
  {
    int ok;
    Phidget_getIsChannel(m_handle, &ok);
    return (bool)ok;
  }
  */
  bool is_attached() const
  {
    int ok;
    Phidget_getAttached(m_handle, &ok);
    return (bool)ok;
  }
  bool is_hub_port_device() const
  {
    int ok;
    Phidget_getIsHubPortDevice(m_handle, &ok);
    return (bool)ok;
  }
  bool is_local() const
  {
    int ok;
    Phidget_getIsLocal(m_handle, &ok);
    return (bool)ok;
  }
  bool is_remote() const
  {
    int ok;
    Phidget_getIsRemote(m_handle, &ok);
    return (bool)ok;
  }
  bool channel_count() const
  {
    uint32_t ok;
    Phidget_getDeviceChannelCount(m_handle, PHIDCHCLASS_NOTHING, &ok);
    return (bool)ok;
  }

  Phidget_DeviceClass device_class() const
  {
    return m_class;
  }
  Phidget_DeviceID device_id() const
  {
    return m_id;
  }

  void set_label(const std::string& n);

private:
  phidget_device(PhidgetHandle hdl);
  phidget_device(const phidget_device&) = delete;
  phidget_device(phidget_device&&) = delete;
  phidget_device& operator=(const phidget_device&) = delete;
  phidget_device& operator=(phidget_device&&) = delete;

  PhidgetHandle m_handle{};

  Phidget_DeviceClass m_class{};
  Phidget_DeviceID m_id{};

  std::string m_name;
  std::string m_type;
  std::string m_label;
  int m_serial{};

  // std::unique_ptr<interface_kit> m_ik;
};

struct phidget_channel
{
  friend class phidgets_manager;

public:
  ~phidget_channel();

  std::function<void(int, const char*)> onError;

  PhidgetHandle handle() const;
  const std::string& name() const;
  const std::string& className() const;
  /*
  bool is_channel() const
  {
    int ok;
    Phidget_getIsChannel(m_handle, &ok);
    return (bool)ok;
  }
  */
  bool is_attached() const
  {
    int ok;
    Phidget_getAttached(m_handle, &ok);
    return (bool)ok;
  }

  bool is_local() const
  {
    int ok;
    Phidget_getIsLocal(m_handle, &ok);
    return (bool)ok;
  }

  bool is_remote() const
  {
    int ok;
    Phidget_getIsRemote(m_handle, &ok);
    return (bool)ok;
  }

private:
  phidget_channel(PhidgetHandle device, PhidgetHandle hdl);
  phidget_channel(const phidget_channel&) = delete;
  phidget_channel(phidget_channel&&) = delete;
  phidget_channel& operator=(const phidget_channel&) = delete;
  phidget_channel& operator=(phidget_channel&&) = delete;

  PhidgetHandle m_device{};
  PhidgetHandle m_handle{};

  Phidget_ChannelClass m_class{};
  Phidget_ChannelSubclass m_subclass{};
  int m_channel_id{};

  std::string m_name;
  std::string m_className;
};

using phidget_ptr = std::shared_ptr<phidget_device>;
class phidgets_manager
{
public:
  phidgets_manager();
  phidgets_manager(const phidgets_manager&) = delete;
  phidgets_manager(phidgets_manager&&) = delete;
  phidgets_manager& operator=(const phidgets_manager&) = delete;
  phidgets_manager& operator=(phidgets_manager&&) = delete;
  ~phidgets_manager();

  std::function<void(phidget_ptr)> onDeviceCreated;
  std::function<void(phidget_ptr)> onDeviceDestroyed;

  void open();

  const std::vector<phidget_ptr>& phidgets() const
  {
    return m_phidgets;
  }

private:
  PhidgetManagerHandle m_hdl{};

  std::vector<phidget_ptr> m_phidgets;
};
}
