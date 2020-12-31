#pragma once
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

static constexpr std::size_t pattern_size(std::size_t sz) noexcept
{
  return (sz & 0xFFFFFFFFFFFFFFFC) + 4;
}

static std::size_t write_string(std::string_view str, char* buffer) noexcept
{
  std::size_t i = 0;
  for (; i < str.size(); i++) {
    buffer[i] = str[i];
  }

  if (i%4 == 0)
    buffer[i++] = 0;

  for (; i % 4 != 0; i++) {
    buffer[i] = 0;
  }
  return i;
}
}
