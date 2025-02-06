#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/detail/flat_map.hpp>
#include <ossia/network/common/device_parameter.hpp>

#include <cstdint>

namespace ossia::net
{
struct dmx_buffer;
class OSSIA_EXPORT dmx_led_parameter : public device_parameter
{
public:
  dmx_led_parameter(
      net::node_base& node, dmx_buffer& buffer, const unsigned int channel,
      int8_t bytes_per_pixel, int num_pixels);
  ~dmx_led_parameter();
  int8_t m_bytes{};

private:
  void device_update_value() override;

  dmx_buffer& m_buffer;
  const uint32_t m_channel{};

  int8_t m_bpp{};
  int m_num_pixels{};
};

}
#endif
