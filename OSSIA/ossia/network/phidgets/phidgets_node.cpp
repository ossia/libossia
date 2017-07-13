// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_node.hpp"
#include "phidgets_address.hpp"
#include "phidgets_device.hpp"
#include "phidgets_protocol.hpp"
namespace ossia
{

phidget_node::~phidget_node()
{
}

phidget_node::phidget_node(phidget_device& d, net::node_base& p)
    : m_device{d}, m_parent{p}, m_name{"InterfaceKit"}
{
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

net::address_base* phidget_node::get_address() const
{
  return m_address.get();
}

net::address_base* phidget_node::create_address(val_type)
{
  return m_address.get();
}

bool phidget_node::remove_address()
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
