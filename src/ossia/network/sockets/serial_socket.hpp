#pragma once
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/writers.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>

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
      : m_context {ctx}, m_conf{std::move(conf)}, m_port{ctx}, m_decoder{this->m_port}
  {
  }

  void connect()
  {
    m_port.open(m_conf.port);

    m_port.set_option(proto::baud_rate(m_conf.baud_rate));
    m_port.set_option(proto::character_size(m_conf.character_size));
    m_port.set_option(proto::flow_control(static_cast<proto::flow_control::type>(m_conf.flow_control)));
    m_port.set_option(proto::parity(static_cast<proto::parity::type>(m_conf.parity)));
    m_port.set_option(proto::stop_bits(static_cast<proto::stop_bits::type>(m_conf.stop_bits)));

    m_context.post([this] {
      on_open();
    });
  }

  template <typename F>
  void receive(F f)
  {
    m_decoder.receive(stream_processor<serial_socket, F>{*this, std::move(f)});
  }

  void close()
  {
    if(m_port.is_open())
    {
      m_context.post([this] {
        m_port.close();
        on_close();
      });
    }
  }

  void write(const char* data, std::size_t sz)
  {
    encoder{this->m_port}.write(data, sz);
  }

  bool connected() const noexcept
  {
    return true;
  }

  Nano::Signal<void()> on_open;
  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;

  boost::asio::io_context& m_context;
  serial_configuration m_conf;
  boost::asio::serial_port m_port;
  decoder m_decoder;
};
}
