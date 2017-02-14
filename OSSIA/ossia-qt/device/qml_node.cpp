#include "qml_node.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>

namespace ossia
{
namespace qt
{

void qml_node::resetNode()
{
  if(m_ossia_node)
  {
    auto par = m_ossia_node->getParent();
    if(par)
    {
      par->removeChild(*m_ossia_node);
      m_ossia_node = nullptr;
    }
  }

  if(auto dev = qobject_cast<ossia::qt::qml_device*>(m_device))
  {
    std::string node_name;
    bool relative = false;

    if(m_node.isEmpty())
    {
      if(auto par = this->parent())
      {
        node_name = par->objectName().toStdString();
        if(node_name.empty())
          node_name = par->metaObject()->className();
      }

      if(node_name.empty())
        node_name = "Object";

      relative = true;
    }
    else if(m_node[0] != '/')
    {
      relative = true;
      node_name = m_node.toStdString();
    }
    else
    {
      node_name = m_node.toStdString();
    }

    ossia::net::node_base& parent =
        relative
        ? findClosestParent(this->parent(), dev->device().getRootNode())
        : dev->device().getRootNode();

    m_ossia_node = &ossia::net::find_or_create_node(parent, node_name);
    setPath(
          QString::fromStdString(
            ossia::net::address_string_from_node(*m_ossia_node)));

    reparentChildren();
    return;
  }
  reparentChildren();

  // In case something went wrong...
  setPath({});
}

}
}
