#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_HAS_FMT)
#include <ossia/detail/fmt.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/value/format_value.hpp>

/**
 * \file ossia/network/common/debug.hpp
 *
 * Various utilities to debug ossia types. Uses fmtlib.
 *
 */

namespace fmt
{
template <>
struct formatter<ossia::access_mode>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ossia::access_mode e, FormatContext& ctx) const
  {
    string_view name = "unknown";
    switch(e)
    {
      case ossia::access_mode::BI:
        name = "BI";
        break;
      case ossia::access_mode::GET:
        name = "GET";
        break;
      case ossia::access_mode::SET:
        name = "SET";
        break;
    }
    return formatter<string_view>{}.format(name, ctx);
  }
};

template <>
struct formatter<ossia::bounding_mode> final : formatter<string_view>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ossia::bounding_mode e, FormatContext& ctx) const
  {
    string_view name = "unknown";
    switch(e)
    {
      case ossia::bounding_mode::FREE:
        name = "FREE";
        break;
      case ossia::bounding_mode::CLIP:
        name = "CLIP";
        break;
      case ossia::bounding_mode::WRAP:
        name = "WRAP";
        break;
      case ossia::bounding_mode::FOLD:
        name = "FOLD";
        break;
      case ossia::bounding_mode::CLAMP_LOW:
        name = "LOW";
        break;
      case ossia::bounding_mode::CLAMP_HIGH:
        name = "HIGH";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

template <>
struct formatter<ossia::domain> final : formatter<string_view>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ossia::domain& e, FormatContext& ctx) const
  {
    return formatter<string_view>::format(e.to_pretty_string(), ctx);
  }
};

template <>
struct formatter<ossia::net::node_base>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ossia::net::node_base& n, FormatContext& ctx) const
  {
    int num_indent = 0;
    auto parent = n.get_parent();
    while(parent != nullptr)
    {
      num_indent++;
      parent = parent->get_parent();
    }

    if(auto p = n.get_parameter())
    {
      return fmt::format_to(
          ctx.out(),
          "{:<{}} : {}, AccessMode({}), BoundingMode({}), Domain({}), "
          "Unit({})",
          n.get_name(), num_indent, p->value(), p->get_access(), p->get_bounding(),
          p->get_domain(), ossia::get_pretty_unit_text(p->get_unit()));
    }
    else
    {
      return fmt::format_to(ctx.out(), "{}{}", num_indent, n.get_name());
    }
  }
};
}

#endif

namespace ossia
{
struct domain;
namespace net
{
class node_base;

OSSIA_EXPORT
void debug_recursively(std::string& str, const ossia::net::node_base&);
}
}
