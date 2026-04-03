#include <ossia/network/sockets/websocket_server_beast.hpp>

#include <ossia/detail/json.hpp>

#include <boost/asio/ip/v6_only.hpp>

namespace ossia::net
{

/// beast_ws_connection implementation

beast_ws_connection::beast_ws_connection(
    boost::asio::ip::tcp::socket&& socket, websocket_server_beast& server)
    : m_ws{std::move(socket)}
    , m_server{server}
{
  auto& tcp = boost::beast::get_lowest_layer(m_ws);
  tcp.expires_never();
  boost::asio::ip::tcp::no_delay opt(true);
  try
  {
    tcp.socket().set_option(opt);
  }
  catch(...)
  {
  }
}

beast_ws_connection::~beast_ws_connection() = default;

void beast_ws_connection::run()
{
  // Start by reading an HTTP request to determine if this is
  // a WebSocket upgrade or a plain HTTP request.
  do_read_http();
}

void beast_ws_connection::send_text(const std::string& msg)
{
  m_ws.text(true);
  boost::beast::error_code ec;
  m_ws.write(boost::asio::buffer(msg), ec);
}

void beast_ws_connection::send_text(const char* data, std::size_t sz)
{
  m_ws.text(true);
  boost::beast::error_code ec;
  m_ws.write(boost::asio::buffer(data, sz), ec);
}

void beast_ws_connection::send_binary(std::string_view msg)
{
  m_ws.binary(true);
  boost::beast::error_code ec;
  m_ws.write(boost::asio::buffer(msg.data(), msg.size()), ec);
}

void beast_ws_connection::close()
{
  boost::beast::error_code ec;
  if(m_ws.is_open())
    m_ws.close(boost::beast::websocket::close_code::going_away, ec);
}

boost::asio::ip::tcp::socket& beast_ws_connection::tcp_socket()
{
  return boost::beast::get_lowest_layer(m_ws).socket();
}

void beast_ws_connection::do_read_http()
{
  m_buffer.clear();
  m_http_req = {};

  boost::beast::http::async_read(
      boost::beast::get_lowest_layer(m_ws), m_buffer, m_http_req,
      [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes) {
    self->on_read_http(ec, bytes);
  });
}

void beast_ws_connection::on_read_http(boost::beast::error_code ec, std::size_t)
{
  if(ec)
    return;

  // Check if this is a WebSocket upgrade request
  if(boost::beast::websocket::is_upgrade(m_http_req))
  {
    m_is_websocket = true;
    do_accept_ws();
    return;
  }

  // Plain HTTP request — handle it like websocketpp did:
  // treat as a TEXT message with the URI as payload, send back the reply.
  if(m_server.m_on_message)
  {
    try
    {
      auto resource = std::string(m_http_req.target());
      auto reply = m_server.m_on_message(
          shared_from_this(), ws_opcode::text, resource);

      namespace http = boost::beast::http;
      http::response<http::string_body> res{http::status::ok, m_http_req.version()};
      res.set(http::field::server, "ossia");
      res.set(http::field::access_control_allow_origin, "*");
      res.set(http::field::connection, "close");

      switch(reply.type)
      {
        case server_reply::data_type::json:
          res.set(http::field::content_type, "application/json; charset=utf-8");
          reply.data += '\0';
          break;
        case server_reply::data_type::html:
          res.set(http::field::content_type, "text/html; charset=utf-8");
          break;
        default:
          break;
      }

      res.body() = std::move(reply.data);
      res.prepare_payload();

      boost::beast::error_code write_ec;
      http::write(boost::beast::get_lowest_layer(m_ws), res, write_ec);
    }
    catch(const std::exception& e)
    {
      namespace http = boost::beast::http;
      http::response<http::string_body> res{
          http::status::internal_server_error, m_http_req.version()};
      res.set(http::field::server, "ossia");
      res.set(http::field::connection, "close");
      res.prepare_payload();

      boost::beast::error_code write_ec;
      http::write(boost::beast::get_lowest_layer(m_ws), res, write_ec);
    }
  }

  // HTTP connection: close after response (no keep-alive for OSCQuery)
  boost::beast::error_code close_ec;
  boost::beast::get_lowest_layer(m_ws).socket().shutdown(
      boost::asio::ip::tcp::socket::shutdown_send, close_ec);
}

void beast_ws_connection::do_accept_ws()
{
  m_ws.set_option(
      boost::beast::websocket::stream_base::timeout::suggested(
          boost::beast::role_type::server));

  m_ws.async_accept(
      m_http_req,
      [self = shared_from_this()](boost::beast::error_code ec) {
    self->on_accept_ws(ec);
  });
}

void beast_ws_connection::on_accept_ws(boost::beast::error_code ec)
{
  if(ec)
    return;

  // Notify server of new connection
  m_server.add_connection(shared_from_this());

  // Start reading messages
  do_read_ws();
}

void beast_ws_connection::do_read_ws()
{
  m_buffer.clear();
  m_ws.async_read(
      m_buffer,
      [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes) {
    self->on_read_ws(ec, bytes);
  });
}

void beast_ws_connection::on_read_ws(boost::beast::error_code ec, std::size_t)
{
  if(ec)
  {
    // Connection closed or error
    m_server.remove_connection(shared_from_this());
    return;
  }

  if(m_server.m_on_message)
  {
    auto opcode = m_ws.got_text() ? ws_opcode::text : ws_opcode::binary;
    auto data = boost::beast::buffers_to_string(m_buffer.data());

    try
    {
      auto reply = m_server.m_on_message(shared_from_this(), opcode, data);

      if(!reply.data.empty())
      {
        switch(reply.type)
        {
          case server_reply::data_type::json:
          case server_reply::data_type::html:
            send_text(reply.data);
            break;
          default:
            send_binary(reply.data);
            break;
        }
      }
    }
    catch(const std::exception& e)
    {
      ossia::logger().error("Error in WS message handling: {}", e.what());
    }
    catch(...)
    {
      ossia::logger().error("Error in WS message handling");
    }
  }

  // Continue reading
  do_read_ws();
}

/// websocket_server_beast implementation

websocket_server_beast::websocket_server_beast(ossia::net::network_context_ptr ctx)
    : m_context{std::move(ctx)}
    , m_acceptor{boost::asio::make_strand(m_context->context)}
{
}

websocket_server_beast::~websocket_server_beast()
{
  stop();
}

void websocket_server_beast::listen(uint16_t port)
{
  // Listen on IPv6 with dual-stack (accepts both IPv4 and IPv6)
  boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::tcp::v6(), port};

