#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "dmx_led_parameter.hpp"

#include <ossia/protocols/artnet/dmx_buffer.hpp>

#include <algorithm>

namespace ossia::net
{

dmx_led_parameter::dmx_led_parameter(
    net::node_base& node, dmx_buffer& buffer, const unsigned int channel,
    int8_t bytes_per_pixel, int num_pixels, bool reverse)
    : device_parameter(
          node, val_type::LIST, bounding_mode::CLIP, access_mode::SET,
          make_domain(0, 255))
    , m_buffer{buffer}
    , m_channel{channel}
    , m_bpp{bytes_per_pixel}
    , m_num_pixels{num_pixels}
    , m_reverse{reverse}
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
  const std::size_t N = std::min(data.size(), value->size());
  const std::size_t bpp = std::max<int>(m_bpp, 1);
  for(std::size_t i = 0; i < N; i++)
  {
    const uint8_t v = std::clamp(ossia::convert<int>((*value)[i]), 0, 255);
    if(m_reverse)
      data[data.size() - bpp * (1 + i / bpp) + i % bpp] = v;
    else
      data[i] = v;
  }
}

}
#endif
