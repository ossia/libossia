#pragma once
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/detail/server.hpp>
#include <ossia/network/oscquery/oscquery_client.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <eggs/variant.hpp>
#include <readerwriterqueue.h>

#include <hopscotch_map.h>
#include <ossia/detail/mutex.hpp>
#include <atomic>
#include <nano_signal_slot.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>
namespace osc
{
class sender;
class receiver;
}

namespace oscpack
{
class ReceivedMessage;
class IpEndpointName;
}

namespace ossia
{
namespace oscquery
{
//! Implementation of an oscquery server.
class OSSIA_EXPORT oscquery_server_protocol final : public ossia::net::protocol_base
{
  friend class query_answerer;
  friend class get_query_answerer;
  friend struct json_query_answerer;
public:
  using connection_handler = websocket_server::connection_handler;
  oscquery_server_protocol(uint16_t osc_port, uint16_t ws_port);
  ~oscquery_server_protocol();

  bool pull(net::address_base&) override;
  std::future<void> pullAsync(net::address_base&) override;
  void request(net::address_base&) override;
  bool push(const net::address_base&) override;
  bool observe(net::address_base&, bool) override;
  bool observe_quietly(net::address_base&, bool) override;
  bool update(net::node_base& b) override;
  void setDevice(net::device_base& dev) override;
  ossia::net::device_base& getDevice() const { return *m_device; }

  int getOSCPort() const { return m_oscPort; }

  Nano::Signal<void(const std::string&)> onClientConnected;
  Nano::Signal<void(const std::string&)> onClientDisconnected;
private:
  // List of connected clients
  oscquery_client* findClient(const connection_handler& hdl);

  // List of clients who connected but did not finish the connection procedure
  oscquery_client* findBuildingClient(const connection_handler& hdl);

  // Called when a client is built (it gave its osc port)
  void enableClient(const connection_handler& hdl);

  void add_node(ossia::string_view path, const string_map<std::string>& parameters);
  void remove_node(ossia::string_view path, const std::string& node);

  // OSC callback
  void on_OSCMessage(
      const oscpack::ReceivedMessage& m,
      const oscpack::IpEndpointName& ip);

  // Websocket callbacks
  void on_connectionOpen(connection_handler hdl);
  void on_connectionClosed(connection_handler hdl);

  // Local device callback
  void on_nodeCreated(const ossia::net::node_base&);
  void on_nodeRemoved(const ossia::net::node_base&);
  void on_attributeChanged(const ossia::net::node_base&, ossia::string_view attr);

  void update_zeroconf();
  // Exceptions here will be catched by the server
  // which will set appropriate error codes.
  rapidjson::StringBuffer on_WSrequest(
      connection_handler hdl,
      const std::string& message);

  std::unique_ptr<osc::receiver> m_oscServer;
  websocket_server m_websocketServer;

  net::zeroconf_server m_zeroconfServer;

  // Listening status of the local software
  net::listened_addresses m_listening;

  // The clients connected to this server
  std::vector<oscquery_client> m_buildingClients;
  std::vector<oscquery_client> m_clients;

  ossia::net::device_base* m_device{};

  // Where the websocket server lives
  std::thread m_serverThread;

  // To lock m_clients
  mutex_t m_buildingClientsMutex;
  mutex_t m_clientsMutex;

  // The local ports
  uint16_t m_oscPort{};
  uint16_t m_wsPort{};

};

}
}
