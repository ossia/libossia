#pragma once
#include <ossia/network/generic/generic_address.hpp>
#include <ossia-qt/serial/serial_address_data.hpp>
#include <ossia-qt/serial/serial_node.hpp>

namespace ossia
{
namespace net
{
class serial_node;
class serial_protocol;
class serial_address final :
    public ossia::net::generic_address
{
  serial_address_data_base mData;
public:
  serial_address(
      const serial_address_data& p,
      serial_node& parent);

  serial_address() = delete;
  serial_address(const serial_address& other) = delete;
  serial_address(serial_address&& other) = delete;
  serial_address& operator=(const serial_address& other) = delete;
  serial_address& operator=(serial_address&& other) = delete;

  void valueCallback(const ossia::value& val);
  const serial_address_data_base& data() const
  { return mData; }
};

}
}
