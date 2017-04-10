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
  resetNode();
  connect(this, &QQuickItem::parentChanged,
          this, [=] (QQuickItem*) { resetNode(); });
}

void qml_node::resetNode(bool recursive)
{
  m_node.clear();
  if(m_ossia_node)
  {
    auto par = m_ossia_node->getParent();
    if(par)
    {
      auto node = m_ossia_node;
      m_ossia_node = nullptr;
      par->removeChild(*node);
    }
  }

  if(m_device)
  {
    std::string node_name;
    bool relative = false;

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
    else
    {
      node_name = m_userRequestedNode.toStdString();
    }

    ossia::net::node_base& parent =
        relative
        ? findClosestParent(this->parent(), m_device->device().getRootNode())
        : m_device->device().getRootNode();

    auto setup_valid_node = [&] {
      m_ossia_node->aboutToBeDeleted.connect<qml_node, &qml_node::on_node_deleted>(this);
      m_node = QString::fromStdString(m_ossia_node->getName());
      setPath(
            QString::fromStdString(
              ossia::net::address_string_from_node(*m_ossia_node)));
      if(recursive)
        reparentChildren();
    };

    if(m_device->readPreset())
    {
      m_ossia_node = ossia::net::find_node(parent, node_name);
      if(m_ossia_node)
      {
        setup_valid_node();
      }
      else
      {
        if(recursive)
          reparentChildren();
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

  if(recursive)
    reparentChildren();

  // In case something went wrong...
  setPath({});
}


void qml_node::on_node_deleted(const ossia::net::node_base&)
{
  m_ossia_node = nullptr;
}
}
}
