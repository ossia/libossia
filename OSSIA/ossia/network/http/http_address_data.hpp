#pragma once
#include <ossia/network/common/js_utilities.hpp>

namespace ossia
{
namespace net
{
struct http_address_data_base
{
  http_address_data_base() = default;
  http_address_data_base(const http_address_data_base&) = default;
  http_address_data_base(http_address_data_base&&) = default;
  http_address_data_base& operator=(const http_address_data_base&) = default;
  http_address_data_base& operator=(http_address_data_base&&) = default;
  http_address_data_base(const QJSValue& val)
  {
    auto r = val.property("request");
    if(r.isString())
    {
      request = r.toString();

      auto a = val.property("answer");
      if(a.isCallable())
      {
        answer = a;
      }
    }
  }

  QString request;
  QJSValue answer;
};

struct http_address_data :
    public generic_address_data,
    public http_address_data_base
{
  http_address_data() = default;
  http_address_data(const http_address_data&) = default;
  http_address_data(http_address_data&&) = default;
  http_address_data& operator=(const http_address_data&) = default;
  http_address_data& operator=(http_address_data&&) = default;

  http_address_data(const std::string& name):
    generic_address_data{name, {}, {}, {}, {}, {}}
  {

  }

  http_address_data(const QJSValue& val):
    generic_address_data{make_address_data(val)},
    http_address_data_base{val}
  {

  }

};

}
}
