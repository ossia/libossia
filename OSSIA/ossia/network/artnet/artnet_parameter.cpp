#include "artnet_parameter.hpp"

#include <cstdio>

namespace ossia
{
namespace net
{

artnet_parameter::artnet_parameter(
    net::node_base& node, dmx_buffer* buffer, const unsigned int channel)
    : device_parameter(
          node, val_type::INT, bounding_mode::CLIP, access_mode::SET,
          make_domain(0, 255))
    , m_buffer(*buffer)
    , m_channel(channel)
{
}

artnet_parameter::~artnet_parameter()
{
}

struct artnet_visitor
{
  artnet_parameter::dmx_buffer& buf;
  const unsigned int channel;
  void operator()(int v) const noexcept
  {
    buf.data[channel] = v;
    buf.dirty = true;
  }
  void operator()(float v) const noexcept
  {
    buf.data[channel] = v;
    buf.dirty = true;
  }
  void operator()(char v) const noexcept
  {
    buf.data[channel] = v;
    buf.dirty = true;
  }
  template<typename... Args>
  void operator()(Args&&...) const noexcept
  {

  }
};

void artnet_parameter::device_update_value()
{
  value().apply(artnet_visitor{m_buffer, m_channel});
}
}
}
