#pragma once
#include <ossia/editor/dataspace/angle.hpp>
#include <ossia/editor/dataspace/distance.hpp>
#include <ossia/editor/dataspace/orientation.hpp>
#include <ossia/editor/dataspace/position.hpp>
#include <ossia/editor/dataspace/speed.hpp>
#include <ossia/editor/dataspace/color.hpp>

#include <brigand/algorithms/transform.hpp>
namespace ossia
{


template<typename T>
using add_value = brigand::transform<T, brigand::bind<ossia::strong_value, brigand::_1>>;
using unit_t = eggs::variant<distance_u, position_u, speed_u, orientation_u, angle_u, color_u>;


using angle = add_value<angle_u>;
using distance = add_value<distance_u>;
using orientation = add_value<orientation_u>;
using position = add_value<position_u>;
using color = add_value<color_u>;
using speed = add_value<speed_u>;

using value_with_unit = brigand::transform<unit_t, brigand::bind<ossia::add_value, brigand::_1>>;
}
