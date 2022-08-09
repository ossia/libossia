#pragma once

#include <ossia/detail/config.hpp>

#if defined(__clang__)
#define OSSIA_MAXIMUM_INLINE OSSIA_INLINE __attribute__((flatten))
#else
#define OSSIA_MAXIMUM_INLINE OSSIA_INLINE
#endif

#if __has_include(<boost/variant2.hpp>)
#include <boost/variant2.hpp>
namespace ossia_variant_alias = boost::variant2;
#else
#include <mpark/variant.hpp>
namespace ossia_variant_alias = mpark;
#endif

namespace ossia
{
using monostate = ossia_variant_alias::monostate;
#if __cpp_concepts >= 201907L
template <typename T>
concept nothrow_move_constructible = std::is_nothrow_move_constructible_v<T>;

template <nothrow_move_constructible... Args>
#else
template <typename... Args>
#endif
using variant = ossia_variant_alias::variant<Args...>;

template <typename... Args>
using slow_variant = ossia_variant_alias::variant<Args...>;

template <std::size_t N, typename T>
using variant_element = ossia_variant_alias::variant_alternative<N, T>;
template <std::size_t N, typename T>
using variant_element_t = ossia_variant_alias::variant_alternative_t<N, T>;

using ossia_variant_alias::operator==;
using ossia_variant_alias::operator!=;
using ossia_variant_alias::operator<;
using ossia_variant_alias::operator>;
using ossia_variant_alias::operator<=;
using ossia_variant_alias::operator>=;

// using boost::variant2::in_place;
// using ossia_variant_alias::in_place;
using ossia_variant_alias::get;
using ossia_variant_alias::get_if;
using ossia_variant_alias::in_place_index;
using ossia_variant_alias::in_place_type;
using ossia_variant_alias::visit;
}
