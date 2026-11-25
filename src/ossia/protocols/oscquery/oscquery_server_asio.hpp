#pragma once
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/detail/mutex.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/sockets/websocket_reply.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

#include <nano_signal_slot.hpp>

#include <atomic>
#include <chrono>
#include <optional>
#include <string>
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
class OSSIA_EXPORT oscquery_server_protocol_base : public ossia::net::protocol_base
{
  friend struct oscquery_client;
  friend class ossia::oscquery::query_answerer;
  friend class ossia::oscquery::get_query_answerer;
  friend struct ossia::oscquery::json_query_answerer;

public:
  using connection_handler = std::weak_ptr<void>;

  // Use oscquery_server_protocol_with_osc
  explicit oscquery_server_protocol_base(
      ossia::net::network_context_ptr ctx, uint16_t, uint16_t)
      = delete;

  oscquery_server_protocol_base(
      ossia::net::network_context_ptr ctx,
      const std::vector<ossia::net::osc_server_configuration>& conf, uint16_t ws_port,
      bool forceWS);
  ~oscquery_server_protocol_base() override;

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
  ossia::net::device_base& get_device() const noexcept { return *m_device; }

  int get_ws_port() const noexcept { return m_wsPort; }

  bool force_ws() const noexcept { return m_forceWS.load(std::memory_order_relaxed); }
  void set_force_ws(bool forceWS) noexcept;

  /**
   * @brief Whether the server knows which client a value comes from.
   *
   * With the echo of its device on, the server forwards a value a client
   * sent to its other clients. On, a client is known by the UDP port its
   * values come from (the LOCAL_SENDER_PORT of its START_OSC_STREAMING) and
   * by its WebSocket connection, and does not get its own value back, as
   * with ossia::oscquery::oscquery_server_protocol.
   *
   * Off by default, as before: no sender is known, and the sender gets its
   * own value back.
   *
   * For hosts where several clients write the same parameters with the echo
   * on, such as the Max, TouchDesigner, Unity and DAW servers of PRISM.
   */
  void set_identify_senders(bool b) noexcept
  {
    m_identifySenders.store(b, std::memory_order_relaxed);
  }
  bool get_identify_senders() const noexcept
  {
    return m_identifySenders.load(std::memory_order_relaxed);
  }

  /**
   * @brief The window within which a client's write crossed a server value.
   *
   * The server sets a value and a client writes another one to the same
   * address in the same instant: the server applies the client's value last,
   * the client applies the server's after its own, and each keeps the
   * other's. With a window, a write that comes less than it after the server
   * sent that client a different value on that address gets the server's
   * final value back, to that client only; a later write, or one that leaves
   * the value as sent, gets nothing, and the answer opens no window.
   *
   * 0, the default, turns it off: the server behaves as before. PRISM's
   * hosts (Max, TouchDesigner, Unity, the DAW servers), where several
   * clients write the same parameters, use 100 ms: a crossed value reached
   * the server 12 ms after it sent its own at most, on one machine, and the
   * rest covers a network and a frame of the host.
   */
  void set_crossing_window(std::chrono::milliseconds window) noexcept
  {
    m_crossingWindow.store(window.count(), std::memory_order_relaxed);
  }
  std::chrono::milliseconds crossing_window() const noexcept
  {
    return std::chrono::milliseconds{m_crossingWindow.load(std::memory_order_relaxed)};
  }

  const std::vector<ossia::net::osc_server_configuration>&
  get_transports() const noexcept
  {
    return m_oscConf;
  }

  Nano::Signal<void(const std::string&)> onClientConnected;
  Nano::Signal<void(const std::string&)> onClientDisconnected;

protected:
  explicit oscquery_server_protocol_base(ossia::net::network_context_ptr ctx) = delete;

  // OSC callback. origin: the client the message comes from, 0 when unknown
  void on_osc_message(const oscpack::ReceivedMessage& m, uintptr_t origin = 0);
  void process_raw_osc_data(const char* data, std::size_t sz, uintptr_t origin = 0);
  // Whether the client a value comes from is looked for
  bool wants_origin() const noexcept;
  // The client whose OSC sender has that UDP port (LOCAL_SENDER_PORT of its
  // START_OSC_STREAMING), 0 if none
  uintptr_t client_of_sender_port(int port);

  // set_crossing_window. A value the server sent on its own (a local change,
  // another client's value it forwards), to every client but skipped
  void record_sent(std::string address, const ossia::value& v, uintptr_t skipped = 0);
  // Before a write of origin to address is applied: the value the server
  // sent it there within the window, if any
  std::optional<ossia::value>
  sent_within_window(std::string_view address, uintptr_t origin);
  // After the write: its answer, when it left the address with another value
  // than sent
  void
  answer_crossing(std::string_view address, uintptr_t origin, const ossia::value& sent);

  // List of connected clients
  oscquery_client* find_client(const connection_handler& hdl);

  void add_node(std::string_view path, const string_map<std::string>& parameters);
  void remove_node(std::string_view path, const std::string& node);
  void rename_node(std::string_view node, const std::string& new_name);

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

  std::unique_ptr<ossia::net::websocket_server> m_websocketServer;

  net::zeroconf_server m_zeroconfServerWS;

  // What we expose over OSC
  std::vector<ossia::net::osc_server_configuration> m_oscConf;

  // Listening status of the local software
  net::listened_parameters m_listening;

  // The clients connected to this server
  clients m_clients;
  std::atomic_int m_clientCount{};

  ossia::net::device_base* m_device{};

  // To lock m_clients
  mutex_t m_clientsMutex;

  // The local ports
  uint16_t m_wsPort{};

  // Will only send changes through WS
  std::atomic_bool m_forceWS{};

  // set_identify_senders
  std::atomic_bool m_identifySenders{};

  // set_crossing_window: by address, the last value the server sent on its
  // own
  struct sent_value
  {
    std::chrono::steady_clock::time_point time;
    ossia::value value;
    uintptr_t skipped{};
  };
  mutex_t m_sentMutex;
  ossia::hash_map<std::string, sent_value> m_sent;
  std::atomic<int64_t> m_crossingWindow{0};
};

class OSSIA_EXPORT oscquery_server_protocol : public oscquery_server_protocol_base
{
public:
  struct osc_receiver_impl;
  explicit oscquery_server_protocol(
      ossia::net::network_context_ptr ctx, uint16_t osc_port = 1234,
      uint16_t ws_port = 5678, bool forceWS = false);
  ~oscquery_server_protocol();

  void update_zeroconf();
  void set_device(net::device_base& dev) override;
  void stop() override;
  int get_osc_port() const noexcept { return m_oscPort; }

private:
  uint16_t m_oscPort{};
  std::unique_ptr<osc_receiver_impl> m_oscServer;

  net::zeroconf_server m_zeroconfServerOSC;
};
}
}
