#pragma once
#include <ossia/network/sockets/configuration.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/datagram_protocol.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>

#include <nano_signal_slot.hpp>

namespace ossia::net
{
class tcp_listener
{
public:
  using proto = boost::asio::ip::tcp;
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

  void close() { m_socket.close(); }

  void write(const boost::asio::const_buffer& buf) { boost::asio::write(m_socket, buf); }

  void on_close() { }

  void on_fail() { }

  proto::socket m_socket;
};

class tcp_server
{
public:
  using proto = boost::asio::ip::tcp;
  using socket = typename proto::socket;
  using listener = tcp_listener;

  tcp_server(const socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_acceptor{
            boost::asio::make_strand(ctx),
            proto::endpoint{boost::asio::ip::make_address(conf.host), conf.port}}
  {
  }

  boost::asio::io_context& m_context;
  proto::acceptor m_acceptor;
};

class tcp_client
{
public:
  using proto = boost::asio::ip::tcp;
  using socket = typename proto::socket;

  tcp_client(const socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_endpoint{boost::asio::ip::make_address(conf.host), conf.port}
      , m_socket{boost::asio::make_strand(ctx)}
  {
  }

  void connect()
  {
    m_socket.connect(m_endpoint);
    on_open();
  }

  bool connected() const { return m_connected; }

  void close()
  {
    m_context.post([this] {
      m_socket.close();
      on_close();
    });
  }

  void write(const char* data, std::size_t sz)
  {
    boost::asio::write(m_socket, boost::asio::buffer(data, sz));
  }

  Nano::Signal<void()> on_open;
  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;

  boost::asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
  bool m_connected{false};
};
}
