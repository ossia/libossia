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
class serial_parameter final
    : public ossia::net::generic_parameter
{
  serial_parameter_data_base mData;

public:
  serial_parameter(const serial_parameter_data& p, serial_node& parent);
  ~serial_parameter();

  serial_parameter() = delete;
  serial_parameter(const serial_parameter& other) = delete;
  serial_parameter(serial_parameter&& other) = delete;
  serial_parameter& operator=(const serial_parameter& other) = delete;
  serial_parameter& operator=(serial_parameter&& other) = delete;

  void valueCallback(const ossia::value& val);
  const serial_parameter_data_base& data() const
  {
    return mData;
  }
};
}
}
