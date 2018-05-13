// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qml_node.hpp"
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <QDebug>
#include <ossia-qt/device/qml_device.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::qt::qml_node)
namespace ossia
{
namespace qt
{

qml_node::qml_node(QQuickItem* parent) : qml_node_base{parent}
{
  setDevice(&qml_singleton_device::instance());
  resetNode();
  connect(this, &QQuickItem::parentChanged, this, &qml_node::resetNode);
}

void qml_node::node_destroyed()
{
  reset_parent();
  disconnect(this, &QQuickItem::parentChanged, this, &qml_node::resetNode);
  if (m_device)
    m_device->remove(this);
  m_was_destroyed = true;
  deleteLater();
}
void qml_node::reset_parent()
{
  const bool reading = m_device ? m_device->readPreset() : false;
  if (m_ossia_node)
  {
    auto par = m_ossia_node->get_parent();
    if (par)
    {
      auto node = m_ossia_node;
      m_ossia_node = nullptr;
      if (!reading)
      {
        //qDebug() << ossia::net::address_string_from_node(*par).c_str() << " had " << par->children().size() << " children";

        par->remove_child(*node);
        //qDebug() << ossia::net::address_string_from_node(*par).c_str() << " now has " << par->children().size() << " children";
      }
    }
  }
}
qml_node::~qml_node()
{
  disconnect(this, &QQuickItem::parentChanged, this, &qml_node::resetNode);
  if (m_device)
    m_device->remove(this);
  reset_parent();
}

void qml_node::setDevice(QObject* device)
{
  auto olddev = m_device;
  auto newdev = qobject_cast<qml_device*>(device);
  if (olddev != newdev)
  {
    if (olddev)
    {
      olddev->remove(this);
    }

    if (newdev)
    {
      newdev->add(this);
    }

    qml_node_base::setDevice(device);
  }
}

void qml_node::resetNode()
{
  if(m_was_destroyed)
    return;
  m_node.clear();
  nodeChanged(m_node);
  const bool reading = m_device ? m_device->readPreset() : false;
  reset_parent();

  // Creation may not have finished yet.
  if (m_parentNode && !m_parentNode->ossiaNode())
  {
    m_parentNode->resetNode();
  }

  if (m_device)
  {
    // Utility function to set-up a node.
    auto setup_valid_node = [&] {
      m_ossia_node->about_to_be_deleted
          .connect<qml_node, &qml_node::on_node_deleted>(this);
      m_node = QString::fromStdString(m_ossia_node->get_name());
      nodeChanged(m_node);
      setPath(QString::fromStdString(
          ossia::net::address_string_from_node(*m_ossia_node)));
      applyNodeAttributes();
    };

    std::string node_name;
    bool relative = false;

    // Naming logic
    if (m_userRequestedNode.isEmpty())
    {
      if (auto par = this->parent())
      {
        node_name = par->objectName().toStdString();
        if (node_name.empty())
          node_name = par->metaObject()->className();
      }

      if (node_name.empty())
        node_name = "Object";

      relative = true;
    }
    else if (m_userRequestedNode[0] != '/')
    {
      relative = true;
      node_name = m_userRequestedNode.toStdString();
    }
    else if (m_userRequestedNode == QStringLiteral("/"))
    {
      m_ossia_node = &m_device->device().get_root_node();
      setup_valid_node();
      return;
    }
    else
    {
      node_name = m_userRequestedNode.toStdString();
    }

    // Find the node
    auto get_parent = [&]() -> ossia::net::node_base& {
      if (m_parentNode && m_parentNode->ossiaNode())
        return *m_parentNode->ossiaNode();

      if (relative)
      {
        return findClosestParent(
            this->parent(), m_device->device().get_root_node());
      }
      else
      {
        return m_device->device().get_root_node();
      }
    };

    ossia::net::node_base& parent = get_parent();

    if (reading)
    {
      m_ossia_node = ossia::net::find_node(parent, node_name);
      if (m_ossia_node)
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
