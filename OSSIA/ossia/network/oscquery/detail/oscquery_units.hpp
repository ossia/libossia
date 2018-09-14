#pragma once
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/dataspace/dataspace_variant_visitors.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/oscquery/detail/json_writer_detail.hpp>

#include <fmt/format.h>

namespace std
{
template <>
struct hash<ossia::small_vector<std::string, 4>>
{
  std::size_t operator()(const ossia::small_vector<std::string, 4>& v) const
      noexcept
  {
    std::size_t seed{};
    for (auto& str : v)
      ossia::hash_combine(seed, std::hash<std::string>{}(str));
    return seed;
  }
};
}
namespace ossia::oscquery::detail
{

struct unit_writer
{
  const json_writer_impl& writer;
  void operator()()
  {
  }

  void operator()(const ossia::degree_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::radian_u& u)
  {
    write_unit(u);
  }

  void operator()(const ossia::argb_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.rgb.a");
    writer.writer.String("color.rgb.r");
    writer.writer.String("color.rgb.g");
    writer.writer.String("color.rgb.b");
    writer.writer.EndArray();
  }

  void operator()(const ossia::rgba_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.rgb.r");
    writer.writer.String("color.rgb.g");
    writer.writer.String("color.rgb.b");
    writer.writer.String("color.rgb.a");
    writer.writer.EndArray();
  }

  void operator()(const ossia::rgba8_u&)
  {
    // write nothing since it's already given by the OSC "r" type
  }

  void operator()(const ossia::rgb_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.rgb.r");
    writer.writer.String("color.rgb.g");
    writer.writer.String("color.rgb.b");
    writer.writer.EndArray();
  }

  void operator()(const ossia::bgr_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.rgb.b");
    writer.writer.String("color.rgb.g");
    writer.writer.String("color.rgb.r");
    writer.writer.EndArray();
  }

  void operator()(const ossia::argb8_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.rgb8.a");
    writer.writer.String("color.rgb8.r");
    writer.writer.String("color.rgb8.g");
    writer.writer.String("color.rgb8.b");
    writer.writer.EndArray();
  }

  void operator()(const ossia::hsv_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.hsv.h");
    writer.writer.String("color.hsv.s");
    writer.writer.String("color.hsv.v");
    writer.writer.EndArray();
  }

  void operator()(const ossia::cmy8_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.cmyk8.c");
    writer.writer.String("color.cmyk8.m");
    writer.writer.String("color.cmyk8.y");
    writer.writer.EndArray();
  }

  void operator()(const ossia::xyz_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.ciexyz.x");
    writer.writer.String("color.ciexyz.y");
    writer.writer.String("color.ciexyz.z");
    writer.writer.EndArray();
  }

  void operator()(const ossia::hsl_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.hsl.h");
    writer.writer.String("color.hsl.s");
    writer.writer.String("color.hsl.l");
    writer.writer.EndArray();
  }

  void operator()(const ossia::cmyk8_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.cmyk8.c");
    writer.writer.String("color.cmyk8.m");
    writer.writer.String("color.cmyk8.y");
    writer.writer.String("color.cmyk8.k");
    writer.writer.EndArray();
  }

  void operator()(const ossia::yxy_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.cieYxy.Y");
    writer.writer.String("color.cieYxy.x");
    writer.writer.String("color.cieYxy.y");
    writer.writer.EndArray();
  }

  void operator()(const ossia::hunter_lab_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.hunterLab.l");
    writer.writer.String("color.hunterLab.a");
    writer.writer.String("color.hunterLab.b");
    writer.writer.EndArray();
  }
  void operator()(const ossia::cie_lab_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.cieLab.l");
    writer.writer.String("color.cieLab.a");
    writer.writer.String("color.cieLab.b");
    writer.writer.EndArray();
  }
  void operator()(const ossia::cie_luv_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("color.cieLuv.l");
    writer.writer.String("color.cieLuv.u");
    writer.writer.String("color.cieLuv.v");
    writer.writer.EndArray();
  }

