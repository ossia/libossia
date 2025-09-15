#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/configuration.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/datagram_protocol.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>

#include <nano_signal_slot.hpp>

namespace ossia::net
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
class unix_datagram_socket
{
  using proto = boost::asio::local::datagram_protocol;

public:
  unix_datagram_socket(const fd_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_endpoint{conf.fd}
      , m_socket{boost::asio::make_strand(ctx)}
  {
  }

  void open()
  {
    ::unlink(m_endpoint.path().data());
    m_socket.open();
    m_socket.bind(m_endpoint);
  }

  void connect()
  {
    m_socket.open();
    // m_socket.connect(m_endpoint);
  }

  void close()
  {
    if(m_socket.is_open())
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
  }

  template <typename F>
  void receive(F f)
  {
    m_socket.async_receive_from(
        boost::asio::mutable_buffer(&m_data[0], std::size(m_data)), m_endpoint,
        [this, f](boost::system::error_code ec, std::size_t sz) {
      if(ec == boost::asio::error::operation_aborted)
        return;

      if(!ec && sz > 0)
        try
        {
          f(m_data, sz);
        }
        catch(...)
        {
        }

      this->receive(f);
        });
  }

  void write(const char* data, std::size_t sz)
  {
    m_socket.send_to(boost::asio::buffer(data, sz), m_endpoint);
  }

  Nano::Signal<void()> on_close;

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

  void write(const boost::asio::const_buffer& buf) { boost::asio::write(m_socket, buf); }

  void on_close() { }

  void on_fail() { }

  proto::socket m_socket;
};

class unix_stream_server
{
public:
  using proto = boost::asio::local::stream_protocol;
  using listener = unix_stream_listener;
  [[no_unique_address]] struct ensure_reuse
  {
    explicit ensure_reuse(const proto::endpoint& endpoint)
    {
      ::unlink(endpoint.path().data());
    }
  } m_ensure_reuse;

  unix_stream_server(const fd_configuration& conf, boost::asio::io_context& ctx)
      : m_ensure_reuse{conf.fd}
      , m_context{ctx}
      , m_acceptor{boost::asio::make_strand(ctx), conf.fd}
  {
  }

  unix_stream_server(const fd_configuration& conf, ossia::net::network_context_ptr ctx)
      : unix_stream_server{conf, ctx->context}
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
      : m_context{ctx}
      , m_endpoint{conf.fd}
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
#endif
}
