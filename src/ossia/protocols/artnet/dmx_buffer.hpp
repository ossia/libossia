#pragma once
#include <ossia/detail/config.hpp>

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

  // Some data is reserved for the case where the last parameter
  // has a bit depth of 255.. and some padding for safety
  uint8_t data[DMX_CHANNEL_COUNT + 16];
  bool dirty{};
};

}
