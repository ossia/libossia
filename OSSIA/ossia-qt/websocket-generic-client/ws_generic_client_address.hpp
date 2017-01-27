#pragma once

#include <ossia/network/generic/generic_address.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client_address_data.hpp>

namespace ossia
{
namespace net
{

class OSSIA_EXPORT ws_generic_client_address final :
    public ossia::net::generic_address
{
private:
  ws_generic_client_address_data_base mData;

public:
  ws_generic_client_address(
      const ws_generic_client_address_data& data,
      ossia::net::node_base& node_base);

  ws_generic_client_address() = delete;
  ws_generic_client_address(const ws_generic_client_address& other) = delete;
  ws_generic_client_address(ws_generic_client_address&& other) = delete;
  ws_generic_client_address& operator=(const ws_generic_client_address& other) = delete;
  ws_generic_client_address& operator=(ws_generic_client_address&& other) = delete;
  ~ws_generic_client_address();

  const ws_generic_client_address_data_base& data() const
  { return mData; }

  // Non-const overload required to call functions on it since QJSValue::call is not const
  ws_generic_client_address_data_base& data()
  { return mData; }

private:
};
}
}
