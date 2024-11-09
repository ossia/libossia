#pragma once
#include <ossia/detail/config.hpp>

namespace ossia::net
{
struct dense_configuration
{
  std::chrono::microseconds interval{};
};

struct dense_packet
{
  // All the OSC addresses's values sorted
  struct
  {
    uint32_t protocol_code{};
  } header;

  unsigned char data[1248];

  // protocol:
  // int, float, etc..: as-is
  // vec: uint16 type + uint16 sz + N elements
  // map: uint16 type + uint16 sz + N [k:v]
};
static_assert(sizeof(dense_packet) + 28 <= 1280);

}
