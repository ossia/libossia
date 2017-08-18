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
        .disconnect<alias_node, &alias_node::on_deletion>(this);
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
        .disconnect<alias_node, &alias_node::on_deletion>(this);
  }

  m_origin = o;

  if (m_origin)
  {
    m_origin->about_to_be_deleted.connect<alias_node, &alias_node::on_deletion>(
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
}
}
