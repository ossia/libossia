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
    just_index     // "123"
  } autocreate{no_auto};
  bool multicast{true};
};

struct dmx_buffer
{
  dmx_buffer();
  ~dmx_buffer();

  // Some data is reserved for the case where the last parameter
  // has a bit depth of 255.. and some padding for safety
  uint8_t data[DMX_CHANNEL_COUNT + 256 + 16];
  bool dirty{};
};

}
