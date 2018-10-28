// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <cassert>
#include <iostream>
namespace ossia
{
namespace net
{
generic_node_base::generic_node_base(
    std::string name, ossia::net::device_base& aDevice, node_base& aParent)
    : m_device{aDevice}, m_parent{&aParent}
{
  m_name = std::move(name);
  m_oscAddressCache = ossia::net::osc_parameter_string(*this);
}

generic_node_base::generic_node_base(
    std::string name, ossia::net::device_base& aDevice)
    : m_device{aDevice}
{
  m_name = std::move(name);
  m_oscAddressCache = ossia::net::osc_parameter_string(*this);
}

device_base& generic_node_base::get_device() const
{
  return m_device;
}

node_base* generic_node_base::get_parent() const
{
  return m_parent;
}

void generic_node_base::on_address_change()
{
  m_oscAddressCache = ossia::net::osc_parameter_string(*this);
  for (auto& cld : m_children)
  {
    cld->on_address_change();
  }
}

node_base& generic_node_base::set_name(std::string name)
{
  auto old_name = std::move(m_name);
  if (m_parent)
  {
    read_lock_t lock{m_mutex};
    sanitize_name(name, m_parent->unsafe_children());
    m_name = name;
  }
  else
  {
    m_name = std::move(name);
    sanitize_name(m_name);
  }
  on_address_change();

  // notify observers
  m_device.on_node_renamed(*this, old_name);

  return *this;
}

generic_node::generic_node(
    std::string name, device_base& aDevice, node_base& aParent)
    : generic_node_base{std::move(name), aDevice, aParent}
{
}

generic_node::generic_node(std::string name, device_base& aDevice)
    : generic_node_base{std::move(name), aDevice}
{
}

generic_node::~generic_node()
{
  m_children.clear();

  about_to_be_deleted(*this);

  remove_parameter();
}

ossia::net::parameter_base* generic_node::get_parameter() const
{
  return m_parameter.get();
}

void generic_node::set_parameter(
    std::unique_ptr<ossia::net::parameter_base> addr)
{
  remove_parameter();
  if (addr)
  {
    m_parameter = std::move(addr);
    m_device.on_parameter_created(*m_parameter);
  }
}

ossia::net::parameter_base*
generic_node::create_parameter(ossia::val_type type)
{
  if(!m_parameter)
  {
    // setup new parameter
    m_parameter = std::make_unique<ossia::net::generic_parameter>(*this);
    m_parameter->set_value_type(type);

    // notify observers
    m_device.on_parameter_created(*m_parameter);
  }
  else
  {
    m_parameter->set_value_type(type);
  }

  return m_parameter.get();
}

bool generic_node::remove_parameter()
{
  // use the device protocol to stop address value observation
  if (m_parameter)
  {
    // notify observers
    auto addr = std::move(m_parameter);
    m_device.on_parameter_removing(*addr);
    auto& device = get_device();
    device.get_protocol().observe(*addr, false);

    addr.reset();

    return true;
  }

  return false;
}

std::unique_ptr<ossia::net::node_base>
generic_node::make_child(const std::string& name_base)
{
  return std::make_unique<generic_node>(name_base, m_device, *this);
}
}
}
