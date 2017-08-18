// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-qt/websocket-generic-client/ws_generic_client_parameter.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client_device.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client_node.hpp>

namespace ossia
{
namespace net
{

ws_generic_client_node::ws_generic_client_node(
    const ws_generic_client_parameter_data& data,
    ws_generic_client_device& aDevice, ws_generic_client_node& aParent)
    : m_device{aDevice}, m_parent{&aParent}
{
  m_name = data.name;
  if (!data.request.isNull() || data.type)
    m_parameter = std::make_unique<ws_generic_client_parameter>(data, *this);
}

ws_generic_client_node::ws_generic_client_node(
    const ws_generic_client_parameter_data& data,
    ws_generic_client_device& aDevice)
    : m_device{aDevice}
{
  m_name = data.name;
  if (!data.request.isNull() || data.type)
    m_parameter = std::make_unique<ws_generic_client_parameter>(data, *this);
}

ws_generic_client_node::~ws_generic_client_node()
{
  about_to_be_deleted(*this);

  write_lock_t lock{m_mutex};
  m_children.clear();
  m_parameter.reset();
}

device_base& ws_generic_client_node::get_device() const
{
  return m_device;
}

node_base* ws_generic_client_node::get_parent() const
{
  return m_parent;
}

node_base& ws_generic_client_node::set_name(std::string)
{
  return *this;
}

parameter_base* ws_generic_client_node::get_parameter() const
{
  return m_parameter.get();
}

parameter_base* ws_generic_client_node::create_parameter(val_type)
{
  return nullptr;
}

bool ws_generic_client_node::remove_parameter()
{
  return false;
}

void ws_generic_client_node::add_child(std::unique_ptr<node_base> p)
{
  if (p)
  {
    write_lock_t lock{m_mutex};
    m_children.push_back(std::move(p));
  }
}

std::unique_ptr<node_base>
ws_generic_client_node::make_child(const std::string& name)
{
  return nullptr;
}

void ws_generic_client_node::removing_child(node_base& node_base)
{
}
}
}
