#pragma once
#include <ossia/editor/dataspace/dataspace_base_fwd.hpp>
#include <ossia/editor/dataspace/dataspace_base.hpp>
#include <eggs/variant.hpp>
#include <brigand/algorithms/transform.hpp>
#include <brigand/functions/arithmetic.hpp>
#include <brigand/adapted/list.hpp>
#include <type_traits>
#include <ratio>

namespace brigand
{
template <typename... T>
using eggs_variant_wrapper = typename eggs::variant<T...>;

template <typename L>
using as_eggs_variant = brigand::wrap<L, eggs_variant_wrapper>;
}

namespace ossia
{
template<typename T>
using add_value = brigand::transform<T, brigand::bind<ossia::strong_value, brigand::_1>>;

using angle = add_value<angle_u>;
using distance = add_value<distance_u>;
using orientation = add_value<orientation_u>;
using position = add_value<position_u>;
using color = add_value<color_u>;
using speed = add_value<speed_u>;
using gain = add_value<gain_u>;
using time = add_value<time_u>;

#if defined(_MSC_VER)
inline void swap(ossia::distance& lhs, ossia::distance& rhs);
inline void swap(ossia::position& lhs, ossia::position& rhs);
inline void swap(ossia::angle& lhs, ossia::angle& rhs);
inline void swap(ossia::orientation& lhs, ossia::orientation& rhs);
inline void swap(ossia::color& lhs, ossia::color& rhs);
inline void swap(ossia::gain& lhs, ossia::gain& rhs);
inline void swap(ossia::time& lhs, ossia::time& rhs);
inline void swap(ossia::speed& lhs, ossia::speed& rhs);
#endif
}