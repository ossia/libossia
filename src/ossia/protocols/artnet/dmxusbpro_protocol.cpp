#include "dmxusbpro_protocol.hpp"

#include <ossia/protocols/artnet/dmx_parameter.hpp>

namespace ossia::net
{
dmxusbpro_protocol::dmxusbpro_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf,
    const ossia::net::serial_configuration& socket)
    : protocol_base{flags{}}
    , m_context{ctx}
    , m_timer{ctx->context}
    , m_port{ctx->context}
    , m_autocreate{conf.autocreate}
{
  if(conf.frequency < 1 || conf.frequency > 44)
    throw std::runtime_error("DMX 512 update frequency must be in the range [1, 44] Hz");

  {
    auto& m_conf = socket;
    using proto = boost::asio::serial_port;
    m_port.open(m_conf.port);

    m_port.set_option(proto::baud_rate(m_conf.baud_rate));
    m_port.set_option(proto::character_size(m_conf.character_size));
    m_port.set_option(proto::flow_control(
        static_cast<proto::flow_control::type>(m_conf.flow_control)));
    m_port.set_option(proto::parity(static_cast<proto::parity::type>(m_conf.parity)));
    m_port.set_option(
        proto::stop_bits(static_cast<proto::stop_bits::type>(m_conf.stop_bits)));
  }

  m_timer.set_delay(std::chrono::milliseconds{
      static_cast<int>(1000.0f / static_cast<float>(conf.frequency))});
}

dmxusbpro_protocol::~dmxusbpro_protocol()
{
  m_timer.stop();
}

void dmxusbpro_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  if(m_autocreate)
  {
    auto& root = dev.get_root_node();
    for(unsigned int i = 0; i < 254; ++i)
      device_parameter::create_device_parameter<dmx_parameter>(
          root, fmt::format("{}", i + 1), 0, m_buffer, i);
  }

  m_timer.start([this] { this->update_function(); });
}

bool dmxusbpro_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool dmxusbpro_protocol::push(const net::parameter_base& param, const ossia::value& v)
{
  return true;
}

bool dmxusbpro_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool dmxusbpro_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool dmxusbpro_protocol::update(ossia::net::node_base&)
{
  return true;
}

void dmxusbpro_protocol::update_function()
{
  try
  {
    if(true || m_buffer.dirty)
    {
      // 126 6 (channel nb+1) 0 0 (channel1 channel2 channel3 …) 231
      constexpr unsigned char channels = 254;
      constexpr int bufsize = 5 + channels + 1;
      unsigned char buf[bufsize]{126, 6, channels + 1, 0, 0};

      for(int i = 0; i < channels; i++)
        buf[5 + i] = m_buffer.data[i];
      buf[bufsize - 1] = 231;

      boost::asio::write(m_port, boost::asio::buffer(buf));

      m_buffer.dirty = false;
    }
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
