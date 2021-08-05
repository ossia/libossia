#pragma once
#include <ossia/detail/config.hpp>
#include <string_view>
#include <optional>
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

using quarter_note = double;

inline std::optional<ossia::time_signature>
get_time_signature(const std::string_view& v)
{
  constexpr const auto is_valid_denom = [](int denom) {
    return denom == 1 || denom == 2 || denom == 4 || denom == 8 || denom == 16
           || denom == 32 || denom == 64;
  };

  try
  {
    if (v.size() >= 3)
    {
      auto it = v.find('/');
      if (it > 0 && it < (v.size() - 1))
      {
        std::string_view num{v.data(), it};
        std::string_view denom{v.data() + it + 1, v.size() - it};
        int num_n = std::stoi(std::string(num)); // OPTIMIZEME
        int denom_n = std::stoi(std::string(denom));

        if (num_n >= 1 && num_n <= 512 && is_valid_denom(denom_n))
          return ossia::time_signature{(uint16_t)num_n, (uint16_t)denom_n};
      }
    }
  }
  catch (...)
  {
  }
  return {};
}
}
