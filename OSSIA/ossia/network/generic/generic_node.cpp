// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
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
}

generic_node_base::generic_node_base(
    std::string name, ossia::net::device_base& aDevice)
    : m_device{aDevice}
{
  m_name = std::move(name);
}

device_base& generic_node_base::get_device() const
{
  return m_device;
}

node_base* generic_node_base::get_parent() const
{
  return m_parent;
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
  about_to_be_deleted(*this);

  m_children.clear();
  remove_address();
}

ossia::net::address_base* generic_node::get_address() const
{
  return m_address.get();
}

void generic_node::set_address(std::unique_ptr<ossia::net::address_base> addr)
{
  remove_address();
  if (addr)
  {
    m_address = std::move(addr);
    m_device.on_address_created(*m_address);
  }
}

ossia::net::address_base* generic_node::create_address(ossia::val_type type)
{
  // clear former address
  remove_address();

  // edit new address
  m_address = std::make_unique<ossia::net::generic_address>(*this);

  // set type
  m_address->set_value_type(type);

  // notify observers
  m_device.on_address_created(*m_address);

  return m_address.get();
}

bool generic_node::remove_address()
{
  // use the device protocol to stop address value observation
  if (m_address)
  {
    // notify observers
    m_device.on_address_removing(*m_address);

    auto& device = get_device();
    device.get_protocol().observe(*m_address, false);

    m_address.reset();

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
