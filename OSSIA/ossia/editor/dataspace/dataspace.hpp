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
#include <brigand/algorithms/fold.hpp>
#include <functional>
namespace ossia
{
#include <ossia/editor/dataspace/dataspace_base_variants.hpp>
struct OSSIA_EXPORT unit_t final
{
  unit_variant v;

  unit_t() = default;
  unit_t(const unit_t&) = default;
  unit_t(unit_t&&) = default;
  unit_t& operator=(const unit_t&) = default;
  unit_t& operator=(unit_t&&) = default;

  unit_t(angle_u t): v{t} { }
  unit_t(degree_u): v{angle_u{degree_u{}}} { }
  unit_t(radian_u): v{angle_u{radian_u{}}} { }

  unit_t(color_u t): v{t} { }
  unit_t(argb_u): v{color_u{argb_u{}}} { }
  unit_t(rgba_u): v{color_u{rgba_u{}}} { }
  unit_t(rgb_u): v{color_u{rgb_u{}}} { }
  unit_t(bgr_u): v{color_u{bgr_u{}}} { }
  unit_t(argb8_u): v{color_u{argb8_u{}}} { }
  unit_t(hsv_u): v{color_u{hsv_u{}}} { }
  unit_t(cmy8_u): v{color_u{cmy8_u{}}} { }
  unit_t(xyz_u): v{color_u{xyz_u{}}} { }

  unit_t(distance_u t): v{t} { }
  unit_t(meter_u): v{distance_u{meter_u{}}} { }
  unit_t(kilometer_u): v{distance_u{kilometer_u{}}} { }
  unit_t(decimeter_u): v{distance_u{decimeter_u{}}} { }
  unit_t(centimeter_u): v{distance_u{centimeter_u{}}} { }
  unit_t(millimeter_u): v{distance_u{millimeter_u{}}} { }
  unit_t(micrometer_u): v{distance_u{micrometer_u{}}} { }
  unit_t(nanometer_u): v{distance_u{nanometer_u{}}} { }
  unit_t(picometer_u): v{distance_u{picometer_u{}}} { }
  unit_t(inch_u): v{distance_u{inch_u{}}} { }
  unit_t(foot_u): v{distance_u{foot_u{}}} { }
  unit_t(mile_u): v{distance_u{mile_u{}}} { }

  unit_t(gain_u t): v{t} { }
  unit_t(linear_u): v{gain_u{linear_u{}}} { }
  unit_t(midigain_u): v{gain_u{midigain_u{}}} { }
  unit_t(decibel_u): v{gain_u{decibel_u{}}} { }
  unit_t(decibel_raw_u): v{gain_u{decibel_raw_u{}}} { }

  unit_t(orientation_u t): v{t} { }
  unit_t(quaternion_u): v{orientation_u{quaternion_u{}}} { }
  unit_t(euler_u): v{orientation_u{euler_u{}}} { }
  unit_t(axis_u): v{orientation_u{axis_u{}}} { }

  unit_t(position_u t): v{t} { }
  unit_t(cartesian_3d_u): v{position_u{cartesian_3d_u{}}} { }
  unit_t(cartesian_2d_u): v{position_u{cartesian_2d_u{}}} { }
  unit_t(spherical_u): v{position_u{spherical_u{}}} { }
  unit_t(polar_u): v{position_u{polar_u{}}} { }
  unit_t(opengl_u): v{position_u{opengl_u{}}} { }
  unit_t(cylindrical_u): v{position_u{cylindrical_u{}}} { }

  unit_t(speed_u t): v{t} { }
  unit_t(meter_per_second_u): v{speed_u{meter_per_second_u{}}} { }
  unit_t(miles_per_hour_u): v{speed_u{miles_per_hour_u{}}} { }
  unit_t(kilometer_per_hour_u): v{speed_u{kilometer_per_hour_u{}}} { }
  unit_t(knot_u): v{speed_u{knot_u{}}} { }
  unit_t(foot_per_hour_u): v{speed_u{foot_per_hour_u{}}} { }
  unit_t(foot_per_second_u): v{speed_u{foot_per_second_u{}}} { }

  unit_t(time_u t): v{t} { }
  unit_t(second_u): v{time_u{second_u{}}} { }
  unit_t(bark_u): v{time_u{bark_u{}}} { }
  unit_t(bpm_u): v{time_u{bpm_u{}}} { }
  unit_t(cent_u): v{time_u{cent_u{}}} { }
  unit_t(frequency_u): v{time_u{frequency_u{}}} { }
  unit_t(mel_u): v{time_u{mel_u{}}} { }
  unit_t(midi_pitch_u): v{time_u{midi_pitch_u{}}} { }
  unit_t(millisecond_u): v{time_u{millisecond_u{}}} { }
  unit_t(playback_speed_u): v{time_u{playback_speed_u{}}} { }


  template<typename T>
  unit_t(T unit, typename T::is_unit* = nullptr): v{unit} { }

  auto which() const { return v.which(); }
  operator bool() const { return bool(v); }

  friend bool operator==(const ossia::unit_t& unit, const ossia::unit_t& other)
  {
    return unit.v == other.v;
  }

  friend bool operator!=(const ossia::unit_t& unit, const ossia::unit_t& other)
  {
    return unit.v != other.v;
  }

};

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

static const constexpr int dataspace_count = 8;
static const constexpr int unit_count = 2 + 8 + 11 + 4 + 3 + 6 + 6 + 9;
}
namespace std
{
template<> struct hash<ossia::distance_u>
{ std::size_t operator()(const ossia::distance_u& k) const { return k.which(); } };
template<> struct hash<ossia::position_u>
{ std::size_t operator()(const ossia::position_u& k) const { return k.which(); } };
template<> struct hash<ossia::speed_u>
{ std::size_t operator()(const ossia::speed_u& k) const { return k.which(); } };
template<> struct hash<ossia::orientation_u>
{ std::size_t operator()(const ossia::orientation_u& k) const { return k.which(); } };
template<> struct hash<ossia::angle_u>
{ std::size_t operator()(const ossia::angle_u& k) const { return k.which(); } };
template<> struct hash<ossia::color_u>
{ std::size_t operator()(const ossia::color_u& k) const { return k.which(); } };
template<> struct hash<ossia::gain_u>
{ std::size_t operator()(const ossia::gain_u& k) const { return k.which(); } };
template<> struct hash<ossia::time_u>
{ std::size_t operator()(const ossia::time_u& k) const { return k.which(); } };

}
