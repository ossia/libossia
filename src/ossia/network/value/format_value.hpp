#pragma once
#include <ossia/detail/fmt.hpp>
#include <fmt/ranges.h>
#include <ossia/network/value/value.hpp>
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/optional.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace ossia
{
using fmt_ctx = fmt::v8::basic_format_context<fmt::v8::appender, char>;

struct value_prettyprint_visitor
{
  fmt_ctx& ctx;
  fmt_ctx::iterator operator()(impulse) const;
  fmt_ctx::iterator operator()(int32_t i) const;
  fmt_ctx::iterator operator()(float f) const;
  fmt_ctx::iterator operator()(bool b) const;
  fmt_ctx::iterator operator()(char c) const;
  fmt_ctx::iterator operator()(std::string str) const;
  fmt_ctx::iterator operator()(vec2f vec) const;
  fmt_ctx::iterator operator()(vec3f vec) const;
  fmt_ctx::iterator operator()(vec4f vec) const;
  fmt_ctx::iterator operator()(const std::vector<ossia::value>& t) const;
  fmt_ctx::iterator operator()() const;
};
}

namespace fmt
{
template <typename T>
struct formatter<std::optional<T>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const std::optional<T>& n, FormatContext &ctx) const
  {
    if(n)
    {
      return fmt::format_to(ctx.out(), "{}", *n);
    }
    else
    {
      return fmt::format_to(ctx.out(), "none");
    }
  }
};

template <>
struct formatter<ossia::value>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const ossia::value& v, FormatContext &ctx) const {
    return v.apply(ossia::value_prettyprint_visitor{ctx});
  }
};

}

namespace ossia
{

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(impulse) const
{
  return fmt::format_to(ctx.out(), "impulse");
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(int32_t i) const
{
  return fmt::format_to(ctx.out(), "int: {}", i);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(float f) const
{
  return fmt::format_to(ctx.out(), "float: {}", f);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(bool b) const
{
  return fmt::format_to(ctx.out(), "bool: {}", b ? "true" : "false");
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(char c) const
{
  return fmt::format_to(ctx.out(), "char: '{}'", c);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(std::string str) const
{
  boost::algorithm::replace_all(str, "\"", "\\\"");
  return fmt::format_to(ctx.out(), "string: \"{}\"", str);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(vec2f vec) const
{
  return fmt::format_to(ctx.out(), "vec2f: {}", vec);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(vec3f vec) const
{
  return fmt::format_to(ctx.out(), "vec3f: {}", vec);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(vec4f vec) const
{
  return fmt::format_to(ctx.out(), "vec4f: {}", vec);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()(const std::vector<value>& t) const
{
  return fmt::format_to(ctx.out(), "list: {}", t);
}

inline fmt_ctx::iterator value_prettyprint_visitor::operator()() const
{
  return fmt::format_to(ctx.out(), "invalid");
}

}
