#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client_node.hpp>

namespace ossia
{
namespace net
{
class ws_generic_client_protocol;
class OSSIA_EXPORT ws_generic_client_device final :
    public ossia::net::device_base,
    public ws_generic_client_node
{
public:
  ws_generic_client_device() = delete;
  ws_generic_client_device(const ws_generic_client_device&) = delete;
  ws_generic_client_device(ws_generic_client_device&&) = delete;
  ws_generic_client_device& operator=(const ws_generic_client_device&) = delete;
  ws_generic_client_device& operator=(ws_generic_client_device&&) = delete;

  ws_generic_client_device(
      std::unique_ptr<ossia::net::ws_generic_client_protocol> protocol_base,
      std::string name);

  const ossia::net::node_base& getRootNode() const override
  {
    return *this;
  }
  ossia::net::node_base& getRootNode() override
  {
    return *this;
  }

  using ws_generic_client_node::getName;
  using ws_generic_client_node::setName;

  ~ws_generic_client_device();
};

}
}
