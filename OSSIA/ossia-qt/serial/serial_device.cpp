#include <ossia-qt/serial/serial_device.hpp>
#include <ossia-qt/serial/serial_protocol.hpp>

namespace ossia
{
namespace net
{

serial_device::serial_device(const QByteArray& code, const QSerialPortInfo& bot):
  ossia::net::device_base{std::make_unique<serial_protocol>(code, bot)},
  serial_node{{}, *this}
{
  mProtocol->setDevice(*this);
}

std::string serial_device::getName() const
{ return mName; }

node_base& serial_device::setName(std::string n)
{ mName = n; return *this; }

const node_base& serial_device::getRootNode() const
{
  return *this;
}

node_base& serial_device::getRootNode()
{
  return *this;
}

}
}
