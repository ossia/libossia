#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/websocket_server_interface.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>

#include <memory>
#include <mutex>
#include <set>
#include <string>

namespace ossia::net
{

class websocket_server_beast;

/// A single WebSocket connection managed by the beast server.
class beast_ws_connection : public std::enable_shared_from_this<beast_ws_connection>
{
  friend class websocket_server_beast;

public:
  explicit beast_ws_connection(
      boost::asio::ip::tcp::socket&& socket, websocket_server_beast& server);

  ~beast_ws_connection();

  void run();
  void send_text(const std::string& msg);
  void send_text(const char* data, std::size_t sz);
  void send_binary(std::string_view msg);
  void close();

  boost::asio::ip::tcp::socket& tcp_socket();

private:
  void do_read_http();
  void on_read_http(boost::beast::error_code ec, std::size_t bytes);
  void do_accept_ws();
  void on_accept_ws(boost::beast::error_code ec);
  void do_read_ws();
  void on_read_ws(boost::beast::error_code ec, std::size_t bytes);

  boost::beast::websocket::stream<boost::beast::tcp_stream> m_ws;
  boost::beast::flat_buffer m_buffer;
  boost::beast::http::request<boost::beast::http::string_body> m_http_req;
  websocket_server_beast& m_server;
  bool m_is_websocket{false};
};

/// Beast-based WebSocket + HTTP server implementing websocket_server_interface.
class OSSIA_EXPORT websocket_server_beast final : public websocket_server_interface
{
  friend class beast_ws_connection;

public:
  explicit websocket_server_beast(ossia::net::network_context_ptr ctx);
  ~websocket_server_beast() override;

  void listen(uint16_t port) override;
  void run() override;
  void stop() override;
  void close(ws_connection_handle hdl) override;

  void set_open_handler(ws_open_handler h) override;
  void set_close_handler(ws_close_handler h) override;
  void set_message_handler(ws_server_message_handler h) override;

  std::string get_remote_ip(const ws_connection_handle& hdl) override;
  std::string get_remote_endpoint(const ws_connection_handle& hdl) override;
  std::string get_local_ip(const ws_connection_handle& hdl) override;

  void send_message(ws_connection_handle hdl, const std::string& message) override;
  void send_message(ws_connection_handle hdl, const server_reply& message) override;
  void
  send_message(ws_connection_handle hdl, const rapidjson::StringBuffer& message) override;
  void send_binary_message(ws_connection_handle hdl, std::string_view message) override;

private:
  void do_accept();
  void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
  void add_connection(std::shared_ptr<beast_ws_connection> conn);
  void remove_connection(std::shared_ptr<beast_ws_connection> conn);

  beast_ws_connection* find_connection(const ws_connection_handle& hdl);

  ossia::net::network_context_ptr m_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  std::set<std::shared_ptr<beast_ws_connection>> m_connections;
  std::mutex m_connections_mutex;

  ws_open_handler m_on_open;
  ws_close_handler m_on_close;
  ws_server_message_handler m_on_message;
};

}
