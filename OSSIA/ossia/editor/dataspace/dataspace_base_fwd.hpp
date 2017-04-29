#pragma once
#include <ossia/editor/dataspace/dataspace_base_defs_fwd.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
template<>
struct dataspace_traits<angle_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("angle"); }
};

template<>
struct dataspace_traits<color_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("color"); }
};

template<>
struct dataspace_traits<distance_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("distance"); }
};

template<>
struct dataspace_traits<gain_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("gain"); }
};

template<>
struct dataspace_traits<orientation_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("orientation"); }
};
template<>
struct dataspace_traits<position_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("position"); }
};

template<>
struct dataspace_traits<speed_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("speed"); }
};

template<>
struct dataspace_traits<timing_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("time"); }
};


}
