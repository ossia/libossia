#include <ossia-qt/serial/serial_address.hpp>
#include <ossia-qt/serial/serial_device.hpp>
#include <ossia-qt/serial/serial_node.hpp>

namespace ossia
{
namespace net
{

serial_node::serial_node(
    const serial_address_data& data, serial_device& aDevice,
    serial_node& aParent)
    : m_device{aDevice}, m_parent{&aParent}
{
  m_name = data.name;
  if (!data.request.isEmpty() || data.type)
    m_address = std::make_unique<serial_address>(data, *this);
}

serial_node::serial_node(
    const serial_address_data& data, serial_device& aDevice)
    : m_device{aDevice}
{
  m_name = data.name;
  if (!data.request.isEmpty() || data.type)
    m_address = std::make_unique<serial_address>(data, *this);
}

ossia::net::device_base& serial_node::get_device() const
{
  return m_device;
}

ossia::net::node_base* serial_node::get_parent() const
{
  return m_parent;
}

ossia::net::node_base& serial_node::set_name(std::string)
{
  return *this;
}

ossia::net::address_base* serial_node::get_address() const
{
  return m_address.get();
}

ossia::net::address_base* serial_node::create_address(ossia::val_type)
{
  return get_address();
}

bool serial_node::remove_address()
{
  return false;
}

void serial_node::add_child(std::unique_ptr<node_base> p)
{
  if (p)
  {
    write_lock_t lock{m_mutex};
    m_children.push_back(std::move(p));
  }
}
}
}
