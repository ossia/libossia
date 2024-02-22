#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "artnet_protocol.hpp"

#include <ossia/detail/fmt.hpp>
#include <ossia/protocols/artnet/dmx_parameter.hpp>

#include <artnet/artnet.h>

#include <chrono>

namespace ossia::net
{
dmx_buffer::dmx_buffer()
    : dirty(false)
{
  std::memset(data, 0, DMX_CHANNEL_COUNT);
}

dmx_buffer::~dmx_buffer() = default;

artnet_protocol::artnet_protocol(
    ossia::net::network_context_ptr ctx, const dmx_config& conf, std::string_view host)
    : dmx_protocol_base{ctx, conf}
{
  if(conf.frequency < 1 || conf.frequency > 44)
    throw std::runtime_error("DMX 512 update frequency must be in the range [1, 44] Hz");

  m_timer.set_delay(std::chrono::milliseconds{
      static_cast<int>(1000.0f / static_cast<float>(conf.frequency))});

  //  44  hz limit apply because we send 512 byte frames.
  //  It seem to be possible to send only some value and thus
  //   update at higher frequencies => Work TODO

  //  Do not specify ip address for now, artnet will choose one
#if defined(_NDEBUG)
  bool verbose = 0;
#else
  bool verbose = 1;
#endif
  m_node = artnet_new(host.data(), verbose);

  if(m_node == NULL)
    throw std::runtime_error("Artnet new failed");

  static constexpr int artnet_port_id = 0;
  artnet_set_port_type(m_node, artnet_port_id, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(m_node, artnet_port_id, ARTNET_OUTPUT_PORT, m_conf.universe);

  artnet_set_short_name(m_node, "libossia");
  artnet_set_long_name(m_node, "libossia artnet protocol");
  artnet_set_node_type(m_node, ARTNET_RAW);

  artnet_dump_config(m_node);
  std::fflush(stdout);
  std::fflush(stderr);
  if(artnet_start(m_node) != ARTNET_EOK)
    throw std::runtime_error("Artnet Start failed");
}

artnet_protocol::~artnet_protocol()
{
  stop_processing();
  artnet_destroy(m_node);
}

void artnet_protocol::set_device(ossia::net::device_base& dev)
{
  dmx_protocol_base::set_device(dev);

  m_timer.start([this] { this->update_function(); });
}

void artnet_protocol::update_function()
{
  if(m_buffer.dirty)
  {
    artnet_raw_send_dmx(m_node, m_conf.universe, DMX_CHANNEL_COUNT, m_buffer.data);
    m_buffer.dirty = false;
  }
}
}

#endif
