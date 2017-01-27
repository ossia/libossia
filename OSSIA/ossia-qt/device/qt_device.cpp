#include "qt_device.hpp"
#include <QEvent>
#include <QDebug>
#include <QTimer>
namespace ossia
{
namespace qt
{

qt_device::qt_device(
    QObject& obj,
    std::unique_ptr<net::protocol_base> protocol,
    std::string name):
  device_base(std::move(protocol)),
  m_root{obj, *this}
{
  mProtocol->setDevice(*this);
  mCapabilities.change_tree = false;
  m_root.setName(name);
}

const net::node_base&qt_device::getRootNode() const
{
  return m_root;
}

net::node_base&qt_device::getRootNode()
{
  return m_root;
}

}
}
