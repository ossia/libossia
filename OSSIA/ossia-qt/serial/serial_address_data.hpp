#pragma once
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/base/address_data.hpp>

namespace ossia
{
namespace net
{
struct serial_address_data_base
{
  serial_address_data_base() = default;
  serial_address_data_base(const serial_address_data_base&) = default;
  serial_address_data_base(serial_address_data_base&&) = default;
  serial_address_data_base& operator=(const serial_address_data_base&) = default;
  serial_address_data_base& operator=(serial_address_data_base&&) = default;
  serial_address_data_base(const QJSValue& val)
  {
    auto r = val.property("request");
    if(r.isString())
    {
      request = r.toString();
    }
    else
    {
      request = val.property("name").toString();
    }
  }

  QString request;
};

struct serial_address_data :
    public address_data,
    public serial_address_data_base
{
  serial_address_data() = default;
  serial_address_data(const serial_address_data&) = default;
  serial_address_data(serial_address_data&&) = default;
  serial_address_data& operator=(const serial_address_data&) = default;
  serial_address_data& operator=(serial_address_data&&) = default;

  serial_address_data(const std::string& name):
    address_data{name}
  {

  }

  serial_address_data(const QJSValue& val):
    address_data{ossia::qt::make_address_data(val)},
    serial_address_data_base{val}
  {

  }
};
}
}
