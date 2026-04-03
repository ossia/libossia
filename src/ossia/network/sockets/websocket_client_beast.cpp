#include <ossia/network/sockets/websocket_client_beast.hpp>

#include <ossia/detail/json.hpp>

namespace ossia::net
{

websocket_client_beast::websocket_client_beast(boost::asio::io_context& ctx)
    : m_context{ctx}
    , m_resolver{boost::asio::make_strand(ctx)}
{
}

websocket_client_beast::websocket_client_beast(
    boost::asio::io_context& ctx, ws_client_message_handler handler)
    : m_context{ctx}
    , m_resolver{boost::asio::make_strand(ctx)}
    , m_on_message{std::move(handler)}
{
}

websocket_client_beast::websocket_client_beast(ws_client_message_handler handler)
    : m_owned_context{std::make_unique<boost::asio::io_context>()}
    , m_context{*m_owned_context}
    , m_resolver{boost::asio::make_strand(*m_owned_context)}
    , m_on_message{std::move(handler)}
{
}

websocket_client_beast::~websocket_client_beast()
{
  if(m_open)
    stop();
}

void websocket_client_beast::parse_uri(
    const std::string& uri, std::string& host, std::string& port,
    std::string& path)
{
  // Parse ws://host:port/path or ws://host/path
  std::string_view sv = uri;

  // Remove scheme
  if(sv.starts_with("ws://"))
    sv.remove_prefix(5);
  else if(sv.starts_with("wss://"))
    sv.remove_prefix(6);

  // Find path
  auto path_pos = sv.find('/');
  if(path_pos != std::string_view::npos)
  {
    path = std::string(sv.substr(path_pos));
    sv = sv.substr(0, path_pos);
  }
  else
  {
    path = "/";
  }

  // Find port
  auto port_pos = sv.find(':');
  if(port_pos != std::string_view::npos)
  {
    host = std::string(sv.substr(0, port_pos));
    port = std::string(sv.substr(port_pos + 1));
  }
  else
  {
    host = std::string(sv);
    port = "80";
  }
}

void websocket_client_beast::connect(const std::string& uri)
{
  std::string host, port, path;
  parse_uri(uri, host, port, path);

  m_host = host;
  m_ws = std::make_unique<
      boost::beast::websocket::stream<boost::beast::tcp_stream>>(
      boost::asio::make_strand(m_context));

  m_ws->set_option(
      boost::beast::websocket::stream_base::timeout::suggested(
          boost::beast::role_type::client));

  m_ws->set_option(
      boost::beast::websocket::stream_base::decorator(
          [](boost::beast::websocket::request_type& req) {
    req.set(boost::beast::http::field::user_agent, "ossia");
  }));

  m_connected = true;
  do_resolve(host, port, path);
}

void websocket_client_beast::connect_and_run(const std::string& uri)
{
  connect(uri);
  m_context.run();
  m_context.restart();
  m_connected = false;
}

void websocket_client_beast::stop()
{
  m_connected = false;
  m_open = false;

  // Forcefully close the underlying TCP socket.
  // A synchronous WebSocket close handshake would deadlock if the
  // peer's io_context is not running.
  {
    std::lock_guard lock{m_mutex};
    if(m_ws)
    {
      boost::beast::error_code ec;
      boost::beast::get_lowest_layer(*m_ws).socket().shutdown(
          boost::asio::ip::tcp::socket::shutdown_both, ec);
      boost::beast::get_lowest_layer(*m_ws).socket().close(ec);
    }
  }

  // If we own the io_context, stop it so that run() returns in connect_and_run().
  if(m_owned_context)
    m_owned_context->stop();
}

bool websocket_client_beast::connected() const
{
  return m_open;
}

void websocket_client_beast::send_message(const std::string& request)
{
  if(!m_open || !m_ws)
    return;

  std::lock_guard lock{m_mutex};
  boost::beast::error_code ec;
  m_ws->text(true);
  m_ws->write(boost::asio::buffer(request), ec);
  if(ec)
    ossia::logger().error("WS send error: {}", ec.message());
}

void websocket_client_beast::send_message(const rapidjson::StringBuffer& request)
{
  if(!m_open || !m_ws)
    return;

  std::lock_guard lock{m_mutex};
  boost::beast::error_code ec;
  m_ws->text(true);
  m_ws->write(boost::asio::buffer(request.GetString(), request.GetSize()), ec);
  if(ec)
    ossia::logger().error("WS send error: {}", ec.message());
}

void websocket_client_beast::send_binary_message(std::string_view request)
{
  if(!m_open || !m_ws)
    return;

  std::lock_guard lock{m_mutex};
  boost::beast::error_code ec;
  m_ws->binary(true);
  m_ws->write(boost::asio::buffer(request.data(), request.size()), ec);
  if(ec)
    ossia::logger().error("WS send error: {}", ec.message());
}

void websocket_client_beast::do_resolve(
    const std::string& host, const std::string& port, const std::string& path)
{
  m_resolver.async_resolve(
      host, port,
      [this, path](
          boost::beast::error_code ec,
          boost::asio::ip::tcp::resolver::results_type results) {
    on_resolve(ec, std::move(results), path);
  });
}

void websocket_client_beast::on_resolve(
    boost::beast::error_code ec,
    boost::asio::ip::tcp::resolver::results_type results, std::string path)
{
  if(ec)
  {
    m_connected = false;
    on_fail();
    return;
  }

  boost::beast::get_lowest_layer(*m_ws).async_connect(
      results,
      [this, path = std::move(path)](
          boost::beast::error_code ec,
          boost::asio::ip::tcp::resolver::results_type::endpoint_type ep) {
    on_connect(ec, ep, std::move(path));
  });
}

void websocket_client_beast::on_connect(
    boost::beast::error_code ec,
    boost::asio::ip::tcp::resolver::results_type::endpoint_type,
    std::string path)
{
  if(ec)
  {
    m_connected = false;
    on_fail();
    return;
  }

  boost::beast::get_lowest_layer(*m_ws).expires_never();
  boost::asio::ip::tcp::no_delay opt(true);
  try
  {
    boost::beast::get_lowest_layer(*m_ws).socket().set_option(opt);
  }
  catch(...)
  {
  }

  // Use host:port for the Host header
  auto host = m_host + ":" + std::to_string(
      boost::beast::get_lowest_layer(*m_ws).socket().remote_endpoint().port());

  m_ws->async_handshake(
      host, path,
      [this](boost::beast::error_code ec) { on_handshake(ec); });
}

void websocket_client_beast::on_handshake(boost::beast::error_code ec)
{
  if(ec)
  {
    m_connected = false;
    on_fail();
    return;
  }

  m_open = true;
  on_open();

  // Start reading
  do_read();
}

void websocket_client_beast::do_read()
{
  m_buffer.clear();
  m_ws->async_read(
      m_buffer,
      [this](boost::beast::error_code ec, std::size_t bytes) {
    on_read(ec, bytes);
  });
}

void websocket_client_beast::on_read(boost::beast::error_code ec, std::size_t)
{
  if(ec)
  {
    m_open = false;
    on_close();
    return;
  }

  if(m_on_message)
  {
    auto opcode = m_ws->got_text() ? ws_opcode::text : ws_opcode::binary;
    auto data = boost::beast::buffers_to_string(m_buffer.data());
    ws_connection_handle hdl; // not used for client
    m_on_message(hdl, opcode, data);
  }

  do_read();
}

}
