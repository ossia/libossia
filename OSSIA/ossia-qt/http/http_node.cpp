#include <ossia-qt/http/http_address.hpp>
#include <ossia-qt/http/http_device.hpp>
#include <ossia-qt/http/http_node.hpp>

namespace ossia
{
namespace net
{

http_node::http_node(
    const http_address_data& data, http_device& aDevice, http_node& aParent)
    : m_device{aDevice}, m_parent{&aParent}
{
  m_name = data.name;
  if (!data.request.isEmpty() || data.type)
    m_address = std::make_unique<http_address>(data, *this);
}

http_node::http_node(const http_address_data& data, http_device& aDevice)
    : m_device{aDevice}
{
  m_name = data.name;
  if (!data.request.isEmpty() || data.type)
    m_address = std::make_unique<http_address>(data, *this);
}

http_node::~http_node()
{
  about_to_be_deleted(*this);

  write_lock_t lock{m_mutex};
  m_children.clear();
  m_address.reset();
}

device_base& http_node::get_device() const
{
  return m_device;
}

node_base* http_node::get_parent() const
{
  return m_parent;
}

node_base& http_node::set_name(std::string)
{
  return *this;
}

address_base* http_node::get_address() const
{
  return m_address.get();
}

address_base* http_node::create_address(val_type)
{
  return nullptr;
}

bool http_node::remove_address()
{
  return false;
}

void http_node::add_child(std::unique_ptr<node_base> p)
{
  if (p)
  {
    write_lock_t lock{m_mutex};
    m_children.push_back(std::move(p));
  }
}

std::unique_ptr<node_base> http_node::make_child(const std::string& name)
{
  return nullptr;
}

void http_node::removing_child(ossia::net::node_base& node_base)
{
}
}
}
