#pragma once
#include <fmt/format.h>
#include <ossia/network/value/value.hpp>
#include <ossia/detail/optional.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <flat/flat_set.hpp>
namespace ossia
{
template <typename FormatContext>
struct value_prettyprint_visitor
{
  FormatContext& ctx;
  auto operator()(impulse) const
  {
    return fmt::format_to(ctx.begin(), "impulse");
  }
  auto operator()(int32_t i) const
  {
    return fmt::format_to(ctx.begin(), "int: {}", i);
  }
  auto operator()(float f) const
  {
    return fmt::format_to(ctx.begin(), "float: {}", f);
  }
  auto operator()(bool b) const
  {
    return fmt::format_to(ctx.begin(), "bool: {}", b ? "true" : "false");
  }
  auto operator()(char c) const
  {
    return fmt::format_to(ctx.begin(), "char: '{}'", c);
  }
  auto operator()(std::string str) const
  {
    boost::algorithm::replace_all(str, "\"", "\\\"");
    return fmt::format_to(ctx.begin(), "string: \"{}\"", str);
  }
  auto operator()(vec2f vec) const
  {
    return fmt::format_to(ctx.begin(), "vec2f: {}", vec);
  }
  auto operator()(vec3f vec) const
  {
    return fmt::format_to(ctx.begin(), "vec3f: {}", vec);
  }
  auto operator()(vec4f vec) const
  {
    return fmt::format_to(ctx.begin(), "vec4f: {}", vec);
  }
  auto operator()(const std::vector<ossia::value>& t) const
  {
    return fmt::format_to(ctx.begin(), "list: {}", t);
  }
  auto operator()() const
  {
    return fmt::format_to(ctx.begin(), "invalid");
  }
};

}

namespace fmt
{
template <typename T>
struct formatter<ossia::optional<T>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const ossia::optional<T>& n, FormatContext &ctx)
  {
    if(n)
    {
      return format_to(ctx.begin(), "{}", *n);
    }
    else
    {
      return format_to(ctx.begin(), "none");
    }
  }
};

template <typename T, std::size_t N>
struct formatter<std::array<T, N>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const std::array<T, N>& v, FormatContext &ctx)
  {
    auto out = ctx.out();
    format_to(out, "[{}", v[0]);
    for (std::size_t i = 1; i < N; i++)
      format_to(out, ", {}", v[i]);
    format_to(out, "]");
    return ctx.out();
  }
};

template<typename T, typename F>
auto format_vec(const T& v, F& ctx)
{
  auto out = ctx.out();
  format_to(out, "[");

  if (v.size() > 0)
  {
    auto it = v.begin();
    format_to(out, "{}", *it);
    ++it;

    for (; it != v.end(); ++it)
    {
      format_to(out, ", {}", *it);
    }
  }
  format_to(out, "]");
  return ctx.out();
}

template <>
struct formatter<std::vector<ossia::value>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const std::vector<ossia::value>& v, FormatContext &ctx)
  {
    return format_vec(v, ctx);
  }
};

template <>
struct formatter<std::vector<fc::flat_set<std::vector<ossia::value>>>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const std::vector<fc::flat_set<std::vector<ossia::value>>>& v, FormatContext &ctx)
  {
    return format_vec(v, ctx);
  }
};
template <typename T, typename Compare>
struct formatter<fc::flat_set<T, Compare>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const fc::flat_set<T, Compare>& v, FormatContext &ctx)
  {
    auto out = ctx.out();
    format_to(out, "[");
    const auto n = v.size();
    if (n > 0)
    {
      auto it = v.begin();
      format_to(out, "{}", *it);
      ++it;

      for (; it != v.end(); ++it)
      {
        format_to(out, ", {}", *it);
      }
    }
    format_to(out, "]");
    return ctx.out();
  }
};

template <>
struct formatter<ossia::value>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const ossia::value& v, FormatContext &ctx) {
    return v.apply(ossia::value_prettyprint_visitor<FormatContext>{ctx});
  }
};


}
