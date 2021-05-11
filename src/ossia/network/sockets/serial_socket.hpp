#pragma once
#include <asio/io_context.hpp>
#include <asio/serial_port.hpp>
#include <asio/placeholders.hpp>

namespace ossia::net
{

template<typename Framing>
class serial_socket
{
public:
  using proto = asio::serial_port;
  using socket = asio::serial_port;
  using encoder = typename Framing::template encoder<asio::serial_port>;
  using decoder = typename Framing::template decoder<asio::serial_port>;

  serial_socket(std::string_view path, asio::io_context& ctx)
      : m_context {ctx}, m_path(path), m_port{ctx}, m_decoder{this->m_port}
  {
  }

  template <typename F>
  void listen(F f)
  {
    m_port.open(m_path);

    m_decoder.receive(std::move(f));
  }

  template <typename F>
  void close(F f)
  {
    m_context.post([this, f] {
      m_port.close();
      f();
    });
  }

  void write(const char* data, std::size_t sz)
  {
    encoder{this->m_port}.write(data, sz);
  }

  asio::io_context& m_context;
  std::string m_path;
  asio::serial_port m_port;

  decoder m_decoder;
};
}
