#pragma once
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia-qt/serial/serial_parameter_data.hpp>
#include <ossia-qt/serial/serial_node.hpp>

namespace ossia
{
namespace net
{
class serial_node;
class serial_protocol;
class serial_address final : public ossia::net::generic_parameter
{
  serial_parameter_data_base mData;

public:
  serial_address(const serial_parameter_data& p, serial_node& parent);

  serial_address() = delete;
  serial_address(const serial_address& other) = delete;
  serial_address(serial_address&& other) = delete;
  serial_address& operator=(const serial_address& other) = delete;
  serial_address& operator=(serial_address&& other) = delete;

  void valueCallback(const ossia::value& val);
  const serial_parameter_data_base& data() const
  {
    return mData;
  }
};
}
}