  void operator()(const ossia::meter_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::kilometer_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::decimeter_u& u)
  {
    write_unit(u);
  }
  void operator()(const centimeter_u& u)
  {
    write_unit(u);
  }
  void operator()(const millimeter_u& u)
  {
    write_unit(u);
  }
  void operator()(const micrometer_u& u)
  {
    write_unit(u);
  }
  void operator()(const nanometer_u& u)
  {
    write_unit(u);
  }
  void operator()(const picometer_u& u)
  {
    write_unit(u);
  }
  void operator()(const inch_u& u)
  {
    write_unit(u);
  }
  void operator()(const foot_u& u)
  {
    write_unit(u);
  }
  void operator()(const mile_u& u)
  {
    write_unit(u);
  }

  void operator()(const linear_u& u)
  {
    write_unit(u);
  }
  void operator()(const midigain_u& u)
  {
    write_unit(u);
  }
  void operator()(const decibel_u& u)
  {
    write_unit(u);
  }
  void operator()(const decibel_raw_u& u)
  {
    write_unit(u);
  }

  void operator()(const quaternion_u& u)
  {
    write_extended(u);
  }
  void operator()(const euler_u& u)
  {
    write_extended(u);
  }
  void operator()(const axis_u& u)
  {
    write_extended(u);
  }

  void operator()(const spherical_u& u)
  {
    write_extended(u);
  }
  void operator()(const polar_u& u)
  {
    write_extended(u);
  }
  void operator()(const opengl_u& u)
  {
    write_extended(u);
  }
  void operator()(const cylindrical_u& u)
  {
    write_extended(u);
  }

  void operator()(const cartesian_3d_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("position.cartesian.x");
    writer.writer.String("position.cartesian.y");
    writer.writer.String("position.cartesian.z");
    writer.writer.EndArray();
  }

  void operator()(const cartesian_2d_u&)
  {
    writer.writeKey("EXTENDED_TYPE");
    writer.writer.StartArray();
    writer.writer.String("position.cartesian.x");
    writer.writer.String("position.cartesian.y");
    writer.writer.EndArray();
  }

  void operator()(const ossia::meter_per_second_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::miles_per_hour_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::kilometer_per_hour_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::knot_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::foot_per_hour_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::foot_per_second_u& u)
  {
    write_unit(u);
  }

  void operator()(const ossia::second_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::bark_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::bpm_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::cent_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::frequency_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::mel_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::midi_pitch_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::millisecond_u& u)
  {
    write_unit(u);
  }
  void operator()(const ossia::playback_speed_u& u)
  {
    write_unit(u);
  }

  template <typename T>
  void write_unit(const T&)
  {
    writer.writeKey("UNIT");
    writer.writer.StartArray();
    writer.writer.String(fmt::format(
        "{}.{}", dataspace_traits<typename T::dataspace_type>::text()[0],
        unit_traits<T>::text()[0]));
    writer.writer.EndArray();
  }

