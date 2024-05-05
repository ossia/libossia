#pragma once
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/detail/mutex.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/sockets/websocket_reply.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <ossia/detail/hash_map.hpp>

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
namespace net
{
class websocket_server;
}
namespace oscquery
{
class query_answerer;
class get_query_answerer;
struct json_query_answerer;
}
namespace oscquery_asio
{
struct oscquery_client;
using clients = std::vector<std::unique_ptr<oscquery_client>>;
//! Implementation of an oscquery server.
class OSSIA_EXPORT oscquery_server_protocol final : public ossia::net::protocol_base
{
  friend struct oscquery_client;
  friend class ossia::oscquery::query_answerer;
  friend class ossia::oscquery::get_query_answerer;
  friend struct ossia::oscquery::json_query_answerer;

public:
  using connection_handler = std::weak_ptr<void>;
  oscquery_server_protocol(
      ossia::net::network_context_ptr ctx, uint16_t osc_port = 1234,
      uint16_t ws_port = 5678, bool forceWS = false);
  ~oscquery_server_protocol() override;

  bool pull(net::parameter_base&) override;
  std::future<void> pull_async(net::parameter_base&) override;
  void request(net::parameter_base&) override;
  bool push(const net::parameter_base&, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&) override;
  bool echo_incoming_message(
      const ossia::net::message_origin_identifier&, const ossia::net::parameter_base&,
      const ossia::value& v) override;
  bool observe(net::parameter_base&, bool) override;
  bool observe_quietly(net::parameter_base&, bool) override;
  bool update(net::node_base& b) override;

  void set_device(net::device_base& dev) override;
  void stop() override;
  ossia::net::device_base& get_device() const { return *m_device; }

  int get_osc_port() const { return m_oscPort; }

  int get_ws_port() const { return m_wsPort; }

  Nano::Signal<void(const std::string&)> onClientConnected;
  Nano::Signal<void(const std::string&)> onClientDisconnected;

private:
  // List of connected clients
  oscquery_client* find_client(const connection_handler& hdl);

  void add_node(std::string_view path, const string_map<std::string>& parameters);
  void remove_node(std::string_view path, const std::string& node);
  void rename_node(std::string_view node, const std::string& new_name);

  // OSC callback
  void on_osc_message(const oscpack::ReceivedMessage& m);
  void process_raw_osc_data(const char* data, std::size_t sz);

  // Websocket callbacks
  void on_connectionOpen(const connection_handler& hdl);
  void on_connectionClosed(const connection_handler& hdl);

  // Local device callback
  void on_nodeCreated(const ossia::net::node_base&);
  void on_nodeRemoved(const ossia::net::node_base&);
  void on_parameterChanged(const ossia::net::parameter_base&);
  void on_attributeChanged(const ossia::net::node_base&, std::string_view attr);
  void on_nodeRenamed(const ossia::net::node_base& n, std::string oldname);

  template <typename T>
  bool push_impl(const T& addr, const ossia::value& v);

  bool write_impl(std::string_view data, bool critical);

  void update_zeroconf();
  // Exceptions here will be catched by the server
  // which will set appropriate error codes.
  ossia::net::server_reply
  on_text_ws_message(const connection_handler& hdl, const std::string& message);
  ossia::net::server_reply
  on_binary_ws_message(const connection_handler& hdl, const std::string& message);

  ossia::net::network_context_ptr m_context;

  struct osc_receiver_impl;
  std::unique_ptr<osc_receiver_impl> m_oscServer;
  std::unique_ptr<ossia::net::websocket_server> m_websocketServer;

  net::zeroconf_server m_zeroconfServerWS;
  net::zeroconf_server m_zeroconfServerOSC;

  // Listening status of the local software
  net::listened_parameters m_listening;

  // The clients connected to this server
  clients m_clients;
  std::atomic_int m_clientCount{};

  ossia::net::device_base* m_device{};

  // To lock m_clients
  mutex_t m_clientsMutex;

  // The local ports
  uint16_t m_oscPort{};
  uint16_t m_wsPort{};

  // Will only send changes through WS
  bool m_forceWS{};
};
}
}
