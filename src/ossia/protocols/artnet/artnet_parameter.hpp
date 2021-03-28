#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/protocols/artnet/artnet_protocol.hpp>
#include <ossia/network/common/device_parameter.hpp>

#include <ossia/detail/flat_map.hpp>

#include <cstdint>

namespace ossia
{
namespace net
{

class OSSIA_EXPORT artnet_parameter : public device_parameter
{
  using dmx_buffer = artnet_protocol::dmx_buffer;

public:
  artnet_parameter(
      net::node_base& node, dmx_buffer& buffer, const unsigned int channel, int min = 0, int max = 255);
  ~artnet_parameter();

private:
  void device_update_value() override;

  dmx_buffer& m_buffer;
  const unsigned int m_channel{};

  friend struct artnet_visitor;
};

/*
struct artnet_range_element {
  std::string name;
  int min{0};
  int max{512};
};

class artnet_range_parameter : public device_parameter
{
  using dmx_buffer = artnet_protocol::dmx_buffer;

public:
  artnet_range_parameter(
      net::node_base& node, dmx_buffer& buffer, unsigned int channel, int min, int max);
  ~artnet_range_parameter();

private:
  void device_update_value() override;

  dmx_buffer& m_buffer;
  const unsigned int m_channel{};

  friend struct artnet_range_visitor;
};
*/
}
}
#endif
