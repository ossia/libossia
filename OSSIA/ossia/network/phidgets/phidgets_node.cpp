// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_node.hpp"
#include "phidgets_parameter.hpp"
#include "phidgets_device.hpp"
#include "phidgets_protocol.hpp"
#include <ossia/network/base/node_functions.hpp>
namespace ossia
{

phidget_node::~phidget_node()
{
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
      Phidget_ChannelSubclass sc{};
      Phidget_getChannelSubclass(hdl, &sc);
      switch(sc)
      {
        case PHIDCHSUBCLASS_VOLTAGERATIOINPUT_BRIDGE:
          name = "Wheatstone Bridge";
          break;

        case PHIDCHSUBCLASS_NONE:
        default:
        {
          Phidget_getChannelClassName(hdl, &arr);
          if(arr && strlen(arr) > 0)
          {
            name = arr;
          }
          else
          {
            Phidget_getChannelName(hdl, &arr);
            if(arr && strlen(arr) > 0)
            {
              name = arr;
            }
          }
          break;
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

        Phidget_getDeviceClassName(hdl, &arr);
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
        }
      }
    }

    int isport{};
    Phidget_getIsHubPortDevice(hdl, &isport);
    if(isport)
    {

      int port{};
      Phidget_getHubPort(hdl, &port);
      name += " - ";
      name += std::to_string(port);
    }
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
}