  template <typename T>
  void write_extended(const T&)
  {
    writer.writeKey("EXTENDED_TYPE");

    writer.writer.StartArray();
    if constexpr (is_array_unit<T>::value)
    {
      for (char val : T::array_parameters())
      {
        writer.writer.String(fmt::format(
            "{}.{}.{}",
            dataspace_traits<typename T::dataspace_type>::text()[0],
            unit_traits<T>::text()[0], val));
      }
    }
    else
    {
      writer.writer.String(fmt::format(
          "{}.{}", dataspace_traits<typename T::dataspace_type>::text()[0],
          unit_traits<T>::text()[0]));
    }
    writer.writer.EndArray();
  }
};

struct unit_parser
{
  ossia::fast_hash_map<ossia::small_vector<std::string, 4>, ossia::unit_t> map{
      unit_matcher(degree_u{}),
      unit_matcher(radian_u{}),

      {{"color.rgb.a", "color.rgb.r", "color.rgb.g", "color.rgb.b"},
       ossia::argb_u{}},
      {{"color.rgb.r", "color.rgb.g", "color.rgb.b", "color.rgb.a"},
       ossia::rgba_u{}},
      // { { "color.rgb.r", "color.rgb.g", "color.rgb.b" }, ossia::rgba8_u{} },
      // // not needed
      {{"color.rgb.r", "color.rgb.g", "color.rgb.b"}, ossia::rgb_u{}},
      {{"color.rgb.b", "color.rgb.g", "color.rgb.r"}, ossia::bgr_u{}},
      {{"color.rgb8.a", "color.rgb8.r", "color.rgb8.g", "color.rgb8.b"},
       ossia::argb8_u{}},
      {{"color.hsv.h", "color.hsv.s", "color.hsv.v"}, ossia::hsv_u{}},
      {{"color.cmyk8.c", "color.cmyk8.m", "color.cmyk8.y"}, ossia::cmy8_u{}},
      {{"color.ciexyz.x", "color.ciexyz.y", "color.ciexyz.z"},
       ossia::xyz_u{}}, /*
{ { "color.hsl.h", "color.hsl.s", "color.hsl.l" }, ossia::hsl_u{} },
{ { "color.cmyk8.c", "color.cmyk8.m", "color.cmyk8.y", "color.cmyk8.k" },
ossia::cmyk8_u{} }, { { "color.cieYxy.y", "color.cieYxy.x", "color.cieYxy.y" },
ossia::yxy_u{} }, { { "color.hunterLab.l", "color.hunterLab.a",
"color.hunterLab.b" }, ossia::hunter_lab_u{} }, { { "color.cieLab.l",
"color.cieLab.a", "color.cieLab.b" }, ossia::cie_lab_u{} }, { {
"color.cieLuv.l", "color.cieLuv.a", "color.cieLuv.b" }, ossia::cie_luv_u{} },*/

      unit_matcher(meter_u{}),
      unit_matcher(kilometer_u{}),
      unit_matcher(decimeter_u{}),
      unit_matcher(centimeter_u{}),
      unit_matcher(millimeter_u{}),
      unit_matcher(micrometer_u{}),
      unit_matcher(nanometer_u{}),
      unit_matcher(picometer_u{}),
      unit_matcher(inch_u{}),
      unit_matcher(foot_u{}),
      unit_matcher(mile_u{}),

      unit_matcher(linear_u{}),
      unit_matcher(midigain_u{}),
      unit_matcher(decibel_u{}),
      unit_matcher(decibel_raw_u{}),

      unit_matcher(quaternion_u{}),
      unit_matcher(euler_u{}),
      unit_matcher(axis_u{}),

      unit_matcher(spherical_u{}),
      unit_matcher(polar_u{}),
      unit_matcher(opengl_u{}),
      unit_matcher(cylindrical_u{}),

      {{"position.cartesian.x", "position.cartesian.y",
        "position.cartesian.z"},
       ossia::cartesian_3d_u{}},
      {{"position.cartesian.x", "position.cartesian.y"},
       ossia::cartesian_2d_u{}},

      unit_matcher(meter_per_second_u{}),
      unit_matcher(miles_per_hour_u{}),
      unit_matcher(kilometer_per_hour_u{}),
      unit_matcher(knot_u{}),
      unit_matcher(foot_per_hour_u{}),
      unit_matcher(foot_per_second_u{}),

      unit_matcher(second_u{}),
      unit_matcher(bark_u{}),
      unit_matcher(bpm_u{}),
      unit_matcher(cent_u{}),
      unit_matcher(frequency_u{}),
      unit_matcher(mel_u{}),
      unit_matcher(midi_pitch_u{}),
      unit_matcher(millisecond_u{}),
      unit_matcher(playback_speed_u{})};

  template <typename T>
  static std::pair<ossia::small_vector<std::string, 4>, ossia::unit_t>
  unit_matcher(const T&)
  {
    ossia::small_vector<std::string, 4> vec;
    if constexpr (is_array_unit<T>::value)
    {
      for (char val : T::array_parameters())
      {
        vec.push_back(fmt::format(
            "{}.{}.{}",
            dataspace_traits<typename T::dataspace_type>::text()[0],
            unit_traits<T>::text()[0], val));
      }
    }
    else
    {
      vec.push_back(fmt::format(
          "{}.{}", dataspace_traits<typename T::dataspace_type>::text()[0],
          unit_traits<T>::text()[0]));
    }

    return {std::move(vec), T{}};
  }
};
}
