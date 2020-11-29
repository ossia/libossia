#pragma once
#include <cinttypes>

namespace ossia::gfx
{
struct port_index
{
  int32_t node{};
  int32_t port{};

  constexpr auto operator==(port_index other) const noexcept
  {
    return node == other.node && port == other.port;
  }
  constexpr auto operator!=(port_index other) const noexcept
  {
    return node != other.node || port != other.port;
  }
  constexpr auto operator<(port_index other) const noexcept
  {
    return node < other.node || (node == other.node && port < other.port);
  }
};
}
