#pragma once

#include <ossia/network/sockets/websocket_client_interface.hpp>
#include <ossia/protocols/socketio/socketio_session.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>

#include <atomic>
#include <mutex>
#include <thread>

namespace ossia::net
{

/// Socket.IO client implementing websocket_client_interface.
///
/// Performs the Engine.IO v4 handshake (HTTP polling followed by WebSocket upgrade)
/// and the Socket.IO v5 CONNECT, then presents incoming Socket.IO
/// EVENT/BINARY_EVENT messages through the standard ws_client_message_handler
/// callback interface. Outgoing send_message/send_binary_message calls
/// are wrapped into Socket.IO EVENT/BINARY_EVENT packets.
class OSSIA_EXPORT socketio_client final : public websocket_client_interface
{
public:
  explicit socketio_client(boost::asio::io_context& ctx);
  socketio_client(boost::asio::io_context& ctx, ws_client_message_handler handler);

  ~socketio_client() override;

  void connect(const std::string& uri) override;
  void connect_and_run(const std::string& uri) override;
  void stop() override;
  bool connected() const override;

  void send_message(const std::string& request) override;
  void send_message(const rapidjson::StringBuffer& request) override;
  void send_binary_message(std::string_view request) override;

private:
  using websocket_type = boost::beast::websocket::stream<boost::beast::tcp_stream>;
  template <typename T>
  using awaitable = boost::asio::awaitable<T>;

  void parse_uri(
      const std::string& uri, std::string& host, std::string& port,
      std::string& path);

  // Coroutine-based session management
  awaitable<void> run_session(std::string host, std::string port, std::string path);

  awaitable<std::string> http_request(
      boost::beast::http::verb v, boost::beast::tcp_stream& stream,
      std::string_view target, std::string_view body = "");

  awaitable<void> read_loop();
  awaitable<void> write_loop();
  awaitable<void> write_pong();
  awaitable<void> close_session();

  bool process_engineio(std::string_view data);
  void dispatch_socketio_message(std::string_view data);

  void enqueue_write(std::string msg);

  void co_spawn_detached(auto&& f)
  {
    boost::asio::co_spawn(
        m_context, std::move(f),
        [](std::exception_ptr e) {
      if(e)
      {
        try { std::rethrow_exception(e); }
        catch(const std::exception& ex) {
          ossia::logger().error("socketio_client error: {}", ex.what());
        }
        catch(...) {
          ossia::logger().error("socketio_client: unknown error");
        }
      }
    });
  }

  boost::asio::io_context& m_context;
  std::unique_ptr<websocket_type> m_ws;
  boost::beast::flat_buffer m_read_buffer;
  boost::asio::experimental::channel<void(boost::system::error_code, const std::string&)>
      m_write_channel;

  engineio_config m_config;
  ws_client_message_handler m_on_message;
  std::string m_host;
  std::mutex m_mutex;
  std::atomic_bool m_open{false};
  std::atomic_bool m_connected{false};
};

}
