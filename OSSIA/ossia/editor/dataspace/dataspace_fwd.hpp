#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>
#include <eggs/variant.hpp>
#include <brigand/algorithms/transform.hpp>
#include <brigand/algorithms/fold.hpp>
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
class value;
struct degree_u;
struct radian_u;
using angle_u =
  eggs::variant<
    degree_u, radian_u>;

struct argb_u;
struct rgba_u;
struct rgb_u;
struct bgr_u;
struct argb8_u;
struct hsv_u;
struct cmy8_u;
struct xyz_u;
using color_u =
  eggs::variant<
    argb_u, rgba_u, rgb_u, bgr_u, argb8_u, hsv_u, cmy8_u, xyz_u
/*, hsl_u, cmyk8_u, yxy_u, hunter_lab_u, cie_lab_u, cie_luv_u*/>;

template<typename T>
struct distance_ratio;

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

using distance_u =
  eggs::variant<
    meter_u, kilometer_u, decimeter_u, centimeter_u, millimeter_u, micrometer_u, nanometer_u, picometer_u, inch_u, foot_u, mile_u>;

struct linear_u;
struct midigain_u;
struct decibel_u;
struct decibel_raw_u;
using gain_u =
  eggs::variant<
    linear_u, midigain_u, decibel_u, decibel_raw_u>;

struct quaternion_u;
struct euler_u;
struct axis_u;
using orientation_u = eggs::variant<quaternion_u, euler_u, axis_u>;

struct cartesian_3d_u;
struct cartesian_2d_u;
struct spherical_u;
struct polar_u;
struct opengl_u;
struct cylindrical_u;
using position_u =
  eggs::variant<cartesian_3d_u, cartesian_2d_u, spherical_u, polar_u, opengl_u, cylindrical_u>;

template<typename T>
struct speed_ratio;

using meter_per_second_u = speed_ratio<std::ratio<1>>;
using miles_per_hour_u = speed_ratio<std::ratio<254 * 12 * 5280, 3600 * 10000>>;
using kilometer_per_hour_u = speed_ratio<std::ratio<1000, 3600>>;
using knot_u = speed_ratio<std::ratio<1852, 3600>>;
using foot_per_hour_u = speed_ratio<std::ratio<254 * 12, 3600 * 10000>>;
using foot_per_second_u = speed_ratio<std::ratio<254 * 12, 10000>>;
using speed_u =
  eggs::variant<
    meter_per_second_u, miles_per_hour_u, kilometer_per_hour_u, knot_u, foot_per_second_u, foot_per_hour_u>;

struct second_u;
struct bark_u;
struct bpm_u;
struct cent_u;
struct frequency_u;
struct mel_u;
struct midi_pitch_u;
struct millisecond_u;
struct playback_speed_u;
using time_u =
  eggs::variant<
    second_u, bark_u, bpm_u, cent_u, frequency_u, mel_u, midi_pitch_u, millisecond_u, playback_speed_u>;

using unit_t = eggs::variant<distance_u, position_u, speed_u, orientation_u, angle_u, color_u, gain_u, time_u>;


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

// Basically eggs::variant<ossia::value, ossia::distance, ossia::position, ossia::speed...>
using value_with_unit =
brigand::as_eggs_variant<
  brigand::append<
    brigand::list<ossia::value>,
    brigand::as_list<
      brigand::transform<
        unit_t,
        brigand::bind<ossia::add_value, brigand::_1>
      >
    >
  >
>;

}
