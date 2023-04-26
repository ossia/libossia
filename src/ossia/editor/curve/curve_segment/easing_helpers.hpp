#pragma once
#include <ossia/detail/typelist.hpp>
#include <ossia/editor/curve/curve_segment/easing.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
namespace ossia
{
// clang-format off
namespace easing {
using list = ossia::tl<
    linear,
    quadraticIn, quadraticOut, quadraticInOut,
    cubicIn, cubicOut, cubicInOut,
    quarticIn, quarticOut, quarticInOut,
    quinticIn, quinticOut, quinticInOut,
    sineIn, sineOut, sineInOut,
    circularIn, circularOut, circularInOut,
    exponentialIn, exponentialOut, exponentialInOut,
    elasticIn, elasticOut, elasticInOut,
    backIn, backOut, backInOut,
    bounceIn, bounceOut, bounceInOut,
    perlinInOut
>;

constexpr void for_each(auto&& func)  {
  func.template operator()<
    linear,
    quadraticIn, quadraticOut, quadraticInOut,
    cubicIn, cubicOut, cubicInOut,
    quarticIn, quarticOut, quarticInOut,
    quinticIn, quinticOut, quinticInOut,
    sineIn, sineOut, sineInOut,
    circularIn, circularOut, circularInOut,
    exponentialIn, exponentialOut, exponentialInOut,
    elasticIn, elasticOut, elasticInOut,
    backIn, backOut, backInOut,
    bounceIn, bounceOut, bounceInOut,
    perlinInOut
  >();
}
}
// clang-format on

template <typename Easing, typename Y>
constexpr auto make_easing(Y start, Y end) noexcept
{
  return [start, end](Y ratio) constexpr noexcept {
    return easing::ease{}(start, end, Easing{}(ratio));
  };
}

template <typename Y>
constexpr void make_easing(auto& res, std::string_view easing, Y start, Y end) noexcept
{
  auto apply_if_easing = [&res, easing, start, end]<typename Easing>() noexcept {
    constexpr auto name = Easing::name();
    if(name == easing)
    {
      res = make_easing<Easing, Y>(start, end);
      return true;
    }
    return false;
  };
  easing::for_each([apply_if_easing]<typename... Easings>() {
    (apply_if_easing.template operator()<Easings>() || ...);
  });
}

template <typename Y>
constexpr void make_easing(auto& res, std::string_view easing) noexcept
{
  auto apply_if_easing = [&res, easing]<typename Easing>() noexcept {
    constexpr auto name = Easing::name();
    if(name == easing)
    {
      res = Easing{};
      return true;
    }
    return false;
  };
  easing::for_each([apply_if_easing]<typename... Easings>() {
    (apply_if_easing.template operator()<Easings>() || ...);
  });
}
}
