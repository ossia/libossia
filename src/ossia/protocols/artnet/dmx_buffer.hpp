#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/span.hpp>

#include <boost/dynamic_bitset.hpp>

#include <cstdint>

#define DMX_CHANNEL_COUNT 512

namespace ossia::net
{

struct dmx_config
{
  uint32_t frequency{};
  uint16_t universe{};

  enum
  {
    no_auto,
    channel_index, // "Channel-123"
    just_index,    // "123"
    just_universes, // no individual dmx channels, instead create one 512-length array per universe
  } autocreate{no_auto};

  enum
  {
    source, // ossia sends DX
    sink    // ossia receives DMX
  } mode{source};

  bool multicast{true};
  bool start_universe_is_one{};
};

struct dmx_buffer
{
  dmx_buffer();
  ~dmx_buffer();

  static constexpr auto universe_size = DMX_CHANNEL_COUNT;

  int universes() const noexcept { return dirty.size(); }
  void set_universe_count(int universes)
  {
    // Some data is reserved for the case where the last parameter
    // has a bit depth of 255.. and some padding for safety
    data.resize(universes * universe_size + 255 + 16);
    dirty.resize(universes);
  }

  tcb::span<uint8_t> write_channels(int start, int count) noexcept
  {
    int first_universe = start / universe_size;
    int last_universe = (start + count) / universe_size;
    assert(first_universe >= 0);
    assert(first_universe < dirty.size());
    assert(last_universe >= 0);
    assert(last_universe < dirty.size());

    for(int i = first_universe; i <= last_universe; i++)
      dirty[i] = true;
    return {data.data() + start, std::size_t(count)};
  }

  tcb::span<uint8_t> write_universe(int u) noexcept
  {
    if(data.size() < (u + 1) * universe_size)
      return {};
    dirty[u] = true;
    return {data.data() + u * universe_size, universe_size};
  }

  tcb::span<const uint8_t> read_universe(int u) const noexcept
  {
    if(data.size() < (u + 1) * universe_size)
      return {};
    dirty[u] = false;
    return {data.data() + u * universe_size, universe_size};
  }

  ossia::pod_vector<uint8_t> data;
  mutable boost::dynamic_bitset<> dirty;
};

}
