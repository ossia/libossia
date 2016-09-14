#pragma once
#include <ossia/editor/dataspace/angle.hpp>
#include <ossia/editor/dataspace/distance.hpp>
#include <ossia/editor/dataspace/orientation.hpp>
#include <ossia/editor/dataspace/position.hpp>
#include <ossia/editor/dataspace/speed.hpp>

namespace ossia
{

using unit = eggs::variant<distance_u, position_u, speed_u, orientation_u>;


// TODO with some functional map on types this could be easier to write
using value_with_unit = eggs::variant<distance, position, speed, orientation>;
}
