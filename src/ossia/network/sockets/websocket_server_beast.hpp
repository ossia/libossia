#pragma once
#include "websocket_reply.hpp"

#include <ossia/network/context.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
namespace ossia::net
{

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

// Forward declaration
class websocket_shared_state;

/** Represents an active WebSocket connection to the server
*/
class websocket_ws_session : public boost::enable_shared_from_this<websocket_ws_session>
{
  beast::flat_buffer buffer_;
  websocket::stream<beast::tcp_stream> ws_;
  boost::shared_ptr<websocket_shared_state> state_;
  std::vector<boost::shared_ptr<std::string const>> queue_;

  void fail(beast::error_code ec, char const* what);
  void on_accept(beast::error_code ec);
  void on_read(beast::error_code ec, std::size_t bytes_transferred);
  void on_write(beast::error_code ec, std::size_t bytes_transferred);

public:
  websocket_ws_session(
      tcp::socket&& socket, boost::shared_ptr<websocket_shared_state> const& state);

  ~websocket_ws_session();

  template <class Body, class Allocator>
  void run(http::request<Body, http::basic_fields<Allocator>> req);

  // Send a message
  void send(boost::shared_ptr<std::string const> const& ss);

private:
  void on_send(boost::shared_ptr<std::string const> const& ss);
};

class websocket_http_session
    : public boost::enable_shared_from_this<websocket_http_session>
{
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  boost::shared_ptr<websocket_shared_state> state_;

  // The parser is stored in an optional container so we can
  // construct it from scratch it at the beginning of each new message.
  boost::optional<http::request_parser<http::string_body>> parser_;

  struct send_lambda;

  void fail(beast::error_code ec, char const* what);
  void do_read();
  void on_read(beast::error_code ec, std::size_t);
  void on_write(beast::error_code ec, std::size_t, bool close);

public:
  websocket_http_session(
      tcp::socket&& socket, boost::shared_ptr<websocket_shared_state> const& state);

  void run();
};

// Accepts incoming connections and launches the sessions
class websocket_listener : public boost::enable_shared_from_this<websocket_listener>
{
  net::io_context& ioc_;
  tcp::acceptor acceptor_;
  boost::shared_ptr<websocket_shared_state> state_;

  void fail(beast::error_code ec, char const* what);
  void on_accept(beast::error_code ec, tcp::socket socket);

public:
  websocket_listener(
      net::io_context& ioc, tcp::endpoint endpoint,
      boost::shared_ptr<websocket_shared_state> const& state);

  // Start accepting incoming connections
  void run();
};

struct websocket_shared_state
    : public std::enable_shared_from_this<websocket_shared_state>
{
  std::function<void()> on_init;
  std::function<void()> on_open;
  std::function<void()> on_close;
  std::function<void()> on_message;

  void join(void*);
  void leave(void*);
  void send(std::string);
};

class websocket_server_beast
{
public:
  explicit websocket_server_beast(ossia::net::network_context_ptr ctx)
      : m_context{ctx}
      , m_state{boost::make_shared<websocket_shared_state>()}
  {
  }

  void listen(uint16_t port = 9002)
  {
    auto endpoint = boost::asio::ip::tcp::endpoint(
        boost::asio::ip::tcp::v4(), static_cast<unsigned short>(port));
    m_server
        = std::make_shared<websocket_listener>(m_context->context, endpoint, m_state);
    m_server->run();
  }

  void stop() { m_server.reset(); }

  template <typename Handler>
  void set_open_handler(Handler h)
  {
    m_state->on_open = h;
  }

  template <typename Handler>
  void set_close_handler(Handler h)
  {
    m_state->on_close = h;
  }

  template <typename Handler>
  void set_message_handler(Handler h)
  {
    m_state->on_message = h;
  }

  void close(auto hdl)
  {
#if 0
    auto con = m_server->get_con_from_hdl(hdl);
    con->close(websocketpp::close::status::going_away, "Server shutdown");
#endif
  }

  void send_message(auto hdl, const std::string& message)
  {
#if 0
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message);
#endif
  }

  void send_message(auto hdl, const ossia::net::server_reply& message)
  {
#if 0
    auto con = m_server->get_con_from_hdl(hdl);
    switch(message.type)
    {
      case server_reply::data_type::json:
      case server_reply::data_type::html:
        con->send(message.data, websocketpp::frame::opcode::TEXT);
        break;
      default:
        con->send(message.data, websocketpp::frame::opcode::BINARY);
        break;
    }
#endif
  }

  void send_message(auto hdl, const rapidjson::StringBuffer& message)
  {
#if 0
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message.GetString(), message.GetSize(), websocketpp::frame::opcode::text);
#endif
  }

  void send_binary_message(auto hdl, const std::string& message)
  {
#if 0
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message.data(), message.size(), websocketpp::frame::opcode::binary);
#endif
  }

  void send_binary_message(auto hdl, std::string_view message)
  {
#if 0
    auto con = m_server->get_con_from_hdl(hdl);
    con->send(message.data(), message.size(), websocketpp::frame::opcode::binary);
#endif
  }
  std::shared_ptr<websocket_listener> m_server;
  ossia::net::network_context_ptr m_context;
  boost::shared_ptr<websocket_shared_state> m_state;
};
}
