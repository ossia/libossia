#include "websocket_server_beast.hpp"

#include <iostream>
namespace ossia::net
{

template <class Body, class Allocator>
void websocket_ws_session::run(http::request<Body, http::basic_fields<Allocator>> req)
{
  // Set suggested timeout settings for the websocket
  ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

  // Set a decorator to change the Server of the handshake
  ws_.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {
    res.set(
        http::field::server,
        std::string(BOOST_BEAST_VERSION_STRING) + " websocket-chat-multi");
  }));

  // Accept the websocket handshake
  ws_.async_accept(
      req,
      beast::bind_front_handler(&websocket_ws_session::on_accept, shared_from_this()));
}

websocket_listener::websocket_listener(
    net::io_context& ioc, tcp::endpoint endpoint,
    boost::shared_ptr<websocket_shared_state> const& state)
    : ioc_(ioc)
    , acceptor_(ioc)
    , state_(state)
{
  beast::error_code ec;

  // Open the acceptor
  acceptor_.open(endpoint.protocol(), ec);
  if(ec)
  {
    fail(ec, "open");
    return;
  }

  // Allow address reuse
  acceptor_.set_option(net::socket_base::reuse_address(true), ec);
  if(ec)
  {
    fail(ec, "set_option");
    return;
  }

  // Bind to the server address
  acceptor_.bind(endpoint, ec);
  if(ec)
  {
    fail(ec, "bind");
    return;
  }

  // Start listening for connections
  acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if(ec)
  {
    fail(ec, "listen");
    return;
  }
}

void websocket_listener::run()
{
  // The new connection gets its own strand
  acceptor_.async_accept(
      net::make_strand(ioc_),
      beast::bind_front_handler(&websocket_listener::on_accept, shared_from_this()));
}

// Report a failure
void websocket_listener::fail(beast::error_code ec, char const* what)
{
  // Don't report on canceled operations
  if(ec == net::error::operation_aborted)
    return;
  std::cerr << what << ": " << ec.message() << "\n";
}

// Handle a connection
void websocket_listener::on_accept(beast::error_code ec, tcp::socket socket)
{
  if(ec)
    return fail(ec, "accept");
  else
  {
    boost::asio::ip::tcp::no_delay option(true);
    boost::system::error_code ec;
    socket.set_option(option, ec);
    // Launch a new session for this connection
    boost::make_shared<websocket_http_session>(std::move(socket), state_)->run();
  }
  // The new connection gets its own strand
  acceptor_.async_accept(
      net::make_strand(ioc_),
      beast::bind_front_handler(&websocket_listener::on_accept, shared_from_this()));
}

// HTTP //

// Return a response for the given request.
//
// The concrete type of the response message (which depends on the
// request), is type-erased in message_generator.
template <class Body, class Allocator>
http::message_generator handle_request(
    boost::shared_ptr<websocket_shared_state> doc_root,
    http::request<Body, http::basic_fields<Allocator>>&& req)
{
#if 0
  // Returns a bad request response
  auto const bad_request = [&req](beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  };

  // Returns a not found response
  auto const not_found = [&req](beast::string_view target) {
    http::response<http::string_body> res{http::status::not_found, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
  };

  // Returns a server error response
  auto const server_error = [&req](beast::string_view what) {
    http::response<http::string_body> res{
        http::status::internal_server_error, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
  };

  // Make sure we can handle the method
  if(req.method() != http::verb::get && req.method() != http::verb::head)
    return bad_request("Unknown HTTP-method");

  // Request path must be absolute and not contain "..".
  if(req.target().empty() || req.target()[0] != '/'
     || req.target().find("..") != beast::string_view::npos)
    return bad_request("Illegal request-target");

  // Build the path to the requested file
  std::string path = path_cat(doc_root, req.target());
  if(req.target().back() == '/')
    path.append("index.html");

  // Attempt to open the file
  beast::error_code ec;
  http::file_body::value_type body;
  body.open(path.c_str(), beast::file_mode::scan, ec);

  // Handle the case where the file doesn't exist
  if(ec == boost::system::errc::no_such_file_or_directory)
    return not_found(req.target());

  // Handle an unknown error
  if(ec)
    return server_error(ec.message());

  // Cache the size since we need it after the move
  auto const size = body.size();

  // Respond to HEAD request
  if(req.method() == http::verb::head)
  {
    http::response<http::empty_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
  }

  // Respond to GET request
  http::response<http::file_body> res{
      std::piecewise_construct, std::make_tuple(std::move(body)),
      std::make_tuple(http::status::ok, req.version())};
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, mime_type(path));
  res.content_length(size);
  res.keep_alive(req.keep_alive());
  return res;
#endif

  http::response<http::empty_body> res{http::status::ok, req.version()};
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, "none");
  res.content_length(0);
  res.keep_alive(req.keep_alive());
  return res;
}

//------------------------------------------------------------------------------

websocket_http_session::websocket_http_session(
    tcp::socket&& socket, boost::shared_ptr<websocket_shared_state> const& state)
    : stream_(std::move(socket))
    , state_(state)
{
}

void websocket_http_session::run()
{
  do_read();
}

