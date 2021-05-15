#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "dmx_parameter.hpp"
#include <ossia/protocols/artnet/dmx_buffer.hpp>

#include <cstdio>

namespace ossia::net
{
struct artnet_visitor
{
  dmx_buffer& buf;
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

dmx_parameter::dmx_parameter(
    net::node_base& node, dmx_buffer& buffer, const unsigned int channel, int min, int max)
    : device_parameter(
          node, val_type::INT, bounding_mode::CLIP, access_mode::SET,
          make_domain(min, max))
    , m_buffer{buffer}
    , m_channel{channel}
{
}

dmx_parameter::~dmx_parameter()
{
}

void dmx_parameter::device_update_value()
{
  value().apply(artnet_visitor{m_buffer, m_channel});
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
    net::node_base& node, dmx_buffer& buffer, unsigned int channel, artnet_range r)
    : device_parameter(
          node, val_type::STRING, bounding_mode::CLIP, access_mode::SET,
          make_domain_from_range(r))
    , m_buffer{buffer}
    , m_channel{channel}
    , m_range{std::move(r)}
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
    auto it = ossia::find_if(range, [&] (const auto& r) { return r.name == v; });
    if(it != range.end())
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
