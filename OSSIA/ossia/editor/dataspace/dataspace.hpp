#pragma once
#include <ossia/editor/dataspace/angle.hpp>
#include <ossia/editor/dataspace/distance.hpp>
#include <ossia/editor/dataspace/orientation.hpp>
#include <ossia/editor/dataspace/position.hpp>
#include <ossia/editor/dataspace/speed.hpp>
#include <ossia/editor/dataspace/color.hpp>
#include <ossia/editor/dataspace/gain.hpp>
#include <ossia/editor/dataspace/temperature.hpp>
#include <ossia/editor/dataspace/time.hpp>

#include <brigand/algorithms/transform.hpp>
#include <type_traits>

namespace ossia
{


template<typename T>
using add_value = brigand::transform<T, brigand::bind<ossia::strong_value, brigand::_1>>;
using unit_t = eggs::variant<distance_u, position_u, speed_u, orientation_u, angle_u, color_u, gain_u>;


using angle = add_value<angle_u>;
using distance = add_value<distance_u>;
using orientation = add_value<orientation_u>;
using position = add_value<position_u>;
using color = add_value<color_u>;
using speed = add_value<speed_u>;
using gain = add_value<gain_u>;

using value_with_unit = brigand::transform<unit_t, brigand::bind<ossia::add_value, brigand::_1>>;


template <typename T>
struct is_unit
{
    template <typename U>
    static char test(typename U::neutral_unit* x);

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
}
