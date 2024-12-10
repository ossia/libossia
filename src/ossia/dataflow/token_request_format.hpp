#pragma once
#include <ossia/dataflow/token_request.hpp>
#include <ossia/detail/fmt.hpp>
#if !defined(OSSIA_FREESTANDING)
namespace fmt
{
template <>
struct formatter<ossia::token_request> final : formatter<string_view>
{
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ossia::token_request& e, FormatContext& ctx) const
  {
    return fmt::format_to(
        ctx.out(), "{} -> {} | offset: {} | parent: {} | continuity: {} {}",
        e.prev_date.impl, e.date.impl, e.offset.impl, e.parent_duration.impl,
        e.start_discontinuous, e.end_discontinuous);
  }
};
}
#endif
