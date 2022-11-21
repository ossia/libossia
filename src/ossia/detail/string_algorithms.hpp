#pragma once
#include <ossia/detail/string_view.hpp>

namespace ossia
{
constexpr bool string_starts_with(std::string_view src, std::string_view prefix) noexcept
{
#if defined(__cpp_lib_starts_ends_with) && __cpp_lib_starts_ends_with >= 201711L
  return src.starts_with(prefix);
#else
  return src.rfind(prefix, 0) == 0;
#endif
}
}
