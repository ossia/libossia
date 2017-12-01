#pragma once
#include <ossia/detail/config.hpp>
#include <string_view>
namespace ossia
{
using string_view = std::string_view;

// See also https://gist.github.com/klmr/2775736
template <std::size_t N>
constexpr ossia::string_view make_string_view(const char (&str)[N]) noexcept
{
  return {str, N - 1};
}
}