  boost::beast::error_code ec;
  m_acceptor.open(endpoint.protocol(), ec);
  if(ec)
    return;

  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if(ec)
    return;

  // Allow both IPv4 and IPv6 connections on this socket
  m_acceptor.set_option(boost::asio::ip::v6_only(false), ec);
  // Not fatal if this fails (some OS don't support dual-stack)

  m_acceptor.bind(endpoint, ec);
  if(ec)
  {
    // Fallback to IPv4-only if dual-stack bind fails
    m_acceptor.close(ec);
    endpoint = boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), port};
    m_acceptor.open(endpoint.protocol(), ec);
    if(ec)
      return;
    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    m_acceptor.bind(endpoint, ec);
    if(ec)
      return;
  }

  m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if(ec)
    return;

  do_accept();
}

void websocket_server_beast::run()
{
  m_context->context.run();
}

void websocket_server_beast::stop()
{
  boost::beast::error_code ec;
  if(m_acceptor.is_open())
    m_acceptor.close(ec);

  std::lock_guard lock{m_connections_mutex};
  for(auto& conn : m_connections)
    conn->close();
  m_connections.clear();
}

void websocket_server_beast::close(ws_connection_handle hdl)
{
  if(auto conn = find_connection(hdl))
    conn->close();
}

void websocket_server_beast::set_open_handler(ws_open_handler h)
{
  m_on_open = std::move(h);
}

