#pragma once
#include <ossia/network/sockets/configuration.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/local/datagram_protocol.hpp>
#include <boost/asio/placeholders.hpp>

#include <nano_signal_slot.hpp>
#include <ossia/detail/logger.hpp>

namespace ossia::net
{

class udp_receive_socket
{
  using proto = boost::asio::ip::udp;
public:
  udp_receive_socket(const socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context {ctx}
      , m_endpoint {boost::asio::ip::make_address(conf.host), conf.port}
      , m_socket {ctx}
  {
  }

  void open()
  {
    m_socket.open(boost::asio::ip::udp::v4());
    m_socket.bind(m_endpoint);
  }

  void close()
  {
    m_context.post([this] {
      m_socket.close();
      on_close();
    });
  }

  template <typename F>
  void receive(F f)
  {
    m_socket.async_receive_from(
        boost::asio::buffer(m_data), m_endpoint,
        [this, f](auto ec, std::size_t sz) {
          if (ec == boost::asio::error::operation_aborted)
            return;

          if (!ec && sz > 0)
          {
            try
            {
              f(m_data, sz);
            }
            catch (const std::exception& e)
            {
              ossia::logger().error("[udp_socket::receive]: {}", e.what());
            }
            catch (...)
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
  udp_send_socket(const socket_configuration& conf, boost::asio::io_context& ctx)
      : m_context {ctx}
      , m_endpoint {boost::asio::ip::make_address(conf.host), conf.port}
      , m_socket {ctx}
  {
  }

  void connect()
  {
    m_socket.open(boost::asio::ip::udp::v4());
  }

  void close()
  {
    m_context.post([this] {
      m_socket.close();
      on_close();
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
};

}
