#include "qml_property_reader.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <QDebug>

namespace ossia
{
namespace qt
{
void qml_property_reader::resetNode()
{
  clearNode();

  if(m_device && m_userRequestedNode.startsWith('/'))
  {
    m_ossia_node = ossia::net::find_node(
                     m_device->device().getRootNode(),
                     m_userRequestedNode.toStdString());

    if(m_ossia_node)
    {
      m_ossia_node->aboutToBeDeleted.connect<qml_property_reader, &qml_property_reader::on_node_deleted>(this);
      m_address = m_ossia_node->getAddress();
      m_propCtx = new qml_property_context{m_targetProperty, *m_address, *m_device, this};

      m_address->setValueQuiet(qt_to_ossia{}(m_targetProperty.read()));
    }
  }
}


qml_property_reader::qml_property_reader(QQuickItem *parent)
  : qml_node_base(parent)
{
  setDevice(&qml_singleton_device::instance());
  resetNode();
}

qml_property_reader::~qml_property_reader()
{
  if(m_device) m_device->reader_properties.erase(this);
  m_address = nullptr;
}

void qml_property_reader::setTarget(const QQmlProperty &prop)
{
  m_targetProperty = prop;
  resetNode();
}

void qml_property_reader::setDevice(QObject* device)
{
  auto olddev = m_device;
  auto newdev = qobject_cast<qml_device*>(device);
  if(olddev != newdev)
  {
    if(olddev)
    {
      olddev->reader_properties.erase(this);
    }

    if(newdev)
    {
      newdev->reader_properties.insert({this, this});
    }

    qml_node_base::setDevice(device);
  }
}

void qml_property_reader::on_node_deleted(const net::node_base&)
{
  clearNode();
}

void qml_property_reader::clearNode()
{
  m_node.clear();
  m_address = nullptr;
  m_ossia_node = nullptr;

  if(m_propCtx)
    delete m_propCtx;
  m_propCtx = nullptr;
}

void qml_property_context::qtVariantChanged()
{
  auto val = qt_to_ossia{}(targetProperty.read());
  address.setValueQuiet(val);
  device.device().getProtocol().push(address);
}

}
}
