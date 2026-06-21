#if !defined(__EMSCRIPTEN__)
#include <ossia/network/sockets/websocket_client_beast.hpp>

#include <ossia/detail/json.hpp>

#include <boost/asio/dispatch.hpp>

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

void websocket_client_beast::connect(const std::string& uri)
{
  auto parsed = parse_websocket_uri(uri);

  // TLS is not implemented in the beast backend yet. Rather than silently
  // connecting in plaintext to a wss:// endpoint (a security downgrade), fail
  // loudly so the caller knows the secure expectation could not be met.
  if(parsed.secure)
  {
    ossia::logger().error(
        "wss:// / https:// requested but TLS is not supported by this "
        "WebSocket backend: {}",
        uri);
    m_connected = false;
    on_fail();
    return;
  }

  const auto& host = parsed.host;
  const auto& port = parsed.port;
  const auto& path = parsed.path;

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
  enqueue_write(true, request);
}

void websocket_client_beast::send_message(const rapidjson::StringBuffer& request)
{
  enqueue_write(true, std::string(request.GetString(), request.GetSize()));
}

void websocket_client_beast::send_binary_message(std::string_view request)
{
  enqueue_write(false, std::string(request));
}

void websocket_client_beast::enqueue_write(bool text, std::string data)
{
  if(!m_open || !m_ws)
    return;

  // A beast websocket::stream is not thread-safe. send_*() is called from the
  // producer thread, so marshal the write onto the stream strand and serialize
  // through a queue (one async_write in flight at a time, payload copied here
  // so it outlives the operation). This never runs concurrently with the read.
  boost::asio::dispatch(
      m_ws->get_executor(),
      [this, text, data = std::move(data)]() mutable {
    if(!m_ws)
      return;
    const bool idle = m_write_queue.empty();
    m_write_queue.emplace_back(text, std::move(data));
    if(idle)
      do_write();
  });
}

void websocket_client_beast::do_write()
{
  m_ws->text(m_write_queue.front().first);
  m_ws->async_write(
      boost::asio::buffer(m_write_queue.front().second),
      [this](boost::beast::error_code ec, std::size_t bytes) {
    on_write(ec, bytes);
  });
}

void websocket_client_beast::on_write(boost::beast::error_code ec, std::size_t)
{
  if(ec)
  {
    ossia::logger().error("WS send error: {}", ec.message());
    m_write_queue.clear();
    return;
  }

  m_write_queue.pop_front();
  if(!m_write_queue.empty())
    do_write();
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

  // Use host:port for the Host header. remote_endpoint() can throw if the
  // peer disconnected meanwhile; this runs inside an io_context handler, where
  // an exception would propagate out of run() and stop all networking, so use
  // the non-throwing overload.
  auto host = m_host;
  {
    boost::beast::error_code ep_ec;
    auto ep = boost::beast::get_lowest_layer(*m_ws).socket().remote_endpoint(ep_ec);
    if(!ep_ec)
      host += ":" + std::to_string(ep.port());
  }

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
#endif
