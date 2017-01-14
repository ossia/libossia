#include "alias_node.hpp"


namespace ossia
{
namespace net
{

alias_node::~alias_node()
{
  if(m_origin)
  {
    m_origin->aboutToBeDeleted.disconnect<alias_node, &alias_node::onDeletion>(this);
  }
}

address_base*alias_node::createAddress(val_type v)
{
  return m_origin ? m_origin->createAddress(v) : nullptr;
}

bool alias_node::removeAddress()
{
  return m_origin ? m_origin->removeAddress() : false;
}

address_base*alias_node::getAddress() const
{
  return m_origin ? m_origin->getAddress() : nullptr;
}

node_base*alias_node::getOrigin() const
{ return m_origin; }

void alias_node::setOrigin(node_base* o)
{
  if(m_origin)
  {
    m_origin->aboutToBeDeleted.disconnect<alias_node, &alias_node::onDeletion>(this);
  }

  m_origin = o;

  if(m_origin)
  {
    m_origin->aboutToBeDeleted.connect<alias_node, &alias_node::onDeletion>(this);
  }
}

std::unique_ptr<node_base> alias_node::makeChild(const std::string& name)
{
  return nullptr;
}

void alias_node::removingChild(node_base&)
{
}

void alias_node::onDeletion(const node_base& orig)
{
  m_origin = nullptr;
}

}
}
