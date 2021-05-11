#pragma once
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/placeholders.hpp>

namespace ossia::net
{
#if defined(ASIO_HAS_LOCAL_SOCKETS)
class unix_datagram_socket
{
  using proto = asio::local::datagram_protocol;

public:
  unix_datagram_socket(std::string_view path, asio::io_context& ctx)
      : m_context {ctx}, m_endpoint {path}, m_socket {ctx}
  {
    ::unlink(m_endpoint.path().data());
  }

  void open()
  {
    m_socket.open();
    m_socket.bind(m_endpoint);
  }

  void connect()
  {
    m_socket.open();
    // m_socket.connect(m_endpoint);
  }

  template <typename F>
  void close(F f)
  {
    m_context.post([this, f] {
      m_socket.close();
      f();
    });
  }

  template <typename F>
  void receive(F f)
  {
    m_socket.async_receive_from(
        asio::buffer(m_data), m_endpoint,
        [this, f](std::error_code ec, std::size_t sz) {
          if (ec == asio::error::operation_aborted)
            return;

          if (!ec && sz > 0)
            try
            {
              f(m_data, sz);
            }
            catch (...)
            {
            }

          this->receive(f);
        });
  }

  void write(const char* data, std::size_t sz)
  {
    m_socket.send_to(asio::buffer(data, sz), m_endpoint);
  }

  asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
  alignas(16) char m_data[65535];
};

class unix_stream_listener
{
public:
  using proto = asio::local::stream_protocol;
  unix_stream_listener() = delete;
  unix_stream_listener(const unix_stream_listener&) = delete;
  unix_stream_listener& operator=(const unix_stream_listener&) = delete;
  unix_stream_listener(unix_stream_listener&&) = default;
  unix_stream_listener& operator=(unix_stream_listener&&) = default;
  explicit unix_stream_listener(proto::socket sock)
      : m_socket{std::move(sock)}
  {
  }

  void close()
  {
    m_socket.close();
  }

  void write(const asio::ASIO_CONST_BUFFER& buf)
  {
    m_socket.write_some(buf);
  }

  proto::socket m_socket;
};


class unix_stream_server
{
public:
  using proto = asio::local::stream_protocol;
  using listener = unix_stream_listener;
  [[no_unique_address]]
  struct ensure_reuse {
    explicit ensure_reuse(const proto::endpoint& endpoint) { ::unlink(endpoint.path().data()); }
  } m_ensure_reuse;

  unix_stream_server(
      proto::endpoint endpoint, asio::io_context& ctx)
      : m_ensure_reuse{endpoint}
      , m_context {ctx}
      , m_acceptor {ctx, endpoint}
  {

  }

  asio::io_context& m_context;
  proto::acceptor m_acceptor;
};

class unix_stream_client
{
public:
  using proto = asio::local::stream_protocol;

  unix_stream_client(std::string_view path, asio::io_context& ctx)
      : m_context {ctx}, m_endpoint {path}, m_socket {ctx}
  {
  }

  void connect()
  {
    m_socket.connect(m_endpoint);
  }

  template <typename F>
  void close(F f)
  {
    m_context.post([this, f] {
      m_socket.close();
      f();
    });
  }

  void write(const char* data, std::size_t sz)
  {
    m_socket.write_some(asio::buffer(data, sz));
  }

  asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
};
#endif
}
