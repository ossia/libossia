#pragma once
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>

#include <oscpack/osc/OscTypes.h>

#include <any>
#include <string_view>

namespace ossia::net
{
// 0 -> 4 ; 1 -> 4; 2 -> 4; 3 -> 4;
// 4 -> 8 ; 5 -> 8; etc ...
// an optimized version of
//
// std::size_t pattern_size(std::size_t sz) noexcept
// {
//   switch(sz % 4) {
//   case 0:
//     return sz + 4;
//   case 1:
//     return sz + 3;
//   case 2:
//     return sz + 2;
//   case 3:
//     return sz + 1;
//   }
//   return 0;
// }

static inline constexpr std::size_t pattern_size(std::size_t sz) noexcept
{
  return (sz & 0xFFFFFFFFFFFFFFFC) + 4;
}

static inline std::size_t write_string(std::string_view str, char* buffer) noexcept
{
  std::size_t i = 0;
  for(; i < str.size(); i++)
  {
    buffer[i] = str[i];
  }

  if(i % 4 == 0)
    buffer[i++] = 0;

  for(; i % 4 != 0; i++)
  {
    buffer[i] = 0;
  }
  return i;
}

static inline bool is_blob(const ossia::net::parameter_base& b) noexcept
{
  using namespace std::literals;
  auto& ext = b.get_node().get_extended_attributes();
  auto it = ext.find("extended_type"sv);
  if(it == ext.end())
    return false;

  if(auto* str = std::any_cast<ossia::extended_type>(&it->second))
    return (*str) == "buffer"sv; // TODO keep in sync with extended_types.cpp

  return false;
}

static inline bool is_rgba(const ossia::net::parameter_base& b) noexcept
{
  auto& u = b.get_unit();
  return u == ossia::rgba8_u{};
}

static inline bool is_blob(const ossia::net::full_parameter_data& b) noexcept
{
  using namespace std::literals;
  auto& ext = b.extended;
  auto it = ext.find("extended_type"sv);
  if(it == ext.end())
    return false;

  if(auto* str = std::any_cast<ossia::extended_type>(&it->second))
    return (*str) == "buffer"sv; // TODO keep in sync with extended_types.cpp

  return false;
}

static inline bool is_rgba(const ossia::net::full_parameter_data& b) noexcept
{
  return b.unit == ossia::rgba8_u{};
}

static inline constexpr oscpack::RgbaColor to_osc_rgba(const ossia::rgba8& u) noexcept
{
  uint32_t r = (uint32_t)u.dataspace_value[0] << 24;
  uint32_t g = (uint32_t)u.dataspace_value[1] << 16;
  uint32_t b = (uint32_t)u.dataspace_value[2] << 8;
  uint32_t a = (uint32_t)u.dataspace_value[3];
  return oscpack::RgbaColor(r + g + b + a);
}
}
