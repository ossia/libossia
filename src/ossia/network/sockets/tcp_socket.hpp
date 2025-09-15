#pragma once
#include <ossia/network/context.hpp>
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

  void close()
  {
    // FIXME async?
    try
    {
      m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
    }
    catch(...)
    {
    }
    m_socket.close();
  }

  void write(const boost::asio::const_buffer& buf)
  {
    boost::system::error_code ec;
    boost::asio::write(m_socket, buf, ec);
  }

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

  tcp_server(const inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_acceptor{
            boost::asio::make_strand(ctx),
            proto::endpoint{boost::asio::ip::make_address(conf.bind), conf.port}}
  {
  }

  tcp_server(
      const inbound_socket_configuration& conf, ossia::net::network_context_ptr ctx)
      : tcp_server{conf, ctx->context}
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

  tcp_client(const outbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_endpoint{boost::asio::ip::make_address(conf.host), conf.port}
      , m_socket{boost::asio::make_strand(ctx)}
  {
  }

  void connect()
  {
    boost::system::error_code ec;
    m_socket.set_option(boost::asio::ip::tcp::no_delay{true}, ec);
    m_socket.set_option(boost::asio::socket_base::reuse_address{true}, ec);

    m_socket.async_connect(
        m_endpoint, [this](const boost::system::error_code& ec, auto&&...) {
      if(m_socket.is_open() && !ec)
      {
        m_connected = true;
        on_open();
      }
      else
      {
        m_connected = false;
        puts(ec.message().c_str());
        on_fail();
      }
    });
  }

  bool connected() const { return m_connected; }

  void close()
  {
    boost::asio::post(m_context, [this] {
      try
      {
        m_socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
      }
      catch(...)
      {
      }
      m_socket.close();
      on_close();
    });
  }

  void write(const char* data, std::size_t sz)
  {
    boost::system::error_code ec;
    boost::asio::write(m_socket, boost::asio::const_buffer(data, sz), ec);
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
