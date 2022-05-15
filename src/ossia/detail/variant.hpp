#pragma once

#include <ossia/detail/config.hpp>

#if defined(__clang__)
#define OSSIA_MAXIMUM_INLINE OSSIA_INLINE __attribute__((flatten))
#else
#define OSSIA_MAXIMUM_INLINE OSSIA_INLINE
#endif

#include <mpark/variant.hpp>

//#include <boost/variant2.hpp>
//namespace mpark = boost::variant2;
namespace ossia
{
using monostate = mpark::monostate;
template<typename... Args>
using variant = mpark::variant<Args...>;


template<std::size_t N, typename T>
using variant_element = mpark::variant_alternative<N, T>;
template<std::size_t N, typename T>
using variant_element_t = mpark::variant_alternative_t<N, T>;

using mpark::operator==;
using mpark::operator!=;
using mpark::operator<;
using mpark::operator>;
using mpark::operator<=;
using mpark::operator>=;

using mpark::in_place;
using mpark::in_place_type;
using mpark::in_place_index;
using mpark::visit;
using mpark::get;
using mpark::get_if;
}
