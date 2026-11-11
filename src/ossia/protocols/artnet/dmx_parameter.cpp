#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "dmx_parameter.hpp"

#include <ossia/protocols/artnet/dmx_buffer.hpp>

#include <algorithm>
#include <cassert>
#include <limits>

namespace ossia::net
{
namespace
{
constexpr int dmx_clamp_bytes(int bytes) noexcept
{
  return std::clamp(bytes, 1, dmx_parameter::max_bytes);
}

constexpr uint32_t dmx_max_value(int bytes) noexcept
{
  return bytes >= 4 ? 0xFFFFFFFFu : (uint32_t{1} << (8 * bytes)) - 1;
}
}

struct artnet_in_visitor
{
  uint32_t value{};
  void operator()(int& v) const noexcept { v = value; }
  void operator()(float& v) const noexcept { v = value; }
  void operator()(bool& v) const noexcept { v = value > 0; }
  template <typename... Args>
  void operator()(Args&&...) const noexcept
  {
  }
};

struct artnet_out_visitor
{
  dmx_parameter& self;
  int bytes{};

  void write(uint32_t v) const noexcept
  {
    for(int k = 0; k < bytes; k++)
      self.m_buffer.write_channel(
          self.m_channels[k], (v >> (8 * (bytes - 1 - k))) & 0xFF);
  }

  void operator()(int v) const noexcept
  {
    write(std::clamp<int64_t>(v, 0, dmx_max_value(bytes)));
  }
  void operator()(float v) const noexcept
  {
    if(!(v > 0.f))
      write(0);
    else
      write(static_cast<uint32_t>(std::min<double>(v, dmx_max_value(bytes))));
  }
  void operator()(bool v) const noexcept { write(v ? dmx_max_value(bytes) : 0); }
  template <typename... Args>
  void operator()(const Args&...) const noexcept
  {
  }
};

struct artnet_out_var_visitor
{
  dmx_range_parameter& self;

  static uint8_t to_byte(int v) noexcept { return std::clamp(v, 0, 255); }

  void operator()(int v) const noexcept
  {
    std::ranges::fill(self.m_buffer.write_channels(self.m_channel, self.m_bytes), to_byte(v));
  }
  void operator()(float v) const noexcept { (*this)(int(v)); }
  void operator()(bool v) const noexcept { (*this)(v ? 255 : 0); }
  void operator()(const std::vector<ossia::value>& v) const noexcept
  {
    auto data = self.m_buffer.write_channels(self.m_channel, self.m_bytes);
    const std::size_t n = std::min(data.size(), v.size());
    for(std::size_t i = 0; i < n; i++)
      data[i] = to_byte(ossia::convert<int>(v[i]));
  }

  template <typename... Args>
  void operator()(const Args&...) const noexcept
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
{
  for(int k = 0; k < max_bytes; k++)
    m_channels[k] = channel + k;
}

dmx_parameter::dmx_parameter(
    net::node_base& node, dmx_buffer& buffer, std::span<const uint32_t> channels,
    int min, int max)
    : device_parameter(
          node, val_type::INT, bounding_mode::CLIP, access_mode::SET,
          make_domain(min, max))
    , m_bytes(dmx_clamp_bytes(channels.size()))
    , m_buffer{buffer}
{
  assert(!channels.empty());
  const int n = std::min<int>(channels.size(), max_bytes);
  std::copy_n(channels.begin(), n, m_channels.begin());
}

dmx_parameter::~dmx_parameter() = default;

void dmx_parameter::set_dmx_value(const uint8_t* dmx, const uint8_t* buffer_end)
{
  // dmx in -> ossia
  const auto count = buffer_end - dmx;
  const int bytes = dmx_clamp_bytes(m_bytes);
  uint32_t v = 0;
  for(int k = 0; k < bytes; k++)
  {
    if(m_channels[k] >= count)
      return;
    v = (v << 8) | dmx[m_channels[k]];
  }
  if(m_current_value.valid())
    m_current_value.apply(artnet_in_visitor{v});
  else
    m_current_value = int(v);
  push_value(m_current_value);
}

void dmx_parameter::device_update_value()
{
  // ossia -> dmx out
  m_current_value.apply(artnet_out_visitor{*this, dmx_clamp_bytes(m_bytes)});
}

dmx_range_parameter::dmx_range_parameter(
    net::node_base& node, dmx_buffer& buffer, dmx_range range, int min, int max)
    : device_parameter(
          node, val_type::LIST, bounding_mode::CLIP, access_mode::SET,
          make_domain(min, max))
    , m_bytes(range.num_bytes)
    , m_buffer{buffer}
    , m_channel(range.start)
{
}

dmx_range_parameter::~dmx_range_parameter() = default;
void dmx_range_parameter::device_update_value()
{
  m_current_value.apply(artnet_out_var_visitor{*this});
}

static ossia::domain_base<std::string> keys_to_domain(const auto& values)
{
  ossia::domain_base<std::string> ret;
  for(auto& [k, v] : values)
  {
    ret.values.push_back(k);
  }
  return ret;
}

struct artnet_enum_visitor
{
  dmx_enum_parameter& self;
  void apply(uint32_t bytes) const noexcept
  {
    self.m_param.push_value(int(std::min<uint32_t>(bytes, std::numeric_limits<int>::max())));
  }
  void operator()(int v) const noexcept { return apply(v); }
  void operator()(float v) const noexcept { return apply(v); }
  void operator()(const std::string& v) const noexcept
  {
    if(auto it = self.m_map.find(v); it != self.m_map.end())
    {
      apply(it->second);
    }
  }
  template <typename... Args>
  void operator()(const Args&...) const noexcept
  {
  }
};

dmx_enum_parameter::dmx_enum_parameter(
    net::node_base& node, dmx_parameter& p,
    std::vector<std::pair<std::string, uint32_t>> values)
    : device_parameter(
          node, val_type::STRING, bounding_mode::CLIP, access_mode::SET,
          keys_to_domain(values))
    , m_param{p}
{
  for(auto& [k, v] : values)
  {
    m_map[k] = v;
  }
}

dmx_enum_parameter::~dmx_enum_parameter() = default;

void dmx_enum_parameter::device_update_value()
{
  m_current_value.apply(artnet_enum_visitor{*this});
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
