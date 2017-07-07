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
  m_protocol->set_device(*this);
}

node_base& serial_device::set_name(std::string n)
{ m_name = n; return *this; }

const node_base& serial_device::get_root_node() const
{
  return *this;
}

node_base& serial_device::get_root_node()
{
  return *this;
}

}
}
