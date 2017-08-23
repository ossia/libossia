#pragma once
#include <ossia/network/base/parameter_data.hpp>
#include <ossia-qt/js_utilities.hpp>
namespace ossia
{
namespace net
{
struct ws_generic_client_parameter_data_base
{
  ws_generic_client_parameter_data_base() = default;
  ws_generic_client_parameter_data_base(
      const ws_generic_client_parameter_data_base&)
      = default;
  ws_generic_client_parameter_data_base(ws_generic_client_parameter_data_base&&)
      = default;
  ws_generic_client_parameter_data_base&
  operator=(const ws_generic_client_parameter_data_base&)
      = default;
  ws_generic_client_parameter_data_base&
  operator=(ws_generic_client_parameter_data_base&&)
      = default;
  ws_generic_client_parameter_data_base(const QJSValue& val)
      : request{val.property("request")}
      , openListening{val.property("openListening")}
      , closeListening{val.property("closeListening")}
  {
  }

  QJSValue request;
  QJSValue openListening;
  QJSValue closeListening;
};

struct ws_generic_client_parameter_data
    : public parameter_data,
      public ws_generic_client_parameter_data_base
{
  ws_generic_client_parameter_data() = default;
  ws_generic_client_parameter_data(const ws_generic_client_parameter_data&)
      = default;
  ws_generic_client_parameter_data(ws_generic_client_parameter_data&&) = default;
  ws_generic_client_parameter_data&
  operator=(const ws_generic_client_parameter_data&)
      = default;
  ws_generic_client_parameter_data& operator=(ws_generic_client_parameter_data&&)
      = default;

  ws_generic_client_parameter_data(const std::string& name) : parameter_data{name}
  {
  }

  ws_generic_client_parameter_data(const QJSValue& val)
      : parameter_data{ossia::qt::make_parameter_data(val)}
      , ws_generic_client_parameter_data_base{val}
  {
  }
};
}
}
