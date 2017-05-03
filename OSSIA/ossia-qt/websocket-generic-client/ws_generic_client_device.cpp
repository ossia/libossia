#include <ossia-qt/websocket-generic-client/ws_generic_client_device.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client_protocol.hpp>

namespace ossia
{
namespace net
{

ws_generic_client_device::ws_generic_client_device(std::unique_ptr<ws_generic_client_protocol> protocol, std::string name):
  device_base(std::move(protocol)),
  ws_generic_client_node{ws_generic_client_address_data{name}, *this}
{
  m_protocol->set_device(*this);
}

ws_generic_client_device::~ws_generic_client_device()
{

}

}
}
