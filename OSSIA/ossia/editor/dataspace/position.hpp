#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>
#include <eggs/variant.hpp>

namespace ossia
{
struct cartesian_3d_u;
struct position_dataspace;
template<typename Impl>
struct position_unit
{
  using is_unit = std::true_type;
  using is_multidimensional = std::true_type;
  using dataspace_type = position_dataspace;
  using neutral_unit = cartesian_3d_u;
  using concrete_type = Impl;
};

struct cartesian_3d_u :
    public position_unit<cartesian_3d_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cart3D", "xyz"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("xyz"); }

  using value_type = Vec3f;
  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.value.value;
  }
};

struct cartesian_2d_u :
    public position_unit<cartesian_2d_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cart2D", "xy"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("xy"); }
  using value_type = Vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return std::array<double, 3>{self.value.value[0], self.value.value[1], 0.f};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return std::array<double, 2>{self.value.value[0], self.value.value[1]};
  }
};

// aed
struct spherical_u :
    public position_unit<spherical_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("spherical", "aed"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("aed"); }
  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto a = self.value.value[0] * deg_to_rad;
    const auto e = self.value.value[1] * deg_to_rad;
    const auto d = self.value.value[2];

    const auto temp = std::cos(e) * d;

    return std::array<double, 3>{
      std::sin(a) * temp,
      std::cos(a) * temp,
      std::sin(e) * d
    };
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.value.value[0];
    const auto y = self.value.value[1];
    const auto z = self.value.value[2];

    const auto temp = x * x + y * y;

    return std::array<double, 3>{
          std::atan2(x, y) * rad_to_deg,
          std::atan2(z, std::pow(temp, 0.5)) * rad_to_deg,
          std::pow(temp + (z * z), 0.5)
        };
  }
};

// ad
struct polar_u :
    public position_unit<polar_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("polar", "ad"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("ad"); }
  using value_type = Vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto a = self.value.value[0] * deg_to_rad;
    const auto d = self.value.value[2];

    return std::array<double, 3>{
          std::sin(a) * d,
          std::cos(a) * d,
          0.
        };
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.value.value[0];
    const auto y = self.value.value[1];

    return std::array<double, 2>{
          std::atan2(x, y) * rad_to_deg,
          std::sqrt(x * x + y * y)
        };
  }
};

struct opengl_u :
    public position_unit<opengl_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("openGL"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("xyz"); }
  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return std::array<double, 3>{self.value.value[0], -self.value.value[2], self.value.value[1]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return std::array<double, 3>{self.value.value[0], self.value.value[2], -self.value.value[1]};
  }
};

struct cylindrical_u :
    public position_unit<cylindrical_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cylindrical", "daz"); }
  static OSSIA_DECL_RELAXED_CONSTEXPR auto array_parameters()
  { return ossia::make_string_view("daz"); }
  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto d = self.value.value[0];
    const auto a = self.value.value[1] * deg_to_rad;
    const auto z = self.value.value[2];

    return std::array<double, 3>{
      std::sin(a) * d,
      std::cos(a) * d,
      z
    };
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.value.value[0];
    const auto y = self.value.value[1];
    const auto z = self.value.value[2];

    return std::array<double, 3>{
          std::pow(x * x + y * y, 0.5),
          std::atan2(x, y) * rad_to_deg,
          z
        };
  }
};



using cartesian_3d = strong_value<cartesian_3d_u>;
using cartesian_2d = strong_value<cartesian_2d_u>;
using spherical = strong_value<spherical_u>;
using polar = strong_value<polar_u>;
using opengl = strong_value<opengl_u>;
using cylindrical = strong_value<cylindrical_u>;

using position_u =
  eggs::variant<cartesian_3d_u, cartesian_2d_u, spherical_u, polar_u, opengl_u, cylindrical_u>;

template<>
struct dataspace_traits<position_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("position"); }
};
}
