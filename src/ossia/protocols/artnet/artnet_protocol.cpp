#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "artnet_protocol.hpp"

#include <ossia/detail/fmt.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>

#include <artnet/artnet.h>

#include <chrono>

#define ARTNET_NODE_SHORT_NAME "libossia"
#define ARTNET_NODE_LONG_NAME "Libossia Artnet Protocol"

namespace ossia::net
{
dmx_buffer::dmx_buffer()
    : dirty(false)
{
  std::memset(data, 0, DMX_CHANNEL_COUNT);
}

dmx_buffer::~dmx_buffer() = default;

static constexpr int artnet_port_id = 0;
artnet_protocol::artnet_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf)
    : protocol_base{flags{}}
    , m_context{ctx}
    , m_timer{ctx->context}
    , m_autocreate{conf.autocreate}
{
  if(conf.frequency < 1 || conf.frequency > 44)
    throw std::runtime_error("DMX 512 update frequency must be in the range [1, 44] Hz");

  m_timer.set_delay(std::chrono::milliseconds{
      static_cast<int>(1000.0f / static_cast<float>(conf.frequency))});

  //  44  hz limit apply because we send 512 byte frames.
  //  It seem to be possible to send only some value and thus
  //   update at higher frequencies => Work TODO

  //  Do not specify ip address for now, artnet will choose one
  m_node = artnet_new(nullptr, 1);

  if(m_node == NULL)
    throw std::runtime_error("Artnet new failed");
  m_universe = conf.universe;

  artnet_set_port_type(m_node, artnet_port_id, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(m_node, artnet_port_id, ARTNET_OUTPUT_PORT, m_universe);

  artnet_set_short_name(m_node, ARTNET_NODE_SHORT_NAME);
  artnet_set_long_name(m_node, ARTNET_NODE_LONG_NAME);
  artnet_set_node_type(m_node, ARTNET_RAW);

  artnet_dump_config(m_node);
  if(artnet_start(m_node) != ARTNET_EOK)
    throw std::runtime_error("Artnet Start failed");
}

artnet_protocol::~artnet_protocol()
{
  m_timer.stop();
  artnet_destroy(m_node);
}

void artnet_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  if(m_autocreate)
  {
    auto& root = dev.get_root_node();
    for(unsigned int i = 0; i < DMX_CHANNEL_COUNT; ++i)
      device_parameter::create_device_parameter<dmx_parameter>(
          root, fmt::format("Channel-{}", i + 1), 0, m_buffer, i);
  }

  m_timer.start([this] { this->update_function(); });
}

bool artnet_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool artnet_protocol::push(const net::parameter_base& param, const ossia::value& v)
{
  return true;
}

bool artnet_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool artnet_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool artnet_protocol::update(ossia::net::node_base&)
{
  return true;
}

void artnet_protocol::update_function()
{
  if(m_buffer.dirty)
  {
    artnet_raw_send_dmx(m_node, m_universe, DMX_CHANNEL_COUNT, m_buffer.data);
    m_buffer.dirty = false;
  }
}
}

#endif
