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
  using is_unit = std::true_type;
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
struct unit_traits<meter_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("m", "meters"); }
};

template<>
struct unit_traits<kilometer_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("km", "kilometers"); }
};
template<>
struct unit_traits<decimeter_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("dm", "decimeters"); }
};
template<>
struct unit_traits<centimeter_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("cm", "centimeters"); }
};
template<>
struct unit_traits<millimeter_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("mm", "millimeters"); }
};
template<>
struct unit_traits<micrometer_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("um", "micrometers"); }
};
template<>
struct unit_traits<nanometer_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("nm", "nanometers"); }
};
template<>
struct unit_traits<picometer_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("pm", "picometers"); }
};


template<>
struct unit_traits<inch_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("inches", "\""); }
};
template<>
struct unit_traits<foot_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("feet", "'"); }
};
template<>
struct unit_traits<mile_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("miles"); }
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
}
