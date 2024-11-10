#pragma once

#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/protocols/dense/dense_protocol.hpp>
#include <ossia/protocols/oscquery/oscquery_fwd.hpp>

#include <span>

namespace ossia::oscquery_asio
{
class oscquery_mirror_asio_protocol_dense;
struct oscquery_dense_shared_async_state
{
  oscquery_mirror_asio_protocol_dense& self;
  bool active = true;
};

class OSSIA_EXPORT oscquery_mirror_asio_protocol_dense final
    : public ossia::net::protocol_base
{
  struct osc_sender_impl;
  struct osc_receiver_impl;
  using async_state = oscquery_dense_shared_async_state;

public:
  oscquery_mirror_asio_protocol_dense(
      ossia::net::network_context_ptr ctx, std::string host,
      uint16_t local_osc_port = 0);
  ~oscquery_mirror_asio_protocol_dense() override;

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

  ossia::oscquery::host_info get_host_info() const noexcept;

  void connect() override;

  void set_feedback(bool fb) override;
private:
  void update_function();
  void recompute_nodes();
  int write_packet(std::span<unsigned char> data);

  friend struct http_async_answer<async_state>;
  friend struct http_async_value_answer<async_state>;
  using http_async_request = ossia::oscquery_asio::http_async_request<async_state>;
  using http_async_value_request
      = ossia::oscquery_asio::http_async_value_request<async_state>;
  using http_async_answer = ossia::oscquery_asio::http_async_answer<async_state>;
  using http_async_value_answer
      = ossia::oscquery_asio::http_async_value_answer<async_state>;
  using connection_handler = std::weak_ptr<void>;

  void init();

  void start_http();
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

  // ZeroConf
  bool query_connected();
  void query_stop();

  void on_nodeRenamed(const ossia::net::node_base& n, std::string oldname);

  ossia::net::network_context_ptr m_ctx;

  std::unique_ptr<osc_receiver_impl> m_oscServer;

  std::shared_ptr<oscquery_dense_shared_async_state> m_async_state;
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

  ossia::timer m_timer;

  ossia::flat_set<ossia::net::parameter_base*, ossia::net::parameter_alphabetical_sort>
      m_sorted_params;

  std::optional<ossia::net::udp_send_socket> to_client;
  ossia::net::message_origin_identifier m_id;
  std::atomic<bool> m_feedback{true};
  bool m_zombie_on_remove{true};
};
}
