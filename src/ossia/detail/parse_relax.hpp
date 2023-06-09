#pragma once
#include <ossia/detail/config.hpp>

#include <optional>
#include <string_view>
#include <version>

#if defined(__cpp_lib_to_chars)
#include <charconv>
#else
#include <boost/spirit/home/x3.hpp>
#endif

namespace ossia
{
// parse_relax:
// "123" => 123
// "123x" => 123
// " 123" => std::nullopt
// "123 " => 123

#if defined(__cpp_lib_to_chars)
template <typename T>
static std::optional<T> parse_relax(std::string_view v)
{
  T n{};

  const auto begin = v.data();
  const auto end = v.data() + v.size();
  const auto [ptr, ec] = std::from_chars(begin, end, n);
  return (ec == std::errc{}) ? std::optional<T>{n} : std::nullopt;
}
#else
template <typename T>
static std::optional<T> parse_relax(std::string_view v);

template <>
std::optional<int> parse_relax<int>(std::string_view v)
{
  using boost::spirit::x3::int_;
  int32_t x{};
  int err = boost::spirit::x3::parse(v.begin(), v.end(), int_, x);
  if(err == 0)
    return std::nullopt;
  return x;
}
template <>
std::optional<float> parse_relax<float>(std::string_view v)
{
  using boost::spirit::x3::float_;
  float x{};
  int err = boost::spirit::x3::parse(v.begin(), v.end(), float_, x);
  if(err == 0)
    return std::nullopt;
  return x;
}
template <>
std::optional<double> parse_relax<double>(std::string_view v)
{
  using boost::spirit::x3::double_;
  double x{};
  int err = boost::spirit::x3::parse(v.begin(), v.end(), double_, x);
  if(err == 0)
    return std::nullopt;
  return x;
}
#endif

}
