#include "dmx_protocol_base.hpp"

#include <ossia/detail/fmt.hpp>
#include <ossia/network/common/node_visitor.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>

namespace ossia::net
{

dmx_protocol_base::dmx_protocol_base(
    ossia::net::network_context_ptr ctx, const dmx_config& conf)
    : protocol_base{flags{}}
    , m_context{ctx}
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

dmx_output_protocol_base::dmx_output_protocol_base(
    ossia::net::network_context_ptr ctx, const dmx_config& conf)
    : dmx_protocol_base{ctx, conf}
    , m_timer{ctx->context}
{
}

void dmx_output_protocol_base::stop_processing()
{
  m_timer.stop();
}

void dmx_input_protocol_base::create_channel_map()
{
  m_cache.fill(nullptr);

  ossia::net::visit_parameters(
      m_device->get_root_node(),
      [this](ossia::net::node_base&, ossia::net::parameter_base& param) {
    auto p = static_cast<dmx_parameter*>(&param);
    if(auto c = p->channel(); c >= 0 && c < m_cache.size())
      m_cache[p->channel()] = p;
      });
}

void dmx_input_protocol_base::on_dmx(const uint8_t* dmx, int count)
{
  for(int i = 0; i < count; i++)
  {
    this->m_cache[i]->set_dmx_value(dmx + i, dmx + count);
  }
}
}
