#pragma once
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/base/parameter_data.hpp>

namespace ossia
{
namespace net
{
struct serial_parameter_data_base
{
  serial_parameter_data_base() = default;
  serial_parameter_data_base(const serial_parameter_data_base&) = default;
  serial_parameter_data_base(serial_parameter_data_base&&) = default;
  serial_parameter_data_base& operator=(const serial_parameter_data_base&)
      = default;
  serial_parameter_data_base& operator=(serial_parameter_data_base&&) = default;
  serial_parameter_data_base(const QJSValue& val)
  {
    auto r = val.property("request");
    if (r.isString())
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

struct serial_parameter_data : public parameter_data,
                             public serial_parameter_data_base
{
  serial_parameter_data() = default;
  serial_parameter_data(const serial_parameter_data&) = default;
  serial_parameter_data(serial_parameter_data&&) = default;
  serial_parameter_data& operator=(const serial_parameter_data&) = default;
  serial_parameter_data& operator=(serial_parameter_data&&) = default;

  serial_parameter_data(const std::string& name) : parameter_data{name}
  {
  }

  serial_parameter_data(const QJSValue& val)
      : parameter_data{ossia::qt::make_parameter_data(val)}
      , serial_parameter_data_base{val}
  {
  }
};
}
}
