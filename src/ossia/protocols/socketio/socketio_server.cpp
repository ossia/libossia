#include <ossia/protocols/socketio/socketio_server.hpp>

#include <ossia/detail/json.hpp>

#include <random>

namespace ossia::net
{

// ---- SID generation ----

static std::string generate_random_sid()
{
  // FIXME use pcg
  static thread_local std::mt19937 gen{std::random_device{}()};
  static constexpr char chars[]
      = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  std::uniform_int_distribution<int> dist(0, sizeof(chars) - 2);
  std::string sid;
  sid.reserve(20);
  for(int i = 0; i < 20; ++i)
    sid += chars[dist(gen)];
  return sid;
}

// ---- socketio_server_connection ----

socketio_server_connection::socketio_server_connection(
    boost::asio::ip::tcp::socket&& socket, socketio_server& server)
    : m_ws{std::move(socket)}
    , m_ping_timer{m_ws.get_executor()}
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

  m_config.sid = generate_random_sid();
}

socketio_server_connection::~socketio_server_connection() = default;

void socketio_server_connection::run()
{
  do_read_http();
}

void socketio_server_connection::send_text(const std::string& msg)
{
  if(m_is_websocket)
  {
    m_ws.text(true);
    boost::beast::error_code ec;
    m_ws.write(boost::asio::buffer(msg), ec);
  }
  else
  {
    m_poll_buffer.push_back(msg);
  }
}

void socketio_server_connection::send_text(const char* data, std::size_t sz)
{
  send_text(std::string(data, sz));
}

void socketio_server_connection::send_binary(std::string_view msg)
{
  if(m_is_websocket)
  {
    m_ws.binary(true);
    boost::beast::error_code ec;
    m_ws.write(boost::asio::buffer(msg.data(), msg.size()), ec);
  }
  else
  {
    // For polling, binary is base64-encoded with 'b' prefix
    // Not commonly used in practice since we upgrade to WS
    m_poll_buffer.push_back(std::string(msg));
  }
}

void socketio_server_connection::close()
{
  m_ping_timer.cancel();
  boost::beast::error_code ec;
  if(m_is_websocket && m_ws.is_open())
    m_ws.close(boost::beast::websocket::close_code::going_away, ec);
  else
    boost::beast::get_lowest_layer(m_ws).socket().close(ec);
}

boost::asio::ip::tcp::socket& socketio_server_connection::tcp_socket()
{
  return boost::beast::get_lowest_layer(m_ws).socket();
}

std::string socketio_server_connection::generate_sid()
{
  return generate_random_sid();
}

void socketio_server_connection::do_read_http()
{
  m_buffer.clear();
  m_http_req = {};

  boost::beast::http::async_read(
      boost::beast::get_lowest_layer(m_ws), m_buffer, m_http_req,
      [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes) {
    self->on_read_http(ec, bytes);
  });
}

void socketio_server_connection::on_read_http(boost::beast::error_code ec, std::size_t)
{
  if(ec)
    return;

  auto target = std::string(m_http_req.target());

  // Check for WebSocket upgrade
  if(boost::beast::websocket::is_upgrade(m_http_req))
  {
    m_is_websocket = true;
    do_accept_ws(std::move(m_http_req));
    return;
  }

  // Engine.IO HTTP polling
  if(target.find("transport=polling") != std::string::npos)
  {
    if(target.find("sid=") == std::string::npos)
    {
      // New connection: no SID yet
      handle_new_connection(m_http_req);
    }
    else if(m_http_req.method() == boost::beast::http::verb::get)
    {
      handle_polling_get(m_http_req);
    }
    else if(m_http_req.method() == boost::beast::http::verb::post)
    {
      handle_polling_post(m_http_req);
    }
    // Continue reading next HTTP request
    do_read_http();
    return;
  }

  // Unknown request
  send_http_response(
      m_http_req.version(), boost::beast::http::status::not_found, "Not found");
  do_read_http();
}

