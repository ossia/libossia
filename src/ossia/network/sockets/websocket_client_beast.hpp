#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/websocket_client_interface.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <atomic>
#include <mutex>
#include <string>

namespace ossia::net
{

/// Beast-based WebSocket client implementing websocket_client_interface.
class OSSIA_EXPORT websocket_client_beast final : public websocket_client_interface
{
public:
  websocket_client_beast(boost::asio::io_context& ctx);

  /// Construct with a message handler callback.
  websocket_client_beast(
      boost::asio::io_context& ctx, ws_client_message_handler handler);

  /// Construct with own io_context and a message handler callback.
  /// The io_context is run internally by connect_and_run().
  explicit websocket_client_beast(ws_client_message_handler handler);

  ~websocket_client_beast() override;

  void connect(const std::string& uri) override;
  void connect_and_run(const std::string& uri) override;
  void stop() override;
  bool connected() const override;

  void send_message(const std::string& request) override;
  void send_message(const rapidjson::StringBuffer& request) override;
  void send_binary_message(std::string_view request) override;

private:
  void do_resolve(const std::string& host, const std::string& port, const std::string& path);
  void on_resolve(
      boost::beast::error_code ec,
      boost::asio::ip::tcp::resolver::results_type results,
      std::string path);
  void on_connect(
      boost::beast::error_code ec,
      boost::asio::ip::tcp::resolver::results_type::endpoint_type ep,
      std::string path);
  void on_handshake(boost::beast::error_code ec);
  void do_read();
  void on_read(boost::beast::error_code ec, std::size_t bytes);

  void parse_uri(
      const std::string& uri, std::string& host, std::string& port,
      std::string& path);

  std::unique_ptr<boost::asio::io_context> m_owned_context;
  boost::asio::io_context& m_context;
  boost::asio::ip::tcp::resolver m_resolver;
  std::unique_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> m_ws;
  boost::beast::flat_buffer m_buffer;

  ws_client_message_handler m_on_message;
  std::string m_host;
  std::mutex m_mutex;
  std::atomic_bool m_open{false};
  std::atomic_bool m_connected{false};
};

}
