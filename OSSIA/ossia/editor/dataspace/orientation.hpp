#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>
#include <eggs/variant.hpp>

namespace ossia
{

struct quaternion_u;
struct orientation_dataspace;
template<typename Impl>
struct orientation_unit
{
  using is_unit = std::true_type;
  using is_multidimensional = std::true_type;
  using dataspace_type = orientation_dataspace;
  using neutral_unit = quaternion_u;
  using concrete_type = Impl;
};


struct quaternion_u :
    public orientation_unit<quaternion_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("quaternion", "quat"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("1ijk"); } // TODO find something better than 1 ?

  using value_type = Vec4f;
  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }
};

struct euler_u :
    public orientation_unit<euler_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("euler", "ypr"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("ypr"); }

  using value_type = Vec4f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);
};


struct axis_u :
    public orientation_unit<axis_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("axis", "xyza"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("xyza"); }
  using value_type = Vec4f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);
};



using quaternion = strong_value<quaternion_u>;
using euler = strong_value<euler_u>;
using axis = strong_value<axis_u>;

using orientation_u = eggs::variant<quaternion_u, euler_u, axis_u>;

template<>
struct dataspace_traits<orientation_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("orientation"); }
};


}
