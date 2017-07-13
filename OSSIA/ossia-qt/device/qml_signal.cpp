#include "qml_signal.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <QDebug>
#include <QTimer>
namespace ossia
{
namespace qt
{

qml_signal::qml_signal(QQuickItem* parent) : qml_property_base(parent)
{
  setDevice(&qml_singleton_device::instance());
  resetNode();
  connect(this, &qml_signal::trigger, this, [=] {
    if (m_address)
      m_address->push_value(ossia::impulse{});
  });
}

qml_signal::~qml_signal()
{
  if (m_ossia_node)
  {
    m_ossia_node->about_to_be_deleted
        .disconnect<qml_property_base, &qml_property_base::on_node_deleted>(
            this);
  }
  if (m_device)
    m_device->remove(this);
  m_address = nullptr;
}

void qml_signal::setDevice(QObject* device)
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

void qml_signal::setupAddress(bool reading)
{
  if (reading)
    return;

  m_address = nullptr;
  m_callback = ossia::none;
  if (m_ossia_node)
  {
    m_ossia_node->remove_address();
    m_address = m_ossia_node->create_address(ossia::val_type::IMPULSE);
    if (m_address)
    {
      m_callback = m_address->add_callback(
          [this](const ossia::value&) { triggered(); });
    }
  }
}

void qml_signal::resetNode()
{
  const bool reading = m_device ? m_device->readPreset() : false;
  clearNode(reading);

  // Creation may not have finished yet.
  if (m_parentNode && !m_parentNode->ossiaNode())
  {
    setPath({});
    m_address = nullptr;
    m_callback = ossia::none;
    return;
  }

  if (m_device)
  {
    std::string node_name;
    bool relative = false;

    if (m_userRequestedNode.isEmpty())
    {
      node_name = "value";
      relative = true;
    }
    else if (m_userRequestedNode[0] != '/')
    {
      node_name = m_userRequestedNode.toStdString();
      relative = true;
    }
    else
    {
      node_name = m_userRequestedNode.toStdString();
    }

    m_ossia_node = ossia::net::find_or_create_node(
        get_parent(this->parentItem(), relative), node_name, reading);
    if (m_ossia_node)
    {
      m_ossia_node->about_to_be_deleted
          .connect<qml_property_base, &qml_property_base::on_node_deleted>(
              this);
      m_node = QString::fromStdString(m_ossia_node->get_name());
      m_address = m_ossia_node->get_address();

      setPath(QString::fromStdString(
          ossia::net::address_string_from_node(*m_ossia_node)));
      setupAddress(reading);
      applyNodeAttributes();
      return;
    } // else, we go through the reset:
  }

  // In case something went wrong...
  setPath({});
  m_address = nullptr;
  m_callback = ossia::none;
}
}
}
