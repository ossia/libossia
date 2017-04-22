#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia-qt/serial/serial_node.hpp>
class QSerialPortInfo;
namespace ossia
{
namespace net
{

class OSSIA_EXPORT serial_device :
    public ossia::net::device_base,
    public serial_node
{
public:
  serial_device(
      const QByteArray& code,
      const QSerialPortInfo& bot);

  std::string getName() const override;
  node_base& setName(std::string n) override;

  const ossia::net::node_base& get_root_node() const override;
  ossia::net::node_base& get_root_node() override;
};

}
}
