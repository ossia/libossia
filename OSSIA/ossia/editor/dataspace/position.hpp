#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>

namespace ossia
{
struct cartesian_3d_u;
template<typename Impl>
struct position_unit
{
  using dataspace_type = struct position_dataspace;
  using neutral_unit = cartesian_3d_u;
  using concrete_type = Impl;
};

struct cartesian_3d_u :
    public position_unit<cartesian_3d_u>
{
  using value_type = Vec3f;
  static constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.val.value;
  }
};

struct cartesian_2d_u :
    public position_unit<cartesian_2d_u>
{
  using value_type = Vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{self.val.value[0], self.val.value[1], 0.f}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{self.val.value[0], self.val.value[1]}};
  }
};

// aed
struct spherical_u :
    public position_unit<spherical_u>
{
  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto a = self.val.value[0] * deg_to_rad;
    const auto e = self.val.value[1] * deg_to_rad;
    const auto d = self.val.value[2];

    const auto temp = std::cos(e) * d;

    return {{{
      float(std::sin(a) * temp),
      float(std::cos(a) * temp),
      float(std::sin(e) * d)
    }}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.val.value[0];
    const auto y = self.val.value[1];
    const auto z = self.val.value[2];

    const auto temp = x * x + y * y;

    return {{{
          float(std::atan2(x, y) * rad_to_deg),
          float(std::atan2(z, std::pow(temp, 0.5)) * rad_to_deg),
          float(std::pow(temp + (z * z), 0.5))
        }}};
  }
};

// ad
struct polar_u :
    public position_unit<polar_u>
{
  using value_type = Vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto a = self.val.value[0] * deg_to_rad;
    const auto d = self.val.value[2];

    return {{{
          float(std::sin(a) * d),
          float(std::cos(a) * d),
          0.
        }}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.val.value[0];
    const auto y = self.val.value[1];

    return {{{
          float(std::atan2(x, y) * rad_to_deg),
          float(std::pow(x * x + y * y, 0.5))
        }}};
  }
};

struct opengl_u :
    public position_unit<opengl_u>
{
  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{self.val.value[0], -self.val.value[2], self.val.value[1]}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{{self.val.value[0], self.val.value[2], -self.val.value[1]}}};
  }
};

struct cylindrical_u :
    public position_unit<cylindrical_u>
{
  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto d = self.val.value[0];
    const auto a = self.val.value[1] * deg_to_rad;
    const auto z = self.val.value[2];

    return {{{
      float(std::sin(a) * d),
      float(std::cos(a) * d),
      z
    }}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.val.value[0];
    const auto y = self.val.value[1];
    const auto z = self.val.value[2];

    return {{{
          float(std::pow(x * x + y * y, 0.5)),
          float(std::atan2(x, y) * rad_to_deg),
          z
        }}};
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
using position =
  eggs::variant<cartesian_3d, cartesian_2d, spherical, polar, opengl, cylindrical>;

}
