#pragma once
#include <ossia/network/dataspace/dataspace_base.hpp>
#include <ossia/network/domain/domain.hpp>
namespace ossia
{
struct cartesian_3d_u;
struct position_u;
template <typename Impl>
struct position_unit
{
  using is_unit = std::true_type;
  using is_multidimensional = std::true_type;
  using dataspace_type = position_u;
  using neutral_unit = cartesian_3d_u;
  using concrete_type = Impl;
};

struct OSSIA_EXPORT cartesian_3d_u : public position_unit<cartesian_3d_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cart3D", "xyz"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("xyz"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("distance.m", "distance.m", "distance.m"));
  }
  using value_type = vec3f;
  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }

  static ossia::vecf_domain<3> domain()
  {
    return vecf_domain<3>{
        ossia::make_vec(-1.f, -1.f, -1.f), ossia::make_vec(1.f, 1.f, 1.f)};
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT cartesian_2d_u : public position_unit<cartesian_2d_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cart2D", "xy"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("xy"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("distance.m", "distance.m"));
  }
  using value_type = vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {self.dataspace_value[0], self.dataspace_value[1], 0.f};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[0], self.dataspace_value[1]};
  }

  static ossia::vecf_domain<2> domain()
  {
    return vecf_domain{ossia::make_vec(-1.f, -1.f), ossia::make_vec(1.f, 1.f)};
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT spherical_u : public position_unit<spherical_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("spherical", "rtp"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("rtp"));
  }
  static constexpr auto units()
  {
    constexpr_return(
        ossia::make_string_array("distance.m", "angle.radiant", "angle.radiant"));
  }
  using value_type = vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain()
  {
    return vecf_domain<3>{
        ossia::make_vec(0.f, -pi, -half_pi), ossia::make_vec(1.f, pi, half_pi)};
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT polar_u : public position_unit<polar_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("polar", "rp"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("rp"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("distance.m", "angle.radiant"));
  }
  using value_type = vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto r = self.dataspace_value[0];
    const auto t = self.dataspace_value[1];

    return {(float)(std::cos(t) * r), (float)(std::sin(t) * r)};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.dataspace_value[0];
    const auto y = self.dataspace_value[1];

    return {(float)(ossia::norm(x, y)), (float)(std::atan2(y, x))};
  }

  static ossia::vecf_domain<2> domain()
  {
    return vecf_domain<2>{ossia::make_vec(0.f, -pi), ossia::make_vec(1.f, pi)};
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT aed_u : public position_unit<aed_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("AED", "aed"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("aed"));
  }
  static constexpr auto units()
  {
    constexpr_return(
        ossia::make_string_array("angle.degree", "angle.degree", "distance.m"));
  }
  using value_type = vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain()
  {
    return vecf_domain<3>{
        ossia::make_vec(-180.f, -180.f, 0.f), ossia::make_vec(180.f, 180.f, 1.f)};
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT ad_u : public position_unit<ad_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("AD", "ad"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("ad"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("angle.degree", "distance.m"));
  }
  using value_type = vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto a = self.dataspace_value[0] * deg_to_rad - half_pi;
    const auto d = self.dataspace_value[1];

    return {(float)(std::sin(-a) * d), (float)(std::cos(-a) * d), 0.f};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.dataspace_value[0];
    const auto y = self.dataspace_value[1];

    return {
        -(float)(std::atan2(y, x) + half_pi * rad_to_deg), (float)(ossia::norm(x, y))};
  }

  static ossia::vecf_domain<2> domain()
  {
    return vecf_domain<2>{ossia::make_vec(-180.f, 0.f), ossia::make_vec(180.f, 0.f)};
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT opengl_u : public position_unit<opengl_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("openGL")); }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("xyz"));
  }
  static constexpr auto units()
  {
    constexpr_return(ossia::make_string_array("distance.m", "distance.m", "distance.m"));
  }
  using value_type = vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {self.dataspace_value[0], -self.dataspace_value[2], self.dataspace_value[1]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[0], self.dataspace_value[2], -self.dataspace_value[1]};
  }

  static ossia::vecf_domain<3> domain()
  {
    return vecf_domain{
        ossia::make_vec(-1.f, -1.f, -1.f), ossia::make_vec(1.f, 1.f, 1.f)};
  }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT cylindrical_u : public position_unit<cylindrical_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cylindrical", "rtz"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("rtz"));
  }
  static constexpr auto units()
  {
    constexpr_return(
        ossia::make_string_array("distance.m", "angle.degree", "distance.m"));
  }
  using value_type = vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain()
  {
    return vecf_domain<3>{
        ossia::make_vec(0.f, -180.f, 0.f), ossia::make_vec(1.f, 180.f, 1.f)};
  }
  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct OSSIA_EXPORT azd_u : public position_unit<azd_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("AZD", "azd"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("azd"));
  }
  static constexpr auto units()
  {
    constexpr_return(
        ossia::make_string_array("angle.degree", "distance.m", "distance.m"));
  }
  using value_type = vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain()
  {
    return vecf_domain<3>{
        ossia::make_vec(-180.f, -1.f, 0.f), ossia::make_vec(180.f, 1.f, 1.f)};
  }
  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};
}
