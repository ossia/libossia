#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>

namespace ossia
{

struct quaternion_u;
template<typename Impl>
struct orientation_unit
{
  using is_unit = std::true_type;
  using is_multidimensional = std::true_type;
  using dataspace_type = struct orientation_dataspace;
  using neutral_unit = quaternion_u;
  using concrete_type = Impl;
};


struct quaternion_u :
    public orientation_unit<quaternion_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("quaternion", "quat"); }
  static constexpr auto array_parameters()
  { return "1ijk"; } // TODO find something better than 1 ?

  using value_type = Vec4f;
  static constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.value.value;
  }
};

struct euler_u :
    public orientation_unit<euler_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("euler", "ypr"); }
  static constexpr auto array_parameters()
  { return "ypr"; }

  using value_type = Vec4f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto yaw = self.value.value[0] * deg_to_rad * -0.5;
    const auto pitch = self.value.value[1] * deg_to_rad * 0.5;
    const auto roll = self.value.value[2] * deg_to_rad * 0.5;

    const auto sinYaw = std::sin(yaw);
    const auto cosYaw = std::cos(yaw);
    const auto sinPitch = std::sin(pitch);
    const auto cosPitch = std::cos(pitch);
    const auto sinRoll = std::sin(roll);
    const auto cosRoll = std::cos(roll);
    const auto cosPitchCosRoll = cosPitch * cosRoll;
    const auto sinPitchSinRoll = sinPitch * sinRoll;

    return std::array<double, 4>{
     cosYaw * sinPitch * cosRoll  - sinYaw * cosPitch * sinRoll, //X
     cosYaw * cosPitch * sinRoll  + sinYaw * sinPitch * cosRoll, //Y
     sinYaw * cosPitchCosRoll     + cosYaw * sinPitchSinRoll, //Z
     cosYaw * cosPitchCosRoll     - sinYaw * sinPitchSinRoll //W
     };
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.value.value[0];
    const auto y = self.value.value[1];
    const auto z = self.value.value[2];
    const auto w = self.value.value[3];

    return std::array<double, 4>{
          rad_to_deg * std::atan2(-2. * (z*w - x*y), w*w - x*x + y*y - z*z),
          rad_to_deg * std::asin(2. * (w*x + y*z)),
          rad_to_deg * std::atan2(2. * (w*y + x*z), w*w - x*x - y*y + z*z)
        };
  }
};


struct axis_u :
    public orientation_unit<axis_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("axis", "xyza"); }
  static constexpr auto array_parameters()
  { return "xyza"; }
  using value_type = Vec4f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto x = self.value.value[0];
    const auto y = self.value.value[1];
    const auto z = self.value.value[2];
    const auto angle = self.value.value[3] * deg_to_rad * 0.5;

    const auto sinAngle = std::sin(angle);

    //vector normalization:
    const auto norm = std::sqrt(x*x + y*y + z*z);
    const auto n = norm > 0.0 ? 1.0 / norm : norm;

    /* x = x * n;
    y = y * n;
    z = z * n; */

    return std::array<double, 4>{
     x * n * sinAngle, //X
     y * n * sinAngle, //Y
     z * n * sinAngle, //Z
     std::cos(angle) //W
    };
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto x = self.value.value[0];
    const auto y = self.value.value[1];
    const auto z = self.value.value[2];
    const auto w = self.value.value[3];

    const auto sin_a = std::sqrt( 1.0 - w * w );

    const auto sin_a2 =
        std::fabs(sin_a) < 0.0005
            ? 1.0
            : 1.0 / sin_a;

    return std::array<double, 4>{
     x * sin_a2,
     y * sin_a2,
     z * sin_a2,
     rad_to_deg * 2.0 * std::atan2(sin_a, w)
    };
  }
};



using quaternion = strong_value<quaternion_u>;
using euler = strong_value<euler_u>;
using axis = strong_value<axis_u>;

using orientation_u = eggs::variant<quaternion_u, euler_u, axis_u>;

template<>
struct dataspace_traits<orientation_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("orientation"); }
};
}
