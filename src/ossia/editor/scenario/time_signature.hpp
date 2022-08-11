#pragma once
#include <ossia/detail/config.hpp>

#include <cinttypes>
#include <optional>
#include <string>
#include <string_view>

namespace ossia
{
struct time_signature
{
  uint16_t upper{4};
  uint16_t lower{4};

  friend bool operator==(time_signature lhs, time_signature rhs) noexcept
  {
    return lhs.upper == rhs.upper && lhs.lower == rhs.lower;
  }
  friend bool operator!=(time_signature lhs, time_signature rhs) noexcept
  {
    return lhs.upper != rhs.upper || lhs.lower != rhs.lower;
  }
};

using quarter_note = double;

OSSIA_EXPORT
std::optional<ossia::time_signature> get_time_signature(const std::string_view& v);
}