void socketio_server_connection::handle_new_connection(
    boost::beast::http::request<boost::beast::http::string_body>& req)
{
  // Send Engine.IO OPEN packet
  auto open_packet = make_engineio_open(m_config);

  send_http_response(
      req.version(), boost::beast::http::status::ok, open_packet,
      "text/plain; charset=UTF-8");
  m_handshake_done = true;
}

void socketio_server_connection::handle_polling_get(
    boost::beast::http::request<boost::beast::http::string_body>& req)
{
  if(!m_poll_buffer.empty())
  {
    // Concatenate buffered messages with record separator
    std::string response;
    for(size_t i = 0; i < m_poll_buffer.size(); ++i)
    {
      if(i > 0)
        response += '\x1e';
      response += m_poll_buffer[i];
    }
    m_poll_buffer.clear();
    send_http_response(
        req.version(), boost::beast::http::status::ok, response,
        "text/plain; charset=UTF-8");
  }
  else
  {
    // Send NOOP to keep the connection alive
    send_http_response(
        req.version(), boost::beast::http::status::ok,
        std::string(1, EIO_NOOP), "text/plain; charset=UTF-8");
  }
}

void socketio_server_connection::handle_polling_post(
    boost::beast::http::request<boost::beast::http::string_body>& req)
{
  auto body = req.body();

  // Parse potentially multiple messages separated by \x1e
  std::string_view sv = body;
  while(!sv.empty())
  {
    auto sep = sv.find('\x1e');
    auto msg = (sep != std::string_view::npos) ? sv.substr(0, sep) : sv;

    process_engineio_message(msg);

    if(sep != std::string_view::npos)
      sv = sv.substr(sep + 1);
    else
      break;
  }

  send_http_response(req.version(), boost::beast::http::status::ok, "ok");
}

void socketio_server_connection::send_http_response(
    unsigned version, boost::beast::http::status status, const std::string& body,
    const std::string& content_type)
{
  namespace http = boost::beast::http;
  http::response<http::string_body> res{status, version};
  res.set(http::field::server, "ossia");
  res.set(http::field::access_control_allow_origin, "*");
  res.set(http::field::content_type, content_type);
  res.set(http::field::connection, "keep-alive");
  res.body() = body;
  res.prepare_payload();

  boost::beast::error_code ec;
  http::write(boost::beast::get_lowest_layer(m_ws), res, ec);
}

void socketio_server_connection::do_accept_ws(
    boost::beast::http::request<boost::beast::http::string_body> req)
{
  m_ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(
      boost::beast::role_type::server));

  m_ws.async_accept(
      req,
      [self = shared_from_this()](boost::beast::error_code ec) {
    self->on_accept_ws(ec);
  });
}

void socketio_server_connection::on_accept_ws(boost::beast::error_code ec)
{
  if(ec)
    return;

  m_is_websocket = true;
  // Don't notify server yet: wait for the probe/upgrade sequence
  do_read_ws();
}

void socketio_server_connection::do_read_ws()
{
  m_buffer.clear();
  m_ws.async_read(
      m_buffer,
      [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes) {
    self->on_read_ws(ec, bytes);
  });
}

void socketio_server_connection::on_read_ws(boost::beast::error_code ec, std::size_t)
{
  if(ec)
  {
    m_ping_timer.cancel();
    m_server.remove_connection(shared_from_this());
    return;
  }

  auto data = boost::beast::buffers_to_string(m_buffer.data());

  // Handle probe sequence
  if(data == "2probe")
  {
    // Respond with pong probe
    m_ws.text(true);
    boost::beast::error_code wec;
    m_ws.write(boost::asio::buffer("3probe", 6), wec);
    do_read_ws();
    return;
  }

  if(data == "5")
  {
    // Now the client is fully on WebSocket
    m_upgraded = true;

    // Notify the server that this connection is ready
    m_server.add_connection(shared_from_this());

    // Start ping timer
    start_ping_timer();

    do_read_ws();
    return;
  }

  // Normal Engine.IO message
  process_engineio_message(data);
  do_read_ws();
}

