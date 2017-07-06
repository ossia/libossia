#include <ossia-qt/websocket-generic-client/ws_generic_client_node.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client_address.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client_device.hpp>

namespace ossia
{
namespace net
{

ws_generic_client_node::ws_generic_client_node(
    const ws_generic_client_address_data& data,
    ws_generic_client_device& aDevice,
    ws_generic_client_node& aParent):
  m_device{aDevice},
  m_parent{&aParent}
{
  m_name = data.name;
  if(!data.request.isNull() || data.type)
    m_address = std::make_unique<ws_generic_client_address>(data, *this);
}

ws_generic_client_node::ws_generic_client_node(
    const ws_generic_client_address_data& data,
    ws_generic_client_device& aDevice):
  m_device{aDevice}
{
  m_name = data.name;
  if(!data.request.isNull() || data.type)
    m_address = std::make_unique<ws_generic_client_address>(data, *this);
}

ws_generic_client_node::~ws_generic_client_node()
{
  about_to_be_deleted(*this);

  write_lock_t lock{m_mutex};
  m_children.clear();
  m_address.reset();
}

device_base& ws_generic_client_node::get_device() const
{ return m_device; }

node_base*ws_generic_client_node::get_parent() const
{ return m_parent; }

node_base&ws_generic_client_node::set_name(std::string)
{ return *this; }

address_base* ws_generic_client_node::get_address() const
{ return m_address.get(); }

address_base*ws_generic_client_node::create_address(val_type)
{ return nullptr; }

bool ws_generic_client_node::remove_address()
{ return false; }

void ws_generic_client_node::add_child(std::unique_ptr<node_base> p)
{
  if(p)
  {
    write_lock_t lock{m_mutex};
    m_children.push_back(std::move(p));
  }
}

std::unique_ptr<node_base> ws_generic_client_node::make_child(const std::string& name)
{ return nullptr; }

void ws_generic_client_node::removing_child(node_base& node_base)
{ }

}
}
