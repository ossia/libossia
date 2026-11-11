#include "dmxusbpro_protocol.hpp"

#include <ossia/protocols/artnet/dmx_parameter.hpp>

#include <algorithm>

namespace ossia::net
{
dmxusbpro_protocol::dmxusbpro_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf,
    const ossia::net::serial_configuration& socket, int version)
    : dmx_output_protocol_base{ctx, conf}
    , m_port{ctx->context}
    , m_version{version}
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

  switch(m_version)
  {
    case open_dmx_usb:
    case dmx_usb_pro_mk1:
      break;
    case dmx_usb_pro_mk2: {
      // SEND_DMX_PORT1: 0x06,
      // SEND_DMX_PORT2: 0xCA,
      // SET_API_KEY: 0x0D,
      // SET_PORT_ASSIGNMENT: 0x93
      static constexpr unsigned char set_api_key[]{0x7E, 0x0D, 0x04, 0x00, 0xC9,
                                                   0xA4, 0x03, 0xE4, 0xE7};
      boost::asio::write(m_port, boost::asio::buffer(set_api_key));

      static constexpr unsigned char set_port_assignment[]{0x7E, 0x93, 0x02, 0x00,
                                                           0x01, 0x01, 0xE7};
      boost::asio::write(m_port, boost::asio::buffer(set_port_assignment));
    }
  }
}

dmxusbpro_protocol::~dmxusbpro_protocol()
{
  stop_processing();
}

void dmxusbpro_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);

  switch(m_version)
  {
    case dmx_usb_pro_mk1:
      m_timer.start([this] { this->update_function_dmxusbpro(0x06, 0); });
      break;
    case dmx_usb_pro_mk2: {
      // Universe 0 is the first output port, universe 1 the second one.
      m_timer.start([this] {
        for(int i = 0; i < m_buffer.universes(); i++)
        {
          const int universe = this->m_conf.start_universe + i;
          if(universe > 1)
            break;
          this->update_function_dmxusbpro(universe == 0 ? 0x06 : 0xCA, i);
        }
      });
      break;
    }
    case open_dmx_usb:
      m_timer.start([this] { this->update_function_opendmx(); });
      break;
  }
}

void dmxusbpro_protocol::update_function_dmxusbpro(uint8_t command, int universe)
{
  const auto buffer = m_buffer.read_universe(universe);
  boost::system::error_code ec;
  // https://cdn.enttec.com/pdf/assets/70304/70304_DMX_USB_PRO_API.pdf
  // 1: 0x7E
  // 1: message code: 0x6 for DMX send / 0xCA for DMX send on port 2 (DMX USB PRO Mk2)
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

  unsigned char buf[buffer_size]{0x7E, command, data_size_lsb, data_size_msb, 0};

  std::copy_n(buffer.data(), std::min<std::size_t>(buffer.size(), channels), buf + 5);
  buf[buffer_size - 1] = 0xE7;

  boost::asio::write(m_port, boost::asio::buffer(buf));
  if(ec)
    ossia::logger().error("DMX write failure");
}

void dmxusbpro_protocol::update_function_opendmx()
{
  const auto buffer = m_buffer.read_universe(0);
  boost::system::error_code ec;
  // <serial break>
  // 0
  // <512 channels>
  m_port.send_break(ec);
  if(ec)
    return;

  constexpr uint32_t buffer_size = 1 + 512;

  unsigned char buf[buffer_size]{};
  buf[0] = 0; // DMX start code
  std::copy_n(buffer.data(), std::min<std::size_t>(buffer.size(), 512), buf + 1);

  boost::asio::write(m_port, boost::asio::buffer(buf), ec);
  if(ec)
    ossia::logger().error("DMX write failure");
}
}
