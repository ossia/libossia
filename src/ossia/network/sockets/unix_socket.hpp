#pragma once
#include <ossia/network/sockets/configuration.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/datagram_protocol.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/placeholders.hpp>

namespace ossia::net
{
#if defined(ASIO_HAS_LOCAL_SOCKETS)
class unix_datagram_socket
{
  using proto = boost::asio::local::datagram_protocol;

public:
  unix_datagram_socket(const fd_configuration& conf, boost::asio::io_context& ctx)
      : m_context {ctx}, m_endpoint {conf.fd}, m_socket {ctx}
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
        boost::asio::buffer(m_data), m_endpoint,
        [this, f](std::error_code ec, std::size_t sz) {
          if (ec == boost::asio::error::operation_aborted)
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
    m_socket.send_to(boost::asio::buffer(data, sz), m_endpoint);
  }

  boost::asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
  alignas(16) char m_data[65535];
};

class unix_stream_listener
{
public:
  using proto = boost::asio::local::stream_protocol;
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

  void write(const boost::asio::ASIO_CONST_BUFFER& buf)
  {
    m_socket.write_some(buf);
  }

  proto::socket m_socket;
};


class unix_stream_server
{
public:
  using proto = boost::asio::local::stream_protocol;
  using listener = unix_stream_listener;
  [[no_unique_address]]
  struct ensure_reuse {
    explicit ensure_reuse(const proto::endpoint& endpoint) { ::unlink(endpoint.path().data()); }
  } m_ensure_reuse;

  unix_stream_server(const fd_configuration& conf, boost::asio::io_context& ctx)
      : m_ensure_reuse{conf.fd}
      , m_context {ctx}
      , m_acceptor {ctx, conf.fd}
  {

  }

  boost::asio::io_context& m_context;
  proto::acceptor m_acceptor;
};

class unix_stream_client
{
public:
  using proto = boost::asio::local::stream_protocol;
  using socket = typename proto::socket;

  unix_stream_client(const fd_configuration& conf, boost::asio::io_context& ctx)
      : m_context {ctx}, m_endpoint {conf.fd}, m_socket {ctx}
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
    m_socket.write_some(boost::asio::buffer(data, sz));
  }

  boost::asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
};
#endif
}