void websocket_server_beast::set_close_handler(ws_close_handler h)
{
  m_on_close = std::move(h);
}

void websocket_server_beast::set_message_handler(ws_server_message_handler h)
{
  m_on_message = std::move(h);
}

std::string websocket_server_beast::get_remote_ip(const ws_connection_handle& hdl)
{
  if(auto conn = find_connection(hdl))
  {
    try
    {
      return conn->tcp_socket().remote_endpoint().address().to_string();
    }
    catch(...)
    {
    }
  }
  return {};
}

std::string websocket_server_beast::get_remote_endpoint(const ws_connection_handle& hdl)
{
  if(auto conn = find_connection(hdl))
  {
    try
    {
      auto ep = conn->tcp_socket().remote_endpoint();
      return ep.address().to_string() + ":" + std::to_string(ep.port());
    }
    catch(...)
    {
    }
  }
  return {};
}

std::string websocket_server_beast::get_local_ip(const ws_connection_handle& hdl)
{
  if(auto conn = find_connection(hdl))
  {
    try
    {
      return conn->tcp_socket().local_endpoint().address().to_string();
    }
    catch(...)
    {
    }
  }
  return {};
}

void websocket_server_beast::send_message(
    ws_connection_handle hdl, const std::string& message)
{
  if(auto conn = find_connection(hdl))
    conn->send_text(message);
}

void websocket_server_beast::send_message(
    ws_connection_handle hdl, const server_reply& message)
{
  if(auto conn = find_connection(hdl))
  {
    switch(message.type)
    {
      case server_reply::data_type::json:
      case server_reply::data_type::html:
        conn->send_text(message.data);
        break;
      default:
        conn->send_binary(message.data);
        break;
    }
  }
}

void websocket_server_beast::send_message(
    ws_connection_handle hdl, const rapidjson::StringBuffer& message)
{
  if(auto conn = find_connection(hdl))
    conn->send_text(message.GetString(), message.GetSize());
}

void websocket_server_beast::send_binary_message(
    ws_connection_handle hdl, std::string_view message)
{
  if(auto conn = find_connection(hdl))
    conn->send_binary(message);
}

void websocket_server_beast::do_accept()
{
  m_acceptor.async_accept(
      boost::asio::make_strand(m_context->context),
      [this](boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
    on_accept(ec, std::move(socket));
  });
}

void websocket_server_beast::on_accept(
    boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
  if(ec)
    return; // Acceptor was closed

  // Create connection and start reading
  auto conn = std::make_shared<beast_ws_connection>(std::move(socket), *this);
  conn->run();

  // Accept next connection
  do_accept();
}

void websocket_server_beast::add_connection(std::shared_ptr<beast_ws_connection> conn)
{
  ws_connection_handle hdl = conn;
  {
    std::lock_guard lock{m_connections_mutex};
    m_connections.insert(std::move(conn));
  }

  if(m_on_open)
    m_on_open(hdl);
}

void websocket_server_beast::remove_connection(std::shared_ptr<beast_ws_connection> conn)
{
  ws_connection_handle hdl = conn;

  {
    std::lock_guard lock{m_connections_mutex};
    m_connections.erase(conn);
  }

  if(m_on_close)
    m_on_close(hdl);
}

beast_ws_connection*
websocket_server_beast::find_connection(const ws_connection_handle& hdl)
{
  auto sp = hdl.lock();
  if(!sp)
    return nullptr;

  std::lock_guard lock{m_connections_mutex};
  auto it = m_connections.find(
      std::static_pointer_cast<beast_ws_connection>(sp));
  if(it != m_connections.end())
    return it->get();
  return nullptr;
}

}
