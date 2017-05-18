#pragma once
#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/base/address_data.hpp>
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
    request{val.property("request")},
    openListening{val.property("openListening")},
    closeListening{val.property("closeListening")}
  {
  }

  QJSValue request;
  QJSValue openListening;
  QJSValue closeListening;
};

struct ws_generic_client_address_data :
    public address_data,
    public ws_generic_client_address_data_base
{
  ws_generic_client_address_data() = default;
  ws_generic_client_address_data(const ws_generic_client_address_data&) = default;
  ws_generic_client_address_data(ws_generic_client_address_data&&) = default;
  ws_generic_client_address_data& operator=(const ws_generic_client_address_data&) = default;
  ws_generic_client_address_data& operator=(ws_generic_client_address_data&&) = default;

  ws_generic_client_address_data(const std::string& name):
    address_data{name}
  {

  }

  ws_generic_client_address_data(const QJSValue& val):
    address_data{ossia::qt::make_address_data(val)},
    ws_generic_client_address_data_base{val}
  {

  }

};

}
}
