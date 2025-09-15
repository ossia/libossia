#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/network/sockets/configuration.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/datagram_protocol.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>

#include <nano_signal_slot.hpp>

namespace ossia::net
{

class udp_receive_socket
{
  using proto = boost::asio::ip::udp;

public:
  udp_receive_socket(boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_socket{boost::asio::make_strand(ctx)}
  {
  }

  udp_receive_socket(const inbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_endpoint{boost::asio::ip::make_address(conf.bind), conf.port}
      , m_socket{boost::asio::make_strand(ctx)}
  {
  }

  ~udp_receive_socket() = default;

  void assign(int sock) { m_socket.assign(boost::asio::ip::udp::v4(), sock); }
  void open()
  {
    m_socket.open(boost::asio::ip::udp::v4());
    m_socket.bind(m_endpoint);
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
        [this, f](auto ec, std::size_t sz) {
      if(ec == boost::asio::error::operation_aborted)
        return;

      if(!ec && sz > 0)
      {
        try
        {
          f(m_data, sz);
        }
        catch(const std::exception& e)
        {
          ossia::logger().error("[udp_socket::receive]: {}", e.what());
        }
        catch(...)
        {
          ossia::logger().error("[udp_socket::receive]: unknown error");
        }
      }

      this->receive(f);
        });
  }

  Nano::Signal<void()> on_close;

  boost::asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
  alignas(16) char m_data[65535];
};

class udp_send_socket
{
  using proto = boost::asio::ip::udp;

public:
  udp_send_socket(const outbound_socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_endpoint{boost::asio::ip::make_address(conf.host), conf.port}
      , m_socket{boost::asio::make_strand(ctx)}
      , m_broadcast{conf.broadcast}
  {
  }

  udp_send_socket(
      const boost::asio::ip::address& host, const uint16_t port,
      boost::asio::io_context& ctx)
      : m_context{ctx}
      , m_endpoint{host, port}
      , m_socket{boost::asio::make_strand(ctx)}
  {
  }

  void connect()
  {
    m_socket.open(boost::asio::ip::udp::v4());

    m_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));

    if(m_broadcast)
      m_socket.set_option(boost::asio::socket_base::broadcast(true));
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

  void write(const proto::endpoint& ep, const char* data, std::size_t sz)
  {
    boost::system::error_code ec;
    m_socket.send_to(boost::asio::const_buffer(data, sz), ep, 0, ec);
  }

  void write(const char* data, std::size_t sz) { write(m_endpoint, data, sz); }

  Nano::Signal<void()> on_close;

  boost::asio::io_context& m_context;
  proto::endpoint m_endpoint;
  proto::socket m_socket;
  bool m_broadcast{};
};

}
