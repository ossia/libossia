// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "alias_node.hpp"

namespace ossia
{
namespace net
{

alias_node::~alias_node()
{
  if (m_origin)
  {
    m_origin->about_to_be_deleted
        .disconnect<&alias_node::on_deletion>(this);
  }
}

parameter_base* alias_node::create_parameter(val_type v)
{
  return m_origin ? m_origin->create_parameter(v) : nullptr;
}

bool alias_node::remove_parameter()
{
  return m_origin ? m_origin->remove_parameter() : false;
}

parameter_base* alias_node::get_parameter() const
{
  // TODO thread_local rec_count = 0;
  return m_origin ? m_origin->get_parameter() : nullptr;
}

node_base* alias_node::get_origin() const
{
  return m_origin;
}

void alias_node::set_origin(node_base* o)
{
  if (m_origin)
  {
    m_origin->about_to_be_deleted
        .disconnect<&alias_node::on_deletion>(this);
  }

  m_origin = o;

  if (m_origin)
  {
    m_origin->about_to_be_deleted.connect<&alias_node::on_deletion>(
        this);
  }
}

std::unique_ptr<node_base> alias_node::make_child(const std::string& name)
{
  return nullptr;
}

void alias_node::removing_child(node_base&)
{
}

void alias_node::on_deletion(const node_base& orig)
{
  m_origin = nullptr;
}


alias_path::alias_path(std::string name, device_base& aDevice, node_base& parent)
  : ossia::net::generic_node_base{name, aDevice, parent}
  , ossia::net::parameter_base{(ossia::net::node_base&)*this}
{

}

alias_path::~alias_path()
{

}

const std::vector<node_base*>&alias_path::get_roots()
{
  return m_roots;
}

void alias_path::set_roots(std::vector<node_base*> roots)
{
  m_roots = std::move(roots);
  // TODO connect on deletion
}

traversal::path alias_path::get_path() const
{
  return m_path;
}

void alias_path::set_path(traversal::path o)
{
  m_path = std::move(o);
}

parameter_base*alias_path::create_parameter(val_type v) { return this; }

bool alias_path::remove_parameter() { return false; }

parameter_base*alias_path::get_parameter() const { return (ossia::net::parameter_base*)(this); }

std::unique_ptr<node_base> alias_path::make_child(const std::string& name) { return {}; }

void alias_path::removing_child(node_base&) { }

void alias_path::pull_value()
{
}


std::future<void> alias_path::pull_value_async()
{
  return {};
}

void alias_path::request_value()
{
}

value alias_path::value() const
{
  return {};
}

template<typename Fun>
void alias_path::do_for_nodes(Fun f)
{
  auto nodes = m_roots;
  ossia::traversal::apply(m_path, nodes);
  for(ossia::net::node_base* n : nodes)
    f(*n);
}

parameter_base&alias_path::push_value(const ossia::value& v)
{
  do_for_nodes([&] (ossia::net::node_base& n) {
    if(auto p = n.get_parameter())
      p->push_value(v);
  });
  return *this;
}

parameter_base&alias_path::push_value(ossia::value&& v)
{
  return push_value(v);
}


parameter_base&alias_path::push_value()
{
  do_for_nodes([] (ossia::net::node_base& n) {
    if(auto p = n.get_parameter())
      p->push_value();
  });
  return *this;
}


parameter_base&alias_path::set_value(const ossia::value& v)
{
  do_for_nodes([&] (ossia::net::node_base& n) {
    if(auto p = n.get_parameter())
      p->set_value(v);
  });
  return *this;
}


parameter_base&alias_path::set_value(ossia::value&& v)
{
  return set_value(v);
}


void alias_path::set_value_quiet(const ossia::value& v)
{
  do_for_nodes([&] (ossia::net::node_base& n) {
    if(auto p = n.get_parameter())
      p->set_value_quiet(v);
  });
}


void alias_path::set_value_quiet(ossia::value&& v)
{
  return set_value_quiet(v);
}


val_type alias_path::get_value_type() const
{
  return {};
}


parameter_base&alias_path::set_value_type(val_type)
{
  return *this;
}


access_mode alias_path::get_access() const
{
  return {};
}


parameter_base&alias_path::set_access(access_mode)
{
  return *this;
}


const domain&alias_path::get_domain() const
{
  static ossia::domain d;
  return d;
}


parameter_base&alias_path::set_domain(const domain&)
{
  return *this;
}


bounding_mode alias_path::get_bounding() const
{
  return {};
}


parameter_base&alias_path::set_bounding(bounding_mode)
{
  return *this;
}

}
}
