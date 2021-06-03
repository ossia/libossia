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
  bool autocreate{true};
  bool multicast{true};
};

struct dmx_buffer
{
  dmx_buffer();
  ~dmx_buffer();

  uint8_t data[DMX_CHANNEL_COUNT];
  bool dirty{};
};

}