void socketio_server_connection::start_ping_timer()
{
  m_ping_timer.expires_after(m_config.ping_interval);
  m_ping_timer.async_wait(
      [self = shared_from_this()](boost::beast::error_code ec) {
    self->on_ping_timer(ec);
  });
}

void socketio_server_connection::on_ping_timer(boost::beast::error_code ec)
{
  if(ec)
    return; // Timer cancelled

  // Send ping
  if(m_is_websocket && m_ws.is_open())
  {
    m_ws.text(true);
    boost::beast::error_code wec;
    m_ws.write(boost::asio::buffer("2", 1), wec);
    if(!wec)
      start_ping_timer();
  }
}

void socketio_server_connection::process_engineio_message(std::string_view data)
{
  if(data.empty())
    return;

  switch(data[0])
  {
    case EIO_PONG:
      // Client responded to our ping: connection alive
      break;
    case EIO_CLOSE:
      m_ping_timer.cancel();
      m_server.remove_connection(shared_from_this());
      break;
    case EIO_MESSAGE:
      data = data.substr(1);
      dispatch_socketio_message(data);
      break;
    case EIO_PING:
      // Client shouldn't send pings in v4, but respond anyway
      send_text(std::string(1, EIO_PONG));
      break;
    default:
      break;
  }
}

void socketio_server_connection::dispatch_socketio_message(std::string_view data)
{
  if(data.empty())
    return;

  switch(data[0])
  {
    case SIO_CONNECT: {
      // Client sends CONNECT. Anwser: CONNECT + sid
      auto response = make_socketio_connect(m_config.sid);
      send_text(response);
      break;
    }
    case SIO_DISCONNECT:
      m_ping_timer.cancel();
      m_server.remove_connection(shared_from_this());
      break;
    case SIO_EVENT: {
      data = data.substr(1);
      if(!data.empty() && m_server.m_on_message)
      {
        auto payload = std::string(data);
        try
        {
          auto reply = m_server.m_on_message(
              shared_from_this(), ws_opcode::text, payload);
          if(!reply.data.empty())
          {
            // Wrap reply as Socket.IO EVENT
            std::string response;
            response.reserve(2 + reply.data.size());
            response += EIO_MESSAGE;
            response += SIO_EVENT;
            response += reply.data;
            send_text(response);
          }
        }
        catch(const std::exception& e)
        {
          ossia::logger().error("socketio_server: message error: {}", e.what());
        }
      }
      break;
    }
    case SIO_BINARY_EVENT: {
      data = data.substr(1);
      // Skip attachment count
      if(auto dash = data.find('-'); dash != std::string_view::npos)
        data = data.substr(dash + 1);
      if(!data.empty() && m_server.m_on_message)
      {
        auto payload = std::string(data);
        try
        {
          auto reply = m_server.m_on_message(
              shared_from_this(), ws_opcode::binary, payload);
          if(!reply.data.empty())
          {
            std::string response;
            response.reserve(2 + reply.data.size());
            response += EIO_MESSAGE;
            response += SIO_BINARY_EVENT;
            response += reply.data;
            send_text(response);
          }
        }
        catch(const std::exception& e)
        {
          ossia::logger().error("socketio_server: message error: {}", e.what());
        }
      }
      break;
    }
    default:
      break;
  }
}

// ---- socketio_server ----

socketio_server::socketio_server(ossia::net::network_context_ptr ctx)
    : m_context{std::move(ctx)}
    , m_acceptor{boost::asio::make_strand(m_context->context)}
{
}

socketio_server::~socketio_server()
{
  stop();
}

