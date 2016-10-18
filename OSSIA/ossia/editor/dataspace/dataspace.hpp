#pragma once
#include <ossia/editor/dataspace/dataspace_fwd.hpp>
#include <ossia/editor/dataspace/angle.hpp>
#include <ossia/editor/dataspace/distance.hpp>
#include <ossia/editor/dataspace/orientation.hpp>
#include <ossia/editor/dataspace/position.hpp>
#include <ossia/editor/dataspace/speed.hpp>
#include <ossia/editor/dataspace/color.hpp>
#include <ossia/editor/dataspace/gain.hpp>
#include <ossia/editor/dataspace/temperature.hpp>
#include <ossia/editor/dataspace/time.hpp>

namespace ossia
{

template <typename T>
struct is_unit
{
    template <typename U>
    static char test(typename U::is_unit* x);

    template <typename U>
    static long test(U* x);

    static const bool value = sizeof(test<T>(0)) == 1;
};
template<typename T>
constexpr bool is_unit_v = is_unit<T>::value;

template<typename T, typename U>
typename std::enable_if_t<is_unit_v<T> && is_unit_v<U>, bool> operator==(T, U) { return false; }
template<typename T, typename U>
typename std::enable_if_t<is_unit_v<T> && is_unit_v<U>, bool> operator!=(T, U) { return true; }

template<typename T>
typename std::enable_if_t<is_unit_v<T>, bool> operator==(T, T) { return true; }
template<typename T>
typename std::enable_if_t<is_unit_v<T>, bool> operator!=(T, T) { return false; }

using unit_sizes =
  brigand::transform<
    brigand::as_list<ossia::unit_t>, brigand::bind<brigand::size, brigand::_1>>;

using unit_count = brigand::fold<
  unit_sizes,
  brigand::uint64_t<0>,
  brigand::plus<brigand::_state, brigand::_element>>;
}
