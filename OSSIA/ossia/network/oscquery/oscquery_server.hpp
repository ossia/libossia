#pragma once
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/detail/server.hpp>
#include <readerwriterqueue.h>

#include <ossia/detail/mutex.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>
#include <atomic>
#include <hopscotch_map.h>
#include <nano_signal_slot.hpp>
namespace osc
{

template <typename T>
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
class oscquery_client;
//! Implementation of an oscquery server.
class OSSIA_EXPORT oscquery_server_protocol final
    : public ossia::net::protocol_base
{
  friend class query_answerer;
  friend class get_query_answerer;
  friend struct json_query_answerer;

public:
  using connection_handler = websocket_server::connection_handler;
  oscquery_server_protocol(uint16_t osc_port, uint16_t ws_port);
  ~oscquery_server_protocol();

  bool pull(net::address_base&) override;
  std::future<void> pull_async(net::address_base&) override;
  void request(net::address_base&) override;
  bool push(const net::address_base&) override;
  bool push_raw(const ossia::net::full_address_data& address_base) override;
  bool push_bundle(const std::vector<const ossia::net::address_base*>&) override;
  bool push_raw_bundle(const std::vector<ossia::net::full_address_data>&) override;
  bool observe(net::address_base&, bool) override;
  bool observe_quietly(net::address_base&, bool) override;
  bool update(net::node_base& b) override;
  void set_device(net::device_base& dev) override;
  ossia::net::device_base& get_device() const
  {
    return *m_device;
  }

  int get_osc_port() const
  {
    return m_oscPort;
  }

  Nano::Signal<void(const std::string&)> onClientConnected;
  Nano::Signal<void(const std::string&)> onClientDisconnected;

private:
  // List of connected clients
  oscquery_client* find_client(const connection_handler& hdl);

  // List of clients who connected but did not finish the connection procedure
  oscquery_client* find_building_client(const connection_handler& hdl);

  // Called when a client is built (it gave its osc port)
  void enable_client(const connection_handler& hdl);

  void add_node(
      ossia::string_view path,
      const string_map<std::string>& parameters);
  void remove_node(
      ossia::string_view path,
      const std::string& node);

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

  template<typename T>
  bool push_impl(const T& addr);

  void update_zeroconf();
  // Exceptions here will be catched by the server
  // which will set appropriate error codes.
  rapidjson::StringBuffer
  on_WSrequest(connection_handler hdl, const std::string& message);

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
