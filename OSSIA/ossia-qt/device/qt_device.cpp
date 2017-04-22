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
  m_protocol->set_device(*this);
  m_capabilities.change_tree = false;
  m_root.set_name(name);
}

const net::node_base&qt_device::get_root_node() const
{
  return m_root;
}

net::node_base&qt_device::get_root_node()
{
  return m_root;
}

}
}
