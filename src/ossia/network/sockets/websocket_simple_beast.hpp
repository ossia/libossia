#pragma once
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/websocket_client_beast.hpp>
#include <ossia/network/sockets/websocket_server_beast.hpp>

namespace ossia::net
{

/// Duck-typed simple websocket client for use with osc_generic_client_protocol.
/// Replaces websocket_simple_client (wspp-based) with beast-based implementation.
struct websocket_simple_client_beast
{
  websocket_simple_client_beast(
      const ws_client_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_host{conf.url}
  {
  }

  void connect() { }

  template <typename F>
  void receive(F onMessage)
  {
    m_client = std::make_unique<websocket_client_beast>(
        m_context,
        [handler = std::move(onMessage)](
            const ws_connection_handle&, ws_opcode, std::string& data) {
      handler((const unsigned char*)data.data(), data.size());
    });

    m_client->connect(m_host);
  }

  void write(const char* data, std::size_t sz)
  {
    if(m_client)
      m_client->send_binary_message(std::string_view{data, sz});
  }

  void close()
  {
    if(m_client && m_client->connected())
      m_client->stop();
  }

  bool connected() const { return m_client && m_client->connected(); }

  Nano::Signal<void()> on_open;
  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;

  boost::asio::io_context& m_context;
  std::string m_host;
  std::unique_ptr<websocket_client_beast> m_client;
};

/// Duck-typed simple websocket server for use with osc_generic_server_protocol.
/// Replaces websocket_simple_server (wspp-based) with beast-based implementation.
struct websocket_simple_server_beast
{
  websocket_simple_server_beast(
      const ws_server_configuration& conf, ossia::net::network_context_ptr ctx)
      : m_server{std::move(ctx)}
      , m_port{conf.port}
  {
    m_server.set_open_handler(
        [this](ws_connection_handle hdl) { m_listeners.push_back(hdl.lock()); });
    m_server.set_close_handler([this](ws_connection_handle hdl) {
      ossia::remove_erase(m_listeners, hdl.lock());
    });
  }

  template <typename F>
  void listen(F onMessage)
  {
    m_server.set_message_handler(
        [handler = std::move(onMessage)](
            const ws_connection_handle&, ws_opcode, const std::string& data) {
      handler((const unsigned char*)data.data(), data.size());
      return server_reply{};
    });

    m_server.listen(m_port);
  }

  void write(const char* data, std::size_t sz)
  {
    std::string_view dat{data, sz};
    for(auto& listener : m_listeners)
      m_server.send_binary_message(listener, dat);
  }

  void close() { m_server.stop(); }

  websocket_server_beast m_server;
  std::vector<std::shared_ptr<void>> m_listeners;
  int m_port{};
};

}
