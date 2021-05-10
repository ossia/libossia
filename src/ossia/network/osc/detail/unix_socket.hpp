#pragma once
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <asio/placeholders.hpp>

namespace ossia::net
{
#if defined(ASIO_HAS_LOCAL_SOCKETS)
class unix_socket
{
  using proto = asio::local::datagram_protocol;

public:
  unix_socket(std::string_view path, asio::io_context& ctx)
      : m_context {ctx}, m_endpoint {path}, m_socket {ctx}
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
#endif
}
