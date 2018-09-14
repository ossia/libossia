#pragma once
#include <ossia/detail/mutex.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/detail/server_reply.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <hopscotch_map.h>
#include <nano_signal_slot.hpp>

#include <atomic>
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
class websocket_server;
struct oscquery_client;
//! Implementation of an oscquery server.
class OSSIA_EXPORT oscquery_server_protocol final
    : public ossia::net::protocol_base
{
  friend class query_answerer;
  friend class get_query_answerer;
  friend struct json_query_answerer;

public:
  using connection_handler = std::weak_ptr<void>;
  oscquery_server_protocol(uint16_t osc_port = 1234, uint16_t ws_port = 5678);
  ~oscquery_server_protocol() override;

  /**
   * When the server receives a message, it will propagate it to
   * other clients. The implementation will try to prevent sending
   * the message again to its source.
   */
  bool echo() const
  {
    return m_echo;
  }
  void set_echo(bool b)
  {
    m_echo = b;
  }

  bool pull(net::parameter_base&) override;
  std::future<void> pull_async(net::parameter_base&) override;
  void request(net::parameter_base&) override;
  bool push(const net::parameter_base&) override;
  bool
  push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool
  push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw_bundle(
      const std::vector<ossia::net::full_parameter_data>&) override;
  bool observe(net::parameter_base&, bool) override;
  bool observe_quietly(net::parameter_base&, bool) override;
  bool update(net::node_base& b) override;
  void set_device(net::device_base& dev) override;
  void stop() override;
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

  void
  add_node(ossia::string_view path, const string_map<std::string>& parameters);
  void remove_node(ossia::string_view path, const std::string& node);

  // OSC callback
  void
  on_OSCMessage(const oscpack::ReceivedMessage& m, oscpack::IpEndpointName ip);

  // Websocket callbacks
  void on_connectionOpen(const connection_handler& hdl);
  void on_connectionClosed(const connection_handler& hdl);

  // Local device callback
  void on_nodeCreated(const ossia::net::node_base&);
  void on_nodeRemoved(const ossia::net::node_base&);
  void
  on_attributeChanged(const ossia::net::node_base&, ossia::string_view attr);
  void on_nodeRenamed(const ossia::net::node_base& n, std::string oldname);

  template <typename T>
  bool push_impl(const T& addr);

  void update_zeroconf();
  // Exceptions here will be catched by the server
  // which will set appropriate error codes.
  ossia::oscquery::server_reply
  on_WSrequest(const connection_handler& hdl, const std::string& message);
  ossia::oscquery::server_reply on_BinaryWSrequest(
      const connection_handler& hdl, const std::string& message);

  std::unique_ptr<osc::receiver> m_oscServer;
  std::unique_ptr<websocket_server> m_websocketServer;

  net::zeroconf_server m_zeroconfServerWS;
  net::zeroconf_server m_zeroconfServerOSC;

  // Listening status of the local software
  net::listened_parameters m_listening;

  // The clients connected to this server
  std::vector<oscquery_client> m_clients;

  ossia::net::device_base* m_device{};

  // Where the websocket server lives
  std::thread m_serverThread;

  // To lock m_clients
  mutex_t m_clientsMutex;

  // The local ports
  uint16_t m_oscPort{};
  uint16_t m_wsPort{};

  bool m_echo{};
};
}

class OSSIA_EXPORT oscquery_device
{
public:
  oscquery_device(
      uint16_t osc_port = 1234, uint16_t ws_port = 5678,
      std::string name = "oscquery");

  ~oscquery_device();

  ossia::net::generic_device device;
  ossia::oscquery::oscquery_server_protocol& protocol;
};
}