void socketio_server::listen(uint16_t port)
{
  // Dual-stack: try v6 first, fall back to v4
  boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::tcp::v6(), port};

  boost::beast::error_code ec;
  m_acceptor.open(endpoint.protocol(), ec);
  if(ec)
    return;

  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  m_acceptor.set_option(boost::asio::ip::v6_only(false), ec);

  m_acceptor.bind(endpoint, ec);
  if(ec)
  {
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

void socketio_server::run()
{
  m_context->context.run();
}

void socketio_server::stop()
{
  boost::beast::error_code ec;
  if(m_acceptor.is_open())
    m_acceptor.close(ec);

  std::lock_guard lock{m_connections_mutex};
  for(auto& conn : m_connections)
    conn->close();
  m_connections.clear();
}

void socketio_server::close(ws_connection_handle hdl)
{
  if(auto conn = find_connection(hdl))
    conn->close();
}

void socketio_server::set_open_handler(ws_open_handler h)
{
  m_on_open = std::move(h);
}

void socketio_server::set_close_handler(ws_close_handler h)
{
  m_on_close = std::move(h);
}

void socketio_server::set_message_handler(ws_server_message_handler h)
{
  m_on_message = std::move(h);
}

std::string socketio_server::get_remote_ip(const ws_connection_handle& hdl)
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

std::string socketio_server::get_remote_endpoint(const ws_connection_handle& hdl)
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

std::string socketio_server::get_local_ip(const ws_connection_handle& hdl)
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

void socketio_server::send_message(
    ws_connection_handle hdl, const std::string& message)
{
  if(auto conn = find_connection(hdl))
    send_sio_text(conn, message);
}

void socketio_server::send_message(
    ws_connection_handle hdl, const server_reply& message)
{
  if(auto conn = find_connection(hdl))
  {
    switch(message.type)
    {
      case server_reply::data_type::json:
      case server_reply::data_type::html:
        send_sio_text(conn, message.data);
        break;
      default:
        send_sio_binary(conn, message.data);
        break;
    }
  }
}

void socketio_server::send_message(
    ws_connection_handle hdl, const rapidjson::StringBuffer& message)
{
  if(auto conn = find_connection(hdl))
    send_sio_text(conn, std::string(message.GetString(), message.GetSize()));
}

void socketio_server::send_binary_message(
    ws_connection_handle hdl, std::string_view message)
{
  if(auto conn = find_connection(hdl))
    send_sio_binary(conn, message);
}

void socketio_server::send_sio_text(
    socketio_server_connection* conn, const std::string& msg)
{
  // Wrap as EIO_MESSAGE + SIO_EVENT + payload
  std::string packet;
  packet.reserve(2 + msg.size());
  packet += EIO_MESSAGE;
  packet += SIO_EVENT;
  packet += msg;
  conn->send_text(packet);
}

void socketio_server::send_sio_binary(
    socketio_server_connection* conn, std::string_view msg)
{
  // Wrap as EIO_MESSAGE + SIO_BINARY_EVENT + payload
  std::string packet;
  packet.reserve(2 + msg.size());
  packet += EIO_MESSAGE;
  packet += SIO_BINARY_EVENT;
  packet += msg;
  conn->send_binary(packet);
}

void socketio_server::do_accept()
{
  m_acceptor.async_accept(
      boost::asio::make_strand(m_context->context),
      [this](boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
    on_accept(ec, std::move(socket));
  });
}

void socketio_server::on_accept(
    boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
  if(ec)
    return;

  auto conn
      = std::make_shared<socketio_server_connection>(std::move(socket), *this);
  conn->run();

  do_accept();
}

void socketio_server::add_connection(
    std::shared_ptr<socketio_server_connection> conn)
{
  ws_connection_handle hdl = conn;
  {
    std::lock_guard lock{m_connections_mutex};
    m_connections.insert(std::move(conn));
  }

  if(m_on_open)
    m_on_open(hdl);
}

void socketio_server::remove_connection(
    std::shared_ptr<socketio_server_connection> conn)
{
  ws_connection_handle hdl = conn;

  {
    std::lock_guard lock{m_connections_mutex};
    m_connections.erase(conn);
  }

  if(m_on_close)
    m_on_close(hdl);
}

socketio_server_connection*
socketio_server::find_connection(const ws_connection_handle& hdl)
{
  auto sp = hdl.lock();
  if(!sp)
    return nullptr;

  std::lock_guard lock{m_connections_mutex};
  auto it = m_connections.find(
      std::static_pointer_cast<socketio_server_connection>(sp));
  if(it != m_connections.end())
    return it->get();
  return nullptr;
}

}
