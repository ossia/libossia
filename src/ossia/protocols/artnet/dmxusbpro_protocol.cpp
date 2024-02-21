#include "dmxusbpro_protocol.hpp"

#include <ossia/protocols/artnet/dmx_parameter.hpp>

namespace ossia::net
{
dmxusbpro_protocol::dmxusbpro_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf,
    const ossia::net::serial_configuration& socket)
    : dmx_protocol_base{ctx, conf}
    , m_port{ctx->context}
{
  if(conf.frequency < 1 || conf.frequency > 44)
    throw std::runtime_error("DMX 512 update frequency must be in the range [1, 44] Hz");

  {
    using proto = boost::asio::serial_port;
    m_port.open(socket.port);

    m_port.set_option(proto::baud_rate(socket.baud_rate));
    m_port.set_option(proto::character_size(socket.character_size));
    m_port.set_option(proto::flow_control(
        static_cast<proto::flow_control::type>(socket.flow_control)));
    m_port.set_option(proto::parity(static_cast<proto::parity::type>(socket.parity)));
    m_port.set_option(
        proto::stop_bits(static_cast<proto::stop_bits::type>(socket.stop_bits)));
  }

  m_timer.set_delay(std::chrono::milliseconds{
      static_cast<int>(1000.0f / static_cast<float>(conf.frequency))});
}

dmxusbpro_protocol::~dmxusbpro_protocol()
{
  stop_processing();
}

void dmxusbpro_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);
  m_timer.start([this] { this->update_function(); });
}

void dmxusbpro_protocol::update_function()
{
  try
  {
    // https://cdn.enttec.com/pdf/assets/70304/70304_DMX_USB_PRO_API.pdf
    // 1: 0x7E
    // 1: message code (0x6 for DMX send)
    // 1: size LSB
    // 1: size MSB
    // N: message data: [
    //   1: start code (0x0 for DMX)
    //   N-1: DMX channels
    // ]
    // 1: 0xE7

    constexpr uint32_t channels = 512;
    constexpr uint32_t data_size = channels + 1;
    constexpr uint32_t buffer_size = 4 + data_size + 1;

    constexpr uint8_t data_size_lsb = data_size & 0x00FF;
    constexpr uint8_t data_size_msb = (data_size & 0xFF00) >> 8;

    unsigned char buf[buffer_size]{0x7E, 0x6, data_size_lsb, data_size_msb, 0};

    for(uint32_t i = 0; i < channels; i++)
      buf[5 + i] = m_buffer.data[i];
    buf[buffer_size - 1] = 0xE7;

    boost::asio::write(m_port, boost::asio::buffer(buf));

    m_buffer.dirty = false;
  }
  catch(std::exception& e)
  {
    ossia::logger().error("write failure: {}", e.what());
  }
  catch(...)
  {
    ossia::logger().error("write failure");
  }
}

}
