#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "dmx_led_parameter.hpp"

#include <ossia/protocols/artnet/dmx_buffer.hpp>

#include <cstdio>

namespace ossia::net
{

dmx_led_parameter::dmx_led_parameter(
    net::node_base& node, dmx_buffer& buffer, const unsigned int channel,
    int8_t bytes_per_pixel, int num_pixels)
    : device_parameter(
          node, val_type::LIST, bounding_mode::CLIP, access_mode::SET,
          make_domain(0, 255))
    , m_buffer{buffer}
    , m_channel{channel}
    , m_bpp{bytes_per_pixel}
    , m_num_pixels{num_pixels}
{
  this->m_current_value
      = std::vector<ossia::value>(bytes_per_pixel * num_pixels, int(0));
}

dmx_led_parameter::~dmx_led_parameter() = default;

void dmx_led_parameter::device_update_value()
{
  // ossia -> dmx out
  auto value = m_current_value.target<std::vector<ossia::value>>();
  if(!value)
    return;

  auto data = m_buffer.write_channels(m_channel, m_bpp * m_num_pixels);
  int N = std::min(data.size(), value->size());
  for(int i = 0; i < N; i++)
  {
    data[i] = ossia::convert<int>((*value)[i]);
  }
}

}
#endif
