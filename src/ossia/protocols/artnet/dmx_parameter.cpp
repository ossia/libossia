#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include "dmx_parameter.hpp"

#include <ossia/protocols/artnet/dmx_buffer.hpp>

#include <cstdio>

namespace ossia::net
{
template <std::size_t I>
struct artnet_in_visitor
{
  static_assert(I >= 1);
  static_assert(I <= 4);

  const uint8_t* start{};
  void apply(uint32_t& res) const noexcept
  {
    if constexpr(I == 1)
    {
      res += *start;
    }
    else if constexpr(I == 2)
    {
      res += *(start + 1);
      res += *(start + 0) << 8;
    }
    else if constexpr(I == 3)
    {
      res += *(start + 2);
      res += *(start + 1) << 8;
      res += *(start + 0) << 16;
    }
    else if constexpr(I == 4)
    {
      res += *(start + 3);
      res += *(start + 2) << 8;
      res += *(start + 1) << 16;
      res += *(start + 0) << 24;
    }
  }

  void operator()(int& v) const noexcept
  {
    uint32_t vv = 0;
    apply(vv);
    v = vv;
  }
  void operator()(float& v) const noexcept
  {
    uint32_t vv = 0;
    apply(vv);
    v = vv;
  }
  void operator()(bool& v) const noexcept
  {
    uint32_t vv = 0;
    apply(vv);
    v = vv;
  }
  template <typename... Args>
  void operator()(Args&&...) const noexcept
  {
  }
};

template <>
struct artnet_in_visitor<1>
{
  const uint8_t* value{};
  void operator()(int& v) const noexcept { v = *value; }
  void operator()(float& v) const noexcept { v = *value; }
  void operator()(bool& v) const noexcept { v = *value > 0; }
  template <typename... Args>
  void operator()(const Args&...) const noexcept
  {
  }
};

template <std::size_t I>
struct artnet_out_visitor
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
  void operator()(bool v) const noexcept { return apply(v ? 0xFFFFFFFF : 0x0); }
  template <typename... Args>
  void operator()(Args&&...) const noexcept
  {
  }
};

struct artnet_out_var_visitor
{
  dmx_range_parameter& self;
  void operator()(int v) const noexcept
  {
    auto& buf = self.m_buffer.data;
    for(int i = self.m_channel;
        i < self.m_channel + self.m_bytes && i < DMX_CHANNEL_COUNT; i++)
    {
      buf[i] = v;
    }
  }
  void operator()(float v) const noexcept { (*this)(int(v)); }
  void operator()(bool v) const noexcept { (*this)(v ? 255 : 0); }
  void operator()(const std::vector<ossia::value>& v) const noexcept
  {
    int N = v.size();
    auto& buf = self.m_buffer.data;
    int k = 0;
    for(int i = self.m_channel;
        i < self.m_channel + self.m_bytes && i < DMX_CHANNEL_COUNT && k < N; i++, k++)
    {
      buf[i] = ossia::convert<int>(v[k]);
    }
  }

  template <typename... Args>
  void operator()(const Args&...) const noexcept
  {
  }
};

template <>
struct artnet_out_visitor<1>
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
  void operator()(bool v) const noexcept
  {
    self.m_buffer.data[self.m_channel] = v ? 255 : 0;
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
    , m_channel{channel}
{
}

dmx_parameter::~dmx_parameter() = default;

void dmx_parameter::set_dmx_value(const uint8_t* start, const uint8_t* buffer_end)
{
  // dmx in -> ossia
  if(buffer_end - start < m_bytes)
    return;

  switch(m_bytes)
  {
    case 1:
      m_current_value.apply(artnet_in_visitor<1>{start});
      break;
    case 2:
      m_current_value.apply(artnet_in_visitor<2>{start});
      break;
    case 3:
      m_current_value.apply(artnet_in_visitor<3>{start});
      break;
    case 4:
      m_current_value.apply(artnet_in_visitor<4>{start});
      break;
  }
  push_value(m_current_value);
}

void dmx_parameter::device_update_value()
{
  // ossia -> dmx out
  switch(m_bytes)
  {
    case 1:
      m_current_value.apply(artnet_out_visitor<1>{*this});
      break;
    case 2:
      m_current_value.apply(artnet_out_visitor<2>{*this});
      break;
    case 3:
      m_current_value.apply(artnet_out_visitor<3>{*this});
      break;
    case 4:
      m_current_value.apply(artnet_out_visitor<4>{*this});
      break;
  }

  m_buffer.dirty[0] = true;
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
  m_buffer.dirty[0] = true;
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
  void apply(uint32_t bytes) const noexcept { self.m_param.push_value((int)bytes); }
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
    std::vector<std::pair<std::string, uint8_t>> values)
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
