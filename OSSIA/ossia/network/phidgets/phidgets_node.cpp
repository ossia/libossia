// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_node.hpp"
#include "phidgets_device.hpp"
#include "phidgets_protocol.hpp"
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/device.hpp>
namespace ossia
{

phidget_node::~phidget_node()
{
  m_children.clear();

  about_to_be_deleted(*this);

  remove_parameter();
  Phidget_release(&m_hdl);
}

void phidget_node::set_parameter(std::unique_ptr<net::parameter_base> a)
{
  if(!a && m_parameter)
    m_device.on_parameter_removing(*m_parameter);

  m_parameter = std::move(a);

  if(m_parameter)
    m_device.on_parameter_created(*m_parameter);
}

phidget_node::phidget_node(PhidgetHandle hdl, net::device_base& d, net::node_base& p)
  : m_hdl{hdl}, m_device{d}, m_parent{p}
{
    std::string name = "Phidget";

    const char* arr{};
    int chan{false};
    Phidget_getIsChannel(hdl, &chan);
    if(chan)
    {
      Phidget_getChannelName(hdl, &arr);
      if(arr && strlen(arr) > 0)
      {
        name = arr;
        {
          uint32_t chan_count;
          auto err = Phidget_getDeviceChannelCount(hdl, PHIDCHCLASS_NOTHING, &chan_count);
          if(err == EPHIDGET_OK && chan_count > 1)
          {
            int c;
            err = Phidget_getChannel(hdl, &c);
            if(err == EPHIDGET_OK)
            {
              name += "." + std::to_string(c);
            }
          }
        }
      }
      else
      {
        Phidget_getChannelClassName(hdl, &arr);
        if(arr && strlen(arr) > 0)
        {
          name = arr;
        }
      }
    }
    else
    {
      Phidget_getDeviceLabel(hdl, &arr);
      if(arr && strlen(arr) > 0)
      {
        name = arr;
      }
      else
      {

        Phidget_getDeviceName(hdl, &arr);
        if(arr && strlen(arr) > 0)
        {
          name = arr;
        }
        else
        {
          Phidget_getDeviceClassName(hdl, &arr);
          if(arr && strlen(arr) > 0)
          {
            name = arr;
          }
        }
      }
    }

    /*
    {
      int port{};
      Phidget_getHubPort(hdl, &port);
      name += "-";
      name += std::to_string(port);
      name += "-";
    }
    */
    m_name = ossia::net::sanitize_name(name, p.children_names());
    this->m_oscAddressCache = ossia::net::osc_parameter_string(*this);
}

net::device_base& phidget_node::get_device() const
{
  return m_device;
}

net::node_base* phidget_node::get_parent() const
{
  return &m_parent;
}

net::node_base& phidget_node::set_name(std::string n)
{
  return *this;
}

net::parameter_base* phidget_node::get_parameter() const
{
  return m_parameter.get();
}

net::parameter_base* phidget_node::create_parameter(val_type)
{
  return m_parameter.get();
}

bool phidget_node::remove_parameter()
{
  return false;
}

std::unique_ptr<net::node_base>
phidget_node::make_child(const std::string& name)
{
  return {};
}

void phidget_node::removing_child(net::node_base& node_base)
{
}

void phidget_node::add_child_simple(std::unique_ptr<net::node_base> n)
{
  auto& dev = get_device();
  auto ptr = n.get();
  {
    write_lock_t lock{m_mutex};
    m_children.push_back(std::move(n));
  }
  dev.on_node_created(*ptr);
}
}





namespace ossia
{

phidget_hub_port_node::~phidget_hub_port_node()
{
}

phidget_hub_port_node::phidget_hub_port_node(PhidgetHandle hdl, int num, net::device_base& d, net::node_base& p)
  : m_hdl{hdl}, m_device{d}, m_parent{p}
{
  std::string name = "Port." + std::to_string(num);
  m_name = ossia::net::sanitize_name(name, p.children_names());
  this->m_oscAddressCache = ossia::net::osc_parameter_string(*this);
}

net::device_base& phidget_hub_port_node::get_device() const
{
  return m_device;
}

net::node_base* phidget_hub_port_node::get_parent() const
{
  return &m_parent;
}

net::node_base& phidget_hub_port_node::set_name(std::string n)
{
  return *this;
}

net::parameter_base* phidget_hub_port_node::get_parameter() const
{
  return {};
}

net::parameter_base* phidget_hub_port_node::create_parameter(val_type)
{
  return {};
}

bool phidget_hub_port_node::remove_parameter()
{
  return false;
}

std::unique_ptr<net::node_base>
phidget_hub_port_node::make_child(const std::string& name)
{
  return {};
}

void phidget_hub_port_node::removing_child(net::node_base& node_base)
{
}

void phidget_hub_port_node::add_child_simple(std::unique_ptr<net::node_base> n)
{
  auto& dev = get_device();
  auto ptr = n.get();
  {
    write_lock_t lock{m_mutex};
    m_children.push_back(std::move(n));
  }
  dev.on_node_created(*ptr);
}
}