// Report a failure
void websocket_http_session::fail(beast::error_code ec, char const* what)
{
  // Don't report on canceled operations
  if(ec == net::error::operation_aborted)
    return;

  std::cerr << what << ": " << ec.message() << "\n";
}

void websocket_http_session::do_read()
{
  // Construct a new parser for each message
  parser_.emplace();

  // Apply a reasonable limit to the allowed size
  // of the body in bytes to prevent abuse.
  parser_->body_limit(10000);

  // Set the timeout.
  stream_.expires_after(std::chrono::seconds(30));

  // Read a request
  http::async_read(
      stream_, buffer_, parser_->get(),
      beast::bind_front_handler(&websocket_http_session::on_read, shared_from_this()));
}

void websocket_http_session::on_read(beast::error_code ec, std::size_t)
{
  // This means they closed the connection
  if(ec == http::error::end_of_stream)
  {
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    return;
  }

  // Handle the error, if any
  if(ec)
    return fail(ec, "read");

  // See if it is a WebSocket Upgrade
  if(websocket::is_upgrade(parser_->get()))
  {
    // Create a websocket session, transferring ownership
    // of both the socket and the HTTP request.
    boost::make_shared<websocket_ws_session>(stream_.release_socket(), state_)
        ->run(parser_->release());
    return;
  }

  // Handle request
  http::message_generator msg = handle_request(state_, parser_->release());

  // Determine if we should close the connection
  bool keep_alive = msg.keep_alive();

  auto self = shared_from_this();

  // Send the response
  beast::async_write(
      stream_, std::move(msg),
      [self, keep_alive](beast::error_code ec, std::size_t bytes) {
    self->on_write(ec, bytes, keep_alive);
  });
}

void websocket_http_session::on_write(beast::error_code ec, std::size_t, bool keep_alive)
{
  // Handle the error, if any
  if(ec)
    return fail(ec, "write");

  if(!keep_alive)
  {
    // This means we should close the connection, usually because
    // the response indicated the "Connection: close" semantic.
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    return;
  }

  // Read another request
  do_read();
}

// WS //

websocket_ws_session::websocket_ws_session(
    tcp::socket&& socket, boost::shared_ptr<websocket_shared_state> const& state)
    : ws_(std::move(socket))
    , state_(state)
{
}

websocket_ws_session::~websocket_ws_session()
{
  // Remove this session from the list of active sessions
  // state_->leave(this);
}

void websocket_ws_session::fail(beast::error_code ec, char const* what)
{
  // Don't report these
  if(ec == net::error::operation_aborted || ec == websocket::error::closed)
    return;

  std::cerr << what << ": " << ec.message() << "\n";
}

void websocket_ws_session::on_accept(beast::error_code ec)
{
  // Handle the error, if any
  if(ec)
    return fail(ec, "accept");

  // Add this session to the list of active sessions
  state_->join(this);

  // Read a message
  ws_.async_read(
      buffer_,
      beast::bind_front_handler(&websocket_ws_session::on_read, shared_from_this()));
}

void websocket_ws_session::on_read(beast::error_code ec, std::size_t)
{
  // Handle the error, if any
  if(ec)
    return fail(ec, "read");

  // Send to all connections
  state_->send(beast::buffers_to_string(buffer_.data()));

  // Clear the buffer
  buffer_.consume(buffer_.size());

  // Read another message
  ws_.async_read(
      buffer_,
      beast::bind_front_handler(&websocket_ws_session::on_read, shared_from_this()));
}

void websocket_ws_session::send(boost::shared_ptr<std::string const> const& ss)
{
  // Post our work to the strand, this ensures
  // that the members of `this` will not be
  // accessed concurrently.

  net::post(
      ws_.get_executor(),
      beast::bind_front_handler(&websocket_ws_session::on_send, shared_from_this(), ss));
}

void websocket_ws_session::on_send(boost::shared_ptr<std::string const> const& ss)
{
  // Always add to queue
  queue_.push_back(ss);

  // Are we already writing?
  if(queue_.size() > 1)
    return;

  // We are not currently writing, so send this immediately
  ws_.async_write(
      net::buffer(*queue_.front()),
      beast::bind_front_handler(&websocket_ws_session::on_write, shared_from_this()));
}

void websocket_ws_session::on_write(beast::error_code ec, std::size_t)
{
  // Handle the error, if any
  if(ec)
    return fail(ec, "write");

  // Remove the string from the queue
  queue_.erase(queue_.begin());

  // Send the next message if any
  if(!queue_.empty())
    ws_.async_write(
        net::buffer(*queue_.front()),
        beast::bind_front_handler(&websocket_ws_session::on_write, shared_from_this()));
}
}

struct ssssss
{
  ossia::net::network_context_ptr p;
  ossia::net::websocket_server_beast x;
  ssssss()
      : p(std::make_shared<ossia::net::network_context>())
      , x(p)
  {
    x.set_open_handler([] { puts("OPEN\n;"); });
    x.set_close_handler([] { puts("CLOSE\n;"); });
    x.set_message_handler([] { puts("MESSAGE\n;"); });
  }
} server;
