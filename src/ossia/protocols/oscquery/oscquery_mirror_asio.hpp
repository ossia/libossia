#pragma once

#include <ossia/detail/json_fwd.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/host_info.hpp>

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
class websocket_client;
struct parameter_data;
struct network_context;
using network_context_ptr = std::shared_ptr<network_context>;
}
namespace oscquery_asio
{
struct osc_outbound_visitor;
struct http_async_client_context;
struct http_responder;

class oscquery_mirror_asio_protocol;
struct oscquery_shared_async_state
{
  oscquery_mirror_asio_protocol& self;
  bool active = true;
};

class OSSIA_EXPORT oscquery_mirror_asio_protocol final : public ossia::net::protocol_base
{
  struct osc_sender_impl;
  struct osc_receiver_impl;

public:
  oscquery_mirror_asio_protocol(
      ossia::net::network_context_ptr ctx, std::string host,
      uint16_t local_osc_port = 0);
  ~oscquery_mirror_asio_protocol() override;

  bool pull(net::parameter_base&) override;
  std::future<void> pull_async(net::parameter_base&) override;
  void request(net::parameter_base&) override;
  bool push(const net::parameter_base&, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&) override;
  bool observe(net::parameter_base&, bool) override;
  bool observe_quietly(net::parameter_base&, bool) override;
  bool update(net::node_base& b) override;
  bool echo_incoming_message(
      const ossia::net::message_origin_identifier& id,
      const ossia::net::parameter_base& addr, const value& val) override;

  std::future<void> update_async(net::node_base& b) override;

  void stop() override;
  void set_device(net::device_base& dev) override;
  ossia::net::device_base& get_device() const { return *m_device; }

  bool ws_connected() const noexcept { return m_hasWS; }
  bool osc_connected() const noexcept { return bool(m_oscSender); }
  osc_sender_impl& osc_sender() const noexcept { return *m_oscSender; }
  ossia::net::websocket_client& ws_client() const noexcept { return *m_websocketClient; }

  /**
   * @brief Request a new node from the server
   * @param Parent of the new node
   * @param Data of the new node
   */
  void request_add_node(net::node_base&, const ossia::net::parameter_data&);

  /**
   * @brief Request a node removal from the server
   * @param Node to be removed
   */
  void request_remove_node(net::node_base&);

  /**
   * @brief Request a node renaming from the server
   * @param Node to be removed and new name
   */
  void request_rename_node(net::node_base& node, const std::string& new_name);

  /**
   * @brief Define behavior when a node is removed : mark it as zombie if true
   * (default), remove it otherwise.
   * @param zombie_on_removed
   */
  void set_zombie_on_remove(bool zombie_on_remove)
  {
    m_zombie_on_remove = zombie_on_remove;
  }

  /**
   * @brief Get zombie on removed move
   * @return
   */
  bool get_zombie_on_remove() const noexcept { return m_zombie_on_remove; }

  ossia::oscquery::host_info get_host_info() const noexcept;

  bool connected() const noexcept override { return m_hasWS; }
  void connect() override;

  void set_feedback(bool fb) override;
private:
  friend struct http_async_answer;
  friend struct http_async_value_answer;
  using connection_handler = std::weak_ptr<void>;
  void on_ws_disconnected() { m_hasWS = false; }

  void init();

  void start_http();
  void start_websockets();
  void start_osc();

  void cleanup_connections();

  // Input
  bool on_text_ws_message(connection_handler hdl, const std::string& message);
  bool on_binary_ws_message(connection_handler hdl, const std::string& message);
  bool on_value_http_message(const std::string& address, const std::string& message);
  void on_osc_message(const oscpack::ReceivedMessage& m);
  void process_raw_osc_data(const char* data, std::size_t sz);

  // Output
  void http_send_message(const std::string& str);
  void http_send_message(const rapidjson::StringBuffer& str);

  void ws_send_message(const std::string& str);
  void ws_send_message(const rapidjson::StringBuffer& str);

  // ZeroConf
  bool query_connected();
  void query_stop();

  void on_nodeRenamed(const ossia::net::node_base& n, std::string oldname);

  ossia::net::network_context_ptr m_ctx;

  std::unique_ptr<osc_sender_impl> m_oscSender;
  std::unique_ptr<osc_receiver_impl> m_oscServer;

  std::unique_ptr<ossia::net::websocket_client> m_websocketClient;
  std::shared_ptr<oscquery_shared_async_state> m_async_state;
  std::atomic_bool m_hasWS{};

  // Listening status of the local software
  net::listened_parameters m_listening;

  ossia::net::device_base* m_device{};

  std::promise<void> m_namespacePromise;

  std::string m_queryHost;
  std::string m_queryPort;
  std::string m_httpHost;
  int m_osc_port{};

  std::unique_ptr<http_async_client_context> m_http;
  ossia::oscquery::host_info m_host_info;

  ossia::net::message_origin_identifier m_id;
  std::atomic<bool> m_feedback{true};
  bool m_zombie_on_remove{true};
  enum
  {
    any_protocol,
    http,
    websockets
  } m_protocol_to_use{any_protocol};
};
}
}
