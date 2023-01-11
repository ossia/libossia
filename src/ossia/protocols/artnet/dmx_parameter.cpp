#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "dmx_parameter.hpp"

#include <ossia/protocols/artnet/dmx_buffer.hpp>

#include <cstdio>

namespace ossia::net
{
template <std::size_t I>
struct artnet_visitor
{
  static_assert(I >= 1);
  static_assert(I <= 4);
  dmx_parameter& self;
  void apply(uint32_t bytes) const noexcept
  {
    if constexpr(I == 1)
    {
      self.m_buffer.data[self.m_channel] = bytes;
    }
    else if constexpr(I == 2)
    {
      self.m_buffer.data[self.m_channel + 1] = bytes & 0x000000FF;
      self.m_buffer.data[self.m_channel + 0] = (bytes & 0x0000FF00) >> 8;
    }
    else if constexpr(I == 3)
    {
      self.m_buffer.data[self.m_channel + 2] = bytes & 0x000000FF;
      self.m_buffer.data[self.m_channel + 1] = (bytes & 0x0000FF00) >> 8;
      self.m_buffer.data[self.m_channel + 0] = (bytes & 0x00FF0000) >> 16;
    }
    else if constexpr(I == 4)
    {
      self.m_buffer.data[self.m_channel + 3] = bytes & 0x000000FF;
      self.m_buffer.data[self.m_channel + 2] = (bytes & 0x0000FF00) >> 8;
      self.m_buffer.data[self.m_channel + 1] = (bytes & 0x00FF0000) >> 16;
      self.m_buffer.data[self.m_channel + 0] = (bytes & 0xFF000000) >> 24;
    }
  }
  void operator()(int v) const noexcept { return apply(v); }
  void operator()(float v) const noexcept { return apply(v); }
  template <typename... Args>
  void operator()(Args&&...) const noexcept
  {
  }
};

template <>
struct artnet_visitor<1>
{
  dmx_parameter& self;
  void operator()(int v) const noexcept
  {
    self.m_buffer.data[self.m_channel] = std::clamp(v, 0, 255);
  }
  void operator()(float v) const noexcept
  {
    self.m_buffer.data[self.m_channel] = std::clamp(v, 0.f, 255.f);
  }
  template <typename... Args>
  void operator()(Args&&...) const noexcept
  {
  }
};
dmx_parameter::dmx_parameter(
    net::node_base& node, dmx_buffer& buffer, const unsigned int channel, int min,
    int max, int8_t bytes)
    : device_parameter(
        node, val_type::INT, bounding_mode::CLIP, access_mode::SET,
        make_domain(min, max))
    , m_bytes{bytes}
    , m_buffer{buffer}
    , m_channel{channel}
{
}

dmx_parameter::~dmx_parameter() = default;

void dmx_parameter::device_update_value()
{
  switch(m_bytes)
  {
    case 1:
      m_current_value.apply(artnet_visitor<1>{*this});
      break;
    case 2:
      m_current_value.apply(artnet_visitor<2>{*this});
      break;
    case 3:
      m_current_value.apply(artnet_visitor<3>{*this});
      break;
    case 4:
      m_current_value.apply(artnet_visitor<4>{*this});
      break;
  }

  m_buffer.dirty = true;
}

/*
ossia::domain make_domain_from_range(const artnet_range& r)
{
  std::vector<std::string> strs;
  for(auto& [str, min, max] : r)
    strs.push_back(str);
  return ossia::make_domain(std::move(strs));
}

artnet_range_parameter::artnet_range_parameter(
    net::node_base& node, dmx_buffer& buffer, unsigned int channel,
artnet_range r) : device_parameter( node, val_type::STRING,
bounding_mode::CLIP, access_mode::SET, make_domain_from_range(r)) ,
m_buffer{buffer} , m_channel{channel} , m_range{std::move(r)}
{
}

artnet_range_parameter::~artnet_range_parameter()
{
}

struct artnet_range_visitor
{
  artnet_range_parameter::dmx_buffer& buf;
  const unsigned int channel;
  const artnet_range& range;
  void operator()(int v) const noexcept
  {
    if(v >= 0 && v < int(range.size()))
    {
      buf.data[channel] = range[v].min;
      buf.dirty = true;
    }
  }

  void operator()(const std::string& v) const noexcept
  {
    auto it = ossia::find_if(range, [&] (const auto& r) { return r.name == v;
}); if(it != range.end())
    {
      buf.data[channel] = it->min;
      buf.dirty = true;
    }
  }

  template<typename... Args>
  void operator()(Args&&...) const noexcept
  {

  }
};

void artnet_range_parameter::device_update_value()
{
  value().apply(artnet_range_visitor{m_buffer, m_channel, m_range});
}
*/
}
#endif
