#pragma once
#include <ossia/detail/config.hpp>

#include <optional>
#include <string_view>
#include <version>

#if defined(__cpp_lib_to_chars)
#include <charconv>
#else
#include <boost/lexical_cast.hpp>
#endif

namespace ossia
{
// parse_strict:
// "123" => 123
// "123x" => nullopt
// " 123" => nullopt
// "123 " => nullopt

template <typename T>
static std::optional<T> parse_strict(std::string_view instance) noexcept
{
  T n{};
#if defined(__cpp_lib_to_chars)
  const auto begin = instance.data();
  const auto end = instance.data() + instance.size();
  const auto [ptr, ec] = std::from_chars(begin, end, n);
  return (ec == std::errc{} && ptr == end) ? std::optional<T>{n} : std::nullopt;
#else
  if(boost::conversion::detail::try_lexical_convert(instance, n))
    return n;
  return std::nullopt;
#endif
}
}
