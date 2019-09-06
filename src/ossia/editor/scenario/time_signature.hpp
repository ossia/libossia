#pragma once
#include <ossia/detail/config.hpp>
#include <cinttypes>

namespace ossia
{
struct time_signature {
  uint16_t upper{4};
  uint16_t lower{4};

  friend bool operator==(time_signature lhs, time_signature rhs) noexcept
  { return lhs.upper == rhs.upper && lhs.lower == rhs.lower;}
  friend bool operator!=(time_signature lhs, time_signature rhs) noexcept
  { return lhs.upper != rhs.upper || lhs.lower != rhs.lower;}
};
}
