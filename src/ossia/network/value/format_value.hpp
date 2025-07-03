#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_HAS_FMT)
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/fmt.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <fmt/ranges.h>
FMT_BEGIN_NAMESPACE
#if FMT_VERSION >= 100000
namespace detail {
template <typename T> class is_container_adaptor_like<boost::container::flat_set<T>>
    : public std::false_type {

};
}
#endif
FMT_END_NAMESPACE
namespace ossia
{
template <typename FmtCtx>
struct value_prettyprint_visitor
{
  FmtCtx& ctx;
  using iterator = typename FmtCtx::iterator;
  iterator operator()(impulse) const;
  iterator operator()(int32_t i) const;
  iterator operator()(float f) const;
  iterator operator()(bool b) const;
  iterator operator()(char c) const;
  iterator operator()(std::string str) const;
  iterator operator()(vec2f vec) const;
  iterator operator()(vec3f vec) const;
  iterator operator()(vec4f vec) const;
  iterator operator()(const std::vector<ossia::value>& t) const;
  iterator operator()(const value_map_type& t) const;
  iterator operator()() const;
};
}

namespace fmt
{
template <typename T>
struct formatter<std::optional<T>>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::optional<T>& n, FormatContext& ctx) const
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
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ossia::value& v, FormatContext& ctx) const
  {
    return v.apply(ossia::value_prettyprint_visitor<FormatContext>{ctx});
  }
};

}

namespace ossia
{

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(impulse) const
{
  return fmt::format_to(ctx.out(), "impulse");
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(int32_t i) const
{
  return fmt::format_to(ctx.out(), "int: {}", i);
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(float f) const
{
  return fmt::format_to(ctx.out(), "float: {:.2f}", f);
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(bool b) const
{
  return fmt::format_to(ctx.out(), "bool: {}", b ? "true" : "false");
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(char c) const
{
  return fmt::format_to(ctx.out(), "char: '{}'", c);
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(std::string str) const
{
  boost::algorithm::replace_all(str, "\"", "\\\"");
  return fmt::format_to(ctx.out(), "string: \"{}\"", str);
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(vec2f vec) const
{
  return fmt::format_to(ctx.out(), "vec2f: [{:.2f}, {:.2f}]", vec[0], vec[1]);
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(vec3f vec) const
{
  return fmt::format_to(
      ctx.out(), "vec3f: [{:.2f}, {:.2f}, {:.2f}]", vec[0], vec[1], vec[2]);
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(vec4f vec) const
{
  return fmt::format_to(
      ctx.out(), "vec4f: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]", vec[0], vec[1], vec[2],
      vec[3]);
}
template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(const std::vector<value>& t) const
{
  fmt::format_to(ctx.out(), "list: [");
  int i = 0;
  for(auto& v : t)
  {
    v.apply(*this);
    if(++i < std::ssize(t))
      fmt::format_to(ctx.out(), ", ");
  }
  return fmt::format_to(ctx.out(), "]");
}

template <typename FmtCtx>
inline typename FmtCtx::iterator
value_prettyprint_visitor<FmtCtx>::operator()(const value_map_type& t) const
{
  fmt::format_to(ctx.out(), "map: {{");
  int i = 0;
  for(auto& v : t)
  {
    fmt::format_to(ctx.out(), "\"{}\": ", v.first);
    v.second.apply(*this);
    if(++i < std::ssize(t))
      fmt::format_to(ctx.out(), ", ");
  }
  return fmt::format_to(ctx.out(), "}}");
}

template <typename FmtCtx>
inline typename FmtCtx::iterator value_prettyprint_visitor<FmtCtx>::operator()() const
{
  return fmt::format_to(ctx.out(), "invalid");
}

}
#endif
