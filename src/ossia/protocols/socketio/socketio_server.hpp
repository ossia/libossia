#pragma once

#include <ossia/network/context.hpp>
#include <ossia/network/sockets/websocket_server_interface.hpp>
#include <ossia/protocols/socketio/socketio_session.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
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

class socketio_server;

/// A single client connection managed by the Socket.IO server.
/// Handles the Engine.IO handshake, polling transport, WebSocket upgrade,
/// and Socket.IO packet framing.
class socketio_server_connection
    : public std::enable_shared_from_this<socketio_server_connection>
{
  friend class socketio_server;

public:
  socketio_server_connection(
      boost::asio::ip::tcp::socket&& socket, socketio_server& server);
  ~socketio_server_connection();

  void run();
  void send_text(const std::string& msg);
  void send_text(const char* data, std::size_t sz);
  void send_binary(std::string_view msg);
  void close();

  boost::asio::ip::tcp::socket& tcp_socket();
  const std::string& sid() const { return m_config.sid; }

private:
  void do_read_http();
  void on_read_http(boost::beast::error_code ec, std::size_t bytes);

  // HTTP polling handlers
  void handle_polling_get(
      boost::beast::http::request<boost::beast::http::string_body>& req);
  void handle_polling_post(
      boost::beast::http::request<boost::beast::http::string_body>& req);
  void handle_new_connection(
      boost::beast::http::request<boost::beast::http::string_body>& req);
  void send_http_response(
      unsigned version, boost::beast::http::status status,
      const std::string& body, const std::string& content_type = "text/plain");

  // WebSocket mode
  void do_accept_ws(
      boost::beast::http::request<boost::beast::http::string_body> req);
  void on_accept_ws(boost::beast::error_code ec);
  void do_read_ws();
  void on_read_ws(boost::beast::error_code ec, std::size_t bytes);

  // Ping timer
  void start_ping_timer();
  void on_ping_timer(boost::beast::error_code ec);

  // Engine.IO / Socket.IO processing
  void process_engineio_message(std::string_view data);
  void dispatch_socketio_message(std::string_view data);

  static std::string generate_sid();

  boost::beast::websocket::stream<boost::beast::tcp_stream> m_ws;
  boost::beast::flat_buffer m_buffer;
  boost::beast::http::request<boost::beast::http::string_body> m_http_req;
  boost::asio::steady_timer m_ping_timer;
  socketio_server& m_server;
  engineio_config m_config;

  // Buffered messages for long-polling GET
  std::vector<std::string> m_poll_buffer;

  bool m_is_websocket{false};
  bool m_handshake_done{false};
  bool m_upgraded{false};
};

/// Socket.IO server implementing websocket_server_interface.
///
/// Handles Engine.IO v4 handshake (HTTP polling + WebSocket upgrade),
/// Socket.IO v5 CONNECT, ping/pong, and message framing.
class OSSIA_EXPORT socketio_server final : public websocket_server_interface
{
  friend class socketio_server_connection;

public:
  explicit socketio_server(ossia::net::network_context_ptr ctx);
  ~socketio_server() override;

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
  void add_connection(std::shared_ptr<socketio_server_connection> conn);
  void remove_connection(std::shared_ptr<socketio_server_connection> conn);
  socketio_server_connection* find_connection(const ws_connection_handle& hdl);

  /// Wrap a text message as Socket.IO EVENT and send
  void send_sio_text(socketio_server_connection* conn, const std::string& msg);
  /// Wrap binary data as Socket.IO BINARY_EVENT and send
  void send_sio_binary(socketio_server_connection* conn, std::string_view msg);

  ossia::net::network_context_ptr m_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  std::set<std::shared_ptr<socketio_server_connection>> m_connections;
  std::mutex m_connections_mutex;

  ws_open_handler m_on_open;
  ws_close_handler m_on_close;
  ws_server_message_handler m_on_message;
};

}
