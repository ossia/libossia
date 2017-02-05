#pragma once
#include <ossia-qt/js_utilities.hpp>

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
  }

  QString request;
};

struct serial_address_data :
    public generic_address_data,
    public serial_address_data_base
{
  serial_address_data() = default;
  serial_address_data(const serial_address_data&) = default;
  serial_address_data(serial_address_data&&) = default;
  serial_address_data& operator=(const serial_address_data&) = default;
  serial_address_data& operator=(serial_address_data&&) = default;

  serial_address_data(const std::string& name):
    generic_address_data{name}
  {

  }

  serial_address_data(const QJSValue& val):
    generic_address_data{ossia::qt::make_address_data(val)},
    serial_address_data_base{val}
  {

  }
};
}
}
