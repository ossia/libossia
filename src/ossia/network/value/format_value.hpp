#pragma once
#include <ossia/detail/fmt.hpp>
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
  auto format(const std::optional<T>& n, FormatContext &ctx)
  {
    if(n)
    {
      return format_to(ctx.out(), "{}", *n);
    }
    else
    {
      return format_to(ctx.out(), "none");
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
  out = format_to(out, "[");

  if (v.size() > 0)
  {
    auto it = v.begin();
    out = format_to(out, "{}", *it);
    ++it;

    for (; it != v.end(); ++it)
    {
      out = format_to(out, ", {}", *it);
    }
  }
  return format_to(out, "]");
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

template <typename T>
struct formatter<std::vector<T>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const std::vector<T>& v, FormatContext &ctx)
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
    out = format_to(out, "[");
    const auto n = v.size();
    if (n > 0)
    {
      auto it = v.begin();
      out = format_to(out, "{}", *it);
      ++it;

      for (; it != v.end(); ++it)
      {
        out = format_to(out, ", {}", *it);
      }
    }
    return format_to(out, "]");
  }
};

template <>
struct formatter<ossia::value>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const ossia::value& v, FormatContext &ctx) {
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
