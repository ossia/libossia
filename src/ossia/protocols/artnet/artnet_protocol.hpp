#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/protocols/artnet/dmx_protocol_base.hpp>

using artnet_node = void*;

namespace ossia::net
{
struct dmx_config;
class OSSIA_EXPORT artnet_protocol final : public dmx_protocol_base
{
public:
  artnet_protocol(ossia::net::network_context_ptr, const dmx_config& conf);
  ~artnet_protocol();

  void set_device(ossia::net::device_base& dev) override;

private:
  void update_function();

  artnet_node m_node;
};

}

#endif
