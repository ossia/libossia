#pragma once
#include <ossia/network/dataspace/dataspace_base.hpp>
#include <ossia/network/domain/domain_base_impl.hpp>

namespace ossia
{

struct quaternion_u;
struct orientation_u;
template <typename Impl>
struct orientation_unit
{
  using is_unit = std::true_type;
  using is_multidimensional = std::true_type;
  using dataspace_type = orientation_u;
  using neutral_unit = quaternion_u;
  using concrete_type = Impl;
};

struct OSSIA_EXPORT quaternion_u : public orientation_unit<quaternion_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("quaternion", "quat"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("abcd"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("distance.m", "distance.m", "distance.m", "distance.m"));
  }
  using value_type = vec4f;
  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type
  from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }

  static ossia::vecf_domain<4> domain() {
    return vecf_domain<4>{
      ossia::make_vec(0.f,0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f,1.f)
    };
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT euler_u : public orientation_unit<euler_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("euler", "ypr"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("ypr"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("angle.degree", "angle.degree", "angle.degree"));
  }
  using value_type = vec4f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(360.f,360.f,360.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::WRAP; }
};

struct OSSIA_EXPORT axis_u : public orientation_unit<axis_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("axis", "xyzw"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("xyzw"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("distance.m", "distance.m", "distance.m", "angle.degree"));
  }
  using value_type = vec4f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<4> domain() {
    return vecf_domain<4>{
      ossia::make_vec(0.f,0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f,360.f)
    };
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};
}
