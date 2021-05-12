#pragma once
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/writers.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/placeholders.hpp>

#include <nano_signal_slot.hpp>

namespace ossia::net
{

template<typename Framing>
class serial_socket
{
public:
  using proto = boost::asio::serial_port;
  using socket = boost::asio::serial_port;
  using encoder = typename Framing::template encoder<boost::asio::serial_port>;
  using decoder = typename Framing::template decoder<boost::asio::serial_port>;

  serial_socket(const serial_configuration& conf, boost::asio::io_context& ctx)
      : m_context {ctx}, m_path{conf.port}, m_port{ctx}, m_decoder{this->m_port}
  {
  }

  template <typename F>
  void listen(F f)
  {
    m_port.open(m_path);

    m_decoder.receive(stream_processor<serial_socket, F>{*this, std::move(f)});
  }

  void close()
  {
    m_context.post([this] {
      m_port.close();
      on_close();
    });
  }

  void write(const char* data, std::size_t sz)
  {
    encoder{this->m_port}.write(data, sz);
  }

  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;

  boost::asio::io_context& m_context;
  std::string m_path;
  boost::asio::serial_port m_port;
  decoder m_decoder;
};
}
