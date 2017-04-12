#include "qml_node.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <QDebug>
namespace ossia
{
namespace qt
{

qml_node::qml_node(QQuickItem* parent):
  qml_node_base{parent}
{
  setDevice(&qml_singleton_device::instance());
  resetNode();
  connect(this, &QQuickItem::parentChanged,
          this, [=] (QQuickItem*) { resetNode(); });
}

void qml_node::reset_parent()
{
  const bool reading = m_device ? m_device->readPreset() : false;
  if(m_ossia_node)
  {
    auto par = m_ossia_node->getParent();
    if(par)
    {
      auto node = m_ossia_node;
      m_ossia_node = nullptr;
      if(!reading)
        par->removeChild(*node);
    }
  }

}
qml_node::~qml_node()
{
  reset_parent();
}

void qml_node::resetNode()
{
  m_node.clear();
  const bool reading = m_device ? m_device->readPreset() : false;
  reset_parent();

  // Creation may not have finished yet.
  if(m_parentNode && !m_parentNode->ossiaNode())
  {
    setPath({});
    return;
  }

  if(m_device)
  {
    // Utility function to set-up a node.
    auto setup_valid_node = [&] {
      m_ossia_node->aboutToBeDeleted.connect<qml_node, &qml_node::on_node_deleted>(this);
      m_node = QString::fromStdString(m_ossia_node->getName());
      setPath(
            QString::fromStdString(
              ossia::net::address_string_from_node(*m_ossia_node)));
    };

    std::string node_name;
    bool relative = false;

    // Naming logic
    if(m_userRequestedNode.isEmpty())
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
    else if(m_userRequestedNode[0] != '/')
    {
      relative = true;
      node_name = m_userRequestedNode.toStdString();
    }
    else if(m_userRequestedNode == QStringLiteral("/"))
    {
      m_ossia_node = &m_device->device().getRootNode();
      setup_valid_node();
      return;
    }
    else
    {
      node_name = m_userRequestedNode.toStdString();
    }

    // Find the node
    auto get_parent = [&] () -> ossia::net::node_base&
    {
      if(m_parentNode)
        return *m_parentNode->ossiaNode();

      if(relative)
      {
        return findClosestParent(this->parent(), m_device->device().getRootNode());
      }
      else
      {
        return m_device->device().getRootNode();
      }
    };

    ossia::net::node_base& parent = get_parent();

    if(reading)
    {
      m_ossia_node = ossia::net::find_node(parent, node_name);
      if(m_ossia_node)
      {
        setup_valid_node();
      }
      else
      {
        setPath({});
      }
    }
    else
    {
      m_ossia_node = &ossia::net::create_node(parent, node_name);
      setup_valid_node();
    }
    return;
  }

  // In case something went wrong...
  setPath({});
}


void qml_node::on_node_deleted(const ossia::net::node_base&)
{
  m_ossia_node = nullptr;
}

}
}
