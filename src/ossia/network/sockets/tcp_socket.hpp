#pragma once
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <asio/placeholders.hpp>


namespace ossia::net
{
class tcp_listener
{
public:
  using proto = asio::ip::tcp;
  using socket = typename proto::socket;

  tcp_listener() = delete;
  tcp_listener(const tcp_listener&) = delete;
  tcp_listener& operator=(const tcp_listener&) = delete;
  tcp_listener(tcp_listener&&) = default;
  tcp_listener& operator=(tcp_listener&&) = default;
  explicit tcp_listener(proto::socket sock)
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

class tcp_server
{
public:
  using proto = asio::ip::tcp;
  using socket = typename proto::socket;
  using listener = tcp_listener;

  tcp_server(
      std::string_view ip, uint16_t port, asio::io_context& ctx)
      : m_context {ctx}
      , m_acceptor {ctx, proto::endpoint{asio::ip::make_address(ip), port}}
  {
  }

  asio::io_context& m_context;
  proto::acceptor m_acceptor;
};

class tcp_client
{
public:
  using proto = asio::ip::tcp;
  using socket = typename proto::socket;

  tcp_client(
      std::string_view ip, uint16_t port, asio::io_context& ctx)
      : m_context {ctx}
      , m_endpoint {asio::ip::make_address(ip), port}
      , m_socket {ctx}
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
}
