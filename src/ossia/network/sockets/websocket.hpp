#pragma once
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/websocket_client.hpp>
#include <ossia/network/sockets/websocket_server.hpp>
/*
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
*/
#include <ossia/detail/algorithms.hpp>

namespace ossia::net
{
struct websocket_simple_client : websocket_client
{
  websocket_simple_client(const ws_client_configuration& conf, boost::asio::io_context& ctx)
      : ossia::net::websocket_client{}
      , m_host{conf.url}
  {
    m_client.init_asio(&ctx);
  }

  void connect()
  {
    websocket_client::connect(m_host);
  }

  template <typename F>
  void receive(F onMessage) {
    m_client.set_message_handler(
        [handler = std::move(onMessage)](
            connection_handler hdl, client_t::message_ptr msg) {
          const auto& data = msg->get_raw_payload();
          handler(data.data(), data.size());
        });
  }

  void write(const char* data, std::size_t sz)
  {
    send_binary_message(std::string_view{data, sz});
  }

  template <typename F>
  void close(F f)
  {
    /*
    m_context.post([this, f] {
      m_port.close();
      f();
    });
    */
  }

  std::string m_host;
};

struct websocket_simple_server: ossia::net::websocket_server
{
  websocket_simple_server(const ws_server_configuration& conf, boost::asio::io_context& ctx)
      : ossia::net::websocket_server{ctx}
      , m_port{conf.port}
  {
    set_open_handler([this] (connection_handler hdl)
    {
      m_listeners.push_back(hdl.lock());
    });
    set_close_handler([this] (connection_handler hdl)
    {
      ossia::remove_erase(m_listeners, hdl.lock());
    });
  }

  template <typename F>
  void listen(F onMessage)
  {
    m_server.set_message_handler(
        [handler = std::move(onMessage)](
            connection_handler hdl, server_t::message_ptr msg) {
          const auto& data = msg->get_raw_payload();
          handler(data.data(), data.size());
        });
    websocket_server::listen(m_port);
  }

  void write(const char* data, std::size_t sz)
  {
    std::string_view dat{data, sz};
    for(auto& listener : m_listeners)
      send_binary_message(listener, dat);
  }

  template <typename F>
  void close(F f)
  {
    /*
    m_context.post([this, f] {
      m_port.close();
      f();
    });
    */
  }

  std::vector<std::shared_ptr<void>> m_listeners;
  int m_port{};
};

}
