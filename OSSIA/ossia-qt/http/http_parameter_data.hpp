#pragma once
#include <ossia/network/base/parameter_data.hpp>
#include <ossia-qt/js_utilities.hpp>

namespace ossia
{
namespace net
{
struct http_parameter_data_base
{
  http_parameter_data_base() = default;
  http_parameter_data_base(const http_parameter_data_base&) = default;
  http_parameter_data_base(http_parameter_data_base&&) = default;
  http_parameter_data_base& operator=(const http_parameter_data_base&) = default;
  http_parameter_data_base& operator=(http_parameter_data_base&&) = default;
  http_parameter_data_base(const QJSValue& val)
  {
    auto r = val.property("request");
    if (r.isString())
    {
      request = r.toString();

      auto a = val.property("answer");
      if (a.isCallable())
      {
        answer = a;
      }
    }
  }

  QString request;
  QJSValue answer;
};

struct http_parameter_data : public parameter_data, public http_parameter_data_base
{
  http_parameter_data() = default;
  http_parameter_data(const http_parameter_data&) = default;
  http_parameter_data(http_parameter_data&&) = default;
  http_parameter_data& operator=(const http_parameter_data&) = default;
  http_parameter_data& operator=(http_parameter_data&&) = default;

  http_parameter_data(const std::string& name) : parameter_data{name}
  {
  }

  http_parameter_data(const QJSValue& val)
      : parameter_data{ossia::qt::make_parameter_data(val)}
      , http_parameter_data_base{val}
  {
  }
};
}
}
