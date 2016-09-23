#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>

namespace ossia
{


template<typename T>
struct distance_ratio;
struct distance_dataspace;

using meter_u = distance_ratio<std::ratio<1>>;
using kilometer_u = distance_ratio<std::kilo>;
using decimeter_u = distance_ratio<std::deci>;
using centimeter_u = distance_ratio<std::centi>;
using millimeter_u = distance_ratio<std::milli>;
using micrometer_u = distance_ratio<std::micro>;
using nanometer_u = distance_ratio<std::nano>;
using picometer_u = distance_ratio<std::pico>;
using inch_u = distance_ratio<std::ratio<254, 10000>>;
using foot_u = distance_ratio<std::ratio<254 * 12, 10000>>;
using mile_u = distance_ratio<std::ratio<254 * 12 * 5280, 10000>>;

template<typename Impl>
struct distance_unit
{
  using neutral_unit = meter_u;
  using value_type = Float;
  using concrete_type = Impl;
  using dataspace_type = struct distance_dataspace;
};

template<typename T>
struct distance_ratio :
    public linear_unit<distance_unit<distance_ratio<T>>, T>
{
  using linear_unit<distance_unit<distance_ratio<T>>, T>::linear_unit;
};

template<>
class unit_trait<meter_u>
{
  static constexpr const auto text{ossia::make_array("m")};
};
template<>
class unit_trait<kilometer_u>
{
  static constexpr const auto text{ossia::make_array("m")};
};
template<>
class unit_trait<decimeter_u>
{
  static constexpr const auto text{ossia::make_array("dm")};
};
template<>
class unit_trait<centimeter_u>
{
  static constexpr const auto text{ossia::make_array("cm")};
};
template<>
class unit_trait<millimeter_u>
{
  static constexpr const auto text{ossia::make_array("mm")};
};
template<>
class unit_trait<micrometer_u>
{
  static constexpr const auto text{ossia::make_array("mm")};
};
template<>
class unit_trait<nanometer_u>
{
  static constexpr const auto text{ossia::make_array("nm")};
};
template<>
class unit_trait<picometer_u>
{
  static constexpr const auto text{ossia::make_array("pm")};
};


template<>
class unit_trait<inch_u>
{
  static constexpr const auto text{ossia::make_array("inch")};
};
template<>
class unit_trait<foot_u>
{
  static constexpr const auto text{ossia::make_array("foot")};
};
template<>
class unit_trait<mile_u>
{
  static constexpr const auto text{ossia::make_array("mile")};
};

using meter = strong_value<meter_u>;
using kilometer = strong_value<kilometer_u>;
using decimeter = strong_value<decimeter_u>;
using centimeter = strong_value<centimeter_u>;
using millimeter = strong_value<millimeter_u>;
using micrometer = strong_value<micrometer_u>;
using nanometer = strong_value<nanometer_u>;
using picometer = strong_value<picometer_u>;
using inch = strong_value<inch_u>;
using foot = strong_value<foot_u>;
using mile = strong_value<mile_u>;

using distance_u =
  eggs::variant<
    meter_u, kilometer_u, decimeter_u, centimeter_u, millimeter_u, micrometer_u, nanometer_u, picometer_u, inch_u, foot_u, mile_u>;
using distance =
  eggs::variant<
    meter, kilometer, decimeter, centimeter, millimeter, micrometer, nanometer, picometer, inch, foot, mile>;
}
