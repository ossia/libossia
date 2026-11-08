#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/pod_vector.hpp>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <memory>
#include <span>

#define DMX_CHANNEL_COUNT 512

namespace ossia::net
{

struct dmx_config
{
  uint32_t frequency{44};
  uint16_t start_universe{0};
  uint16_t universe_count{1};
  uint16_t channels_per_universe{512};

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
};

class dmx_dirty_flags
{
public:
  std::size_t size() const noexcept { return m_size; }

  void resize(std::size_t n)
  {
    auto flags = std::make_unique<std::atomic_bool[]>(n);
    for(std::size_t i = 0, N = std::min(n, m_size); i < N; i++)
      flags[i].store(m_flags[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
    m_flags = std::move(flags);
    m_size = n;
  }

  std::atomic_bool& operator[](std::size_t i) noexcept { return m_flags[i]; }
  const std::atomic_bool& operator[](std::size_t i) const noexcept { return m_flags[i]; }

  void reset() noexcept
  {
    for(std::size_t i = 0; i < m_size; i++)
      m_flags[i] = false;
  }

  bool none() const noexcept
  {
    for(std::size_t i = 0; i < m_size; i++)
      if(m_flags[i])
        return false;
    return true;
  }

private:
  std::unique_ptr<std::atomic_bool[]> m_flags;
  std::size_t m_size{};
};

struct OSSIA_EXPORT dmx_buffer
{
  explicit dmx_buffer(int universe_size);
  ~dmx_buffer();

  int universes() const noexcept { return dirty.size(); }
  void set_universe_count(int universes)
  {
    // Some data is reserved for the case where the last parameter
    // has a bit depth of 255.. and some padding for safety
    data.resize(universes * universe_size + 255 + 16);
    dirty.resize(universes);
  }

  std::span<uint8_t> write_channels(int start, int count) noexcept
  {
    if(start < 0 || count <= 0)
      return {};
    const std::size_t first_universe = start / universe_size;
    const std::size_t last_universe = (std::size_t(start) + count - 1) / universe_size;
    if(last_universe >= dirty.size())
      return {};

    for(std::size_t i = first_universe; i <= last_universe; i++)
      dirty[i] = true;
    return {data.data() + start, std::size_t(count)};
  }

  void write_channel(std::size_t channel, uint8_t value) noexcept
  {
    const std::size_t u = channel / universe_size;
    if(u >= dirty.size())
      return;
    data[channel] = value;
    dirty[u] = true;
  }

  std::span<uint8_t> write_universe(int u) noexcept
  {
    if(std::ssize(data) < (u + 1) * universe_size)
      return {};
    dirty[u] = true;
    return {data.data() + u * universe_size, (std::size_t)universe_size};
  }

  std::span<const uint8_t> read_universe(int u) const noexcept
  {
    if(std::ssize(data) < (u + 1) * universe_size)
      return {};
    dirty[u] = false;
    return {data.data() + u * universe_size, (std::size_t)universe_size};
  }

  ossia::pod_vector<uint8_t> data;
  mutable dmx_dirty_flags dirty;
  int universe_size{};
};

}
