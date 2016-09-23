#pragma once
#include <ossia/network/common/js_utilities.hpp>

namespace ossia
{
namespace net
{
struct ws_generic_client_address_data_base
{
  ws_generic_client_address_data_base() = default;
  ws_generic_client_address_data_base(const ws_generic_client_address_data_base&) = default;
  ws_generic_client_address_data_base(ws_generic_client_address_data_base&&) = default;
  ws_generic_client_address_data_base& operator=(const ws_generic_client_address_data_base&) = default;
  ws_generic_client_address_data_base& operator=(ws_generic_client_address_data_base&&) = default;
  ws_generic_client_address_data_base(const QJSValue& val):
    request{val.property("request")}
  {
  }

  QJSValue request;
};

struct ws_generic_client_address_data :
    public generic_address_data,
    public ws_generic_client_address_data_base
{
  ws_generic_client_address_data() = default;
  ws_generic_client_address_data(const ws_generic_client_address_data&) = default;
  ws_generic_client_address_data(ws_generic_client_address_data&&) = default;
  ws_generic_client_address_data& operator=(const ws_generic_client_address_data&) = default;
  ws_generic_client_address_data& operator=(ws_generic_client_address_data&&) = default;

  ws_generic_client_address_data(const std::string& name):
    generic_address_data{name}
  {

  }

  ws_generic_client_address_data(const QJSValue& val):
    generic_address_data{make_address_data(val)},
    ws_generic_client_address_data_base{val}
  {

  }

};

}
}
