#include "dmx_protocol_base.hpp"

#include <ossia/detail/fmt.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>

namespace ossia::net
{

dmx_protocol_base::dmx_protocol_base(
    ossia::net::network_context_ptr ctx, const dmx_config& conf)
    : protocol_base{flags{}}
    , m_context{ctx}
    , m_timer{ctx->context}
    , m_conf{conf}
{
}

dmx_protocol_base::~dmx_protocol_base() { }
bool dmx_protocol_base::pull(net::parameter_base& param)
{
  return true;
}

bool dmx_protocol_base::push(const net::parameter_base& param, const ossia::value& v)
{
  return true;
}

bool dmx_protocol_base::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool dmx_protocol_base::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool dmx_protocol_base::update(ossia::net::node_base&)
{
  return true;
}

void dmx_protocol_base::stop_processing()
{
  m_timer.stop();
}

void dmx_protocol_base::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  if(m_conf.autocreate != m_conf.no_auto)
  {
    auto& root = dev.get_root_node();
    for(unsigned int i = 0; i < DMX_CHANNEL_COUNT; ++i)
    {
      auto name = m_conf.autocreate == m_conf.channel_index
                      ? fmt::format("Channel-{}", i + 1)
                      : std::to_string(i + 1);

      device_parameter::create_device_parameter<dmx_parameter>(
          root, name, 0, m_buffer, i);
    }
  }
}
}
