#include <ossia/protocols/socketio/socketio_client.hpp>

#include <ossia/detail/json.hpp>

namespace ossia::net
{

socketio_client::socketio_client(boost::asio::io_context& ctx)
    : m_context{ctx}
    , m_write_channel{ctx}
{
}

socketio_client::socketio_client(
    boost::asio::io_context& ctx, ws_client_message_handler handler)
    : m_context{ctx}
    , m_write_channel{ctx}
    , m_on_message{std::move(handler)}
{
}

socketio_client::~socketio_client()
{
  if(m_open)
    stop();
}

void socketio_client::parse_uri(
    const std::string& uri, std::string& host, std::string& port,
    std::string& path)
{
  std::string_view sv = uri;
  if(sv.starts_with("ws://"))
    sv.remove_prefix(5);
  else if(sv.starts_with("wss://"))
    sv.remove_prefix(6);
  else if(sv.starts_with("http://"))
    sv.remove_prefix(7);
  else if(sv.starts_with("https://"))
    sv.remove_prefix(8);

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

void socketio_client::connect(const std::string& uri)
{
  std::string host, port, path;
  parse_uri(uri, host, port, path);
  m_host = host;
  m_connected = true;

  co_spawn_detached(run_session(std::move(host), std::move(port), std::move(path)));
}

void socketio_client::connect_and_run(const std::string& uri)
{
  connect(uri);
  m_context.run();
  m_connected = false;
}

void socketio_client::stop()
{
  if(m_open && m_ws)
  {
    // Send Socket.IO DISCONNECT then Engine.IO CLOSE
    enqueue_write(std::string("41")); // EIO_MESSAGE + SIO_DISCONNECT
  }

  std::lock_guard lock{m_mutex};
  if(m_ws && m_ws->is_open())
  {
    boost::beast::error_code ec;
    m_ws->close(boost::beast::websocket::close_code::normal, ec);
  }
  m_open = false;
  m_connected = false;
}

bool socketio_client::connected() const
{
  return m_open;
}

void socketio_client::send_message(const std::string& request)
{
  if(!m_open)
    return;
  // Wrap as Socket.IO EVENT: "42" + JSON array with the message
  // For OSCQuery, the message is already JSON, wrap it as: 42["message",<data>]
  std::string packet;
  packet.reserve(4 + request.size());
  packet += EIO_MESSAGE;
  packet += SIO_EVENT;
  packet += request;
  enqueue_write(std::move(packet));
}

void socketio_client::send_message(const rapidjson::StringBuffer& request)
{
  send_message(std::string(request.GetString(), request.GetSize()));
}

void socketio_client::send_binary_message(std::string_view request)
{
  if(!m_open)
    return;
  // Socket.IO BINARY_EVENT: "451-[...binary placeholder...]" + binary frame
  // For simplicity, send as a text event with base64 or just as a raw
  // EIO_MESSAGE with binary flag.
  // Actually, for OSCQuery compatibility, we send binary data as a websocket
  // binary frame wrapped in Engine.IO MESSAGE:
  std::string packet;
  packet.reserve(2 + request.size());
  packet += EIO_MESSAGE;
  packet += SIO_BINARY_EVENT;
  packet += request;
  enqueue_write(std::move(packet));
}

void socketio_client::enqueue_write(std::string msg)
{
  boost::asio::co_spawn(
      m_context,
      [this, msg = std::move(msg)]() -> awaitable<void> {
    co_await m_write_channel.async_send(
        boost::system::error_code{}, msg, boost::asio::use_awaitable);
  },
      [](std::exception_ptr) {});
}

socketio_client::awaitable<void> socketio_client::run_session(
    std::string host, std::string port, std::string)
{
  static constexpr auto init_timeout = std::chrono::seconds(5);
  auto executor = co_await boost::asio::this_coro::executor;
  auto resolver = boost::asio::ip::tcp::resolver{executor};

  m_read_buffer.reserve(1000000);
  auto const results = co_await resolver.async_resolve(host, port);

  m_ws = std::make_unique<websocket_type>(executor);
  m_ws->set_option(boost::beast::websocket::stream_base::timeout::suggested(
      boost::beast::role_type::client));
  m_ws->set_option(
      boost::beast::websocket::stream_base::decorator(
          [](boost::beast::websocket::request_type& req) {
    req.set(boost::beast::http::field::user_agent, "ossia");
  }));

  auto& stream = boost::beast::get_lowest_layer(*m_ws);

  // Step 1: TCP connect
  stream.expires_after(init_timeout);
  co_await stream.async_connect(results);

  // Step 2: Engine.IO handshake via HTTP polling
  {
    stream.expires_after(init_timeout);
    auto rep = co_await http_request(
        boost::beast::http::verb::get, stream,
        "/socket.io/?EIO=4&transport=polling");
    if(!parse_engineio_open(rep, m_config))
    {
      on_fail();
      co_return;
    }
  }

  // Step 3: Socket.IO CONNECT request via HTTP polling
  {
    stream.expires_after(init_timeout);
    auto rep = co_await http_request(
        boost::beast::http::verb::post, stream,
        "/socket.io/?EIO=4&transport=polling&sid=" + m_config.sid, "40");
    if(rep != "ok")
    {
      on_fail();
      co_return;
    }
  }

  // Step 4: Socket.IO CONNECT approval via HTTP polling
  {
    stream.expires_after(init_timeout);
    auto rep = co_await http_request(
        boost::beast::http::verb::get, stream,
        "/socket.io/?EIO=4&transport=polling&sid=" + m_config.sid);
    if(!parse_socketio_connect(rep, m_config))
    {
      on_fail();
      co_return;
    }
  }

  // Step 5: WebSocket upgrade
  {
    stream.expires_after(init_timeout);
    co_await m_ws->async_handshake(
        m_host,
        "/socket.io/?EIO=4&transport=websocket&sid=" + m_config.sid);

    // Probe sequence
    stream.expires_after(init_timeout);
    co_await m_ws->async_write(boost::asio::buffer("2probe", 6));

    stream.expires_after(init_timeout);
    m_read_buffer.clear();
    co_await m_ws->async_read(m_read_buffer);
    if(boost::beast::buffers_to_string(m_read_buffer.data()) != "3probe")
    {
      on_fail();
      co_return;
    }

    // Send upgrade packet
    m_read_buffer.clear();
    stream.expires_after(init_timeout);
    co_await m_ws->async_write(boost::asio::buffer("5", 1));
  }

  // Connected!
  auto& tcp = boost::beast::get_lowest_layer(*m_ws);
  tcp.expires_after(m_config.ping_interval + m_config.ping_timeout);

  m_open = true;
  on_open();

  // Start write loop and read loop
  co_spawn_detached(write_loop());
  co_await read_loop();
  co_await close_session();
}

socketio_client::awaitable<std::string> socketio_client::http_request(
    boost::beast::http::verb v, boost::beast::tcp_stream& stream,
    std::string_view target, std::string_view body)
{
  namespace http = boost::beast::http;
  http::request<http::string_body> req{v, target, 11};
  req.set(http::field::host, m_host);
  req.set(http::field::user_agent, "ossia"); // FIXME give end-user control over this
  req.set(http::field::connection, "keep-alive");
  if(!body.empty())
  {
    req.set(http::field::content_length, std::to_string(body.size()));
    req.body() = body;
  }

  co_await http::async_write(stream, req);
  m_read_buffer.clear();
  http::response<http::string_body> res;
  co_await http::async_read(stream, m_read_buffer, res);
  co_return res.body();
}

socketio_client::awaitable<void> socketio_client::read_loop()
{
  for(;;)
  {
    m_read_buffer.clear();
    auto bytes = co_await m_ws->async_read(m_read_buffer);
    if(bytes > 0)
    {
      auto data = boost::beast::buffers_to_string(m_read_buffer.data());
      if(!process_engineio(data))
      {
        ossia::logger().error("socketio_client: error processing message");
      }
    }
  }
}

socketio_client::awaitable<void> socketio_client::write_loop()
{
  for(;;)
  {
    auto str = co_await m_write_channel.async_receive(boost::asio::use_awaitable);
    co_await m_ws->async_write(boost::asio::buffer(str.data(), str.size()));
  }
}

socketio_client::awaitable<void> socketio_client::write_pong()
{
  auto& tcp = boost::beast::get_lowest_layer(*m_ws);
  tcp.expires_after(m_config.ping_interval + m_config.ping_timeout);

  co_await m_write_channel.async_send(
      boost::system::error_code{}, std::string(1, EIO_PONG),
      boost::asio::use_awaitable);
}

socketio_client::awaitable<void> socketio_client::close_session()
{
  m_open = false;
  if(m_ws)
  {
    auto& stream = boost::beast::get_lowest_layer(*m_ws);
    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  }
  on_close();
  co_return;
}

bool socketio_client::process_engineio(std::string_view data)
{
  if(data.empty())
    return false;

  switch(data[0])
  {
    case EIO_OPEN:
      return false; // Shouldn't happen after handshake
    case EIO_CLOSE:
      co_spawn_detached(close_session());
      return true;
    case EIO_PING:
      co_spawn_detached(write_pong());
      return true;
    case EIO_PONG:
      return true;
    case EIO_NOOP:
      return true;
    case EIO_UPGRADE:
      return true;
    case EIO_MESSAGE: {
      data = data.substr(1);
      if(data.empty())
        return false;
      dispatch_socketio_message(data);
      return true;
    }
    default:
      return true;
  }
}

void socketio_client::dispatch_socketio_message(std::string_view data)
{
  if(data.empty())
    return;

  switch(data[0])
  {
    case SIO_CONNECT:
      // Already connected
      break;
    case SIO_DISCONNECT:
      co_spawn_detached(close_session());
      break;
    case SIO_EVENT: {
      data = data.substr(1);
      if(!data.empty() && m_on_message)
      {
        auto str = std::string(data);
        ws_connection_handle hdl;
        m_on_message(hdl, ws_opcode::text, str);
      }
      break;
    }
    case SIO_ACK: {
      data = data.substr(1);
      // ACKs could be handled here if needed
      break;
    }
    case SIO_BINARY_EVENT: {
      data = data.substr(1);
      // Skip the attachment count prefix (e.g. "1-")
      if(auto dash = data.find('-'); dash != std::string_view::npos)
        data = data.substr(dash + 1);
      if(!data.empty() && m_on_message)
      {
        auto str = std::string(data);
        ws_connection_handle hdl;
        m_on_message(hdl, ws_opcode::binary, str);
      }
      break;
    }
    case SIO_BINARY_ACK:
      break;
    case SIO_CONNECT_ERROR:
      ossia::logger().error("socketio_client: connect error");
      co_spawn_detached(close_session());
      break;
    default:
      break;
  }
}

}
