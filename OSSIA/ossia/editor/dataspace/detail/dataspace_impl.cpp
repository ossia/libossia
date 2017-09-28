// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/dataspace/dataspace.hpp>

namespace ossia
{
// COLOR //
strong_value<hsv_u::neutral_unit>
hsv_u::to_neutral(strong_value<hsv_u::concrete_type> self)
{
  const auto H = self.dataspace_value[0];
  const auto S = self.dataspace_value[1];
  const auto V = self.dataspace_value[2];
  if (S == 0.)
  {
    return {1., V, V, V};
  }
  else
  {
    auto var_h = H * 6.;
    if (var_h == 6.)
      var_h = 0.;      // H must be < 1
    int var_i = var_h; // Or ... var_i = floor( var_h )
    auto var_1 = V * (1. - S);
    auto var_2 = V * (1. - S * (var_h - var_i));
    auto var_3 = V * (1. - S * (1. - (var_h - var_i)));

    switch (var_i)
    {
      case 0:
        return make_vec(1., V, var_3, var_1);
      case 1:
        return make_vec(1., var_2, V, var_1);
      case 2:
        return make_vec(1., var_1, V, var_3);
      case 3:
        return make_vec(1., var_1, var_2, V);
      case 4:
        return make_vec(1., var_3, var_1, V);
      default:
        return make_vec(1., V, var_1, var_2);
    }
  }
}

hsv_u::value_type hsv_u::from_neutral(strong_value<hsv_u::neutral_unit> self)
{
  const auto var_R = self.dataspace_value[1];
  const auto var_G = self.dataspace_value[2];
  const auto var_B = self.dataspace_value[3];

  const auto var_Min
      = std::min(std::min(var_R, var_G), var_B); // Min. value of RGB
  const auto var_Max
      = std::max(std::max(var_R, var_G), var_B); // Max. value of RGB
  const auto del_Max = var_Max - var_Min;        // Delta RGB value

  if (del_Max == 0.) // This is a gray, no chroma...
  {
    return {0., 0., var_Max};
  }
  else // Chromatic data...
  {
    double H{};
    auto S = del_Max / var_Max;
    auto V = var_Max;

    auto del_R = (((var_Max - var_R) / 6.) + (del_Max / 2.)) / del_Max;
    auto del_G = (((var_Max - var_G) / 6.) + (del_Max / 2.)) / del_Max;
    auto del_B = (((var_Max - var_B) / 6.) + (del_Max / 2.)) / del_Max;

    if (var_R == var_Max)
      H = del_B - del_G;
    else if (var_G == var_Max)
      H = (1. / 3.) + del_R - del_B;
    else if (var_B == var_Max)
      H = (2. / 3.) + del_G - del_R;

    if (H < 0.)
      H += 1.;
    if (H > 1.)
      H -= 1.;
    return make_vec(H, S, V);
  }
}

strong_value<xyz_u::neutral_unit>
xyz_u::to_neutral(strong_value<xyz_u::concrete_type> self)
{
  auto var_X
      = self.dataspace_value[0]
        / 100.; // X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
  auto var_Y = self.dataspace_value[1] / 100.; // Y from 0 to 100.000
  auto var_Z = self.dataspace_value[2] / 100.; // Z from 0 to 108.883

  auto var_R = var_X * 3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
  auto var_G = var_X * -0.9689 + var_Y * 1.8758 + var_Z * 0.0415;
  auto var_B = var_X * 0.0557 + var_Y * -0.2040 + var_Z * 1.0570;

  auto translate = [](auto var) {
    return var > 0.0031308 ? 1.055 * (std::pow(var, (1 / 2.4))) - 0.055
                           : var * 12.92;
  };

  return make_vec(1., translate(var_R), translate(var_G), translate(var_B));
}

xyz_u::value_type xyz_u::from_neutral(strong_value<xyz_u::neutral_unit> self)
{
  auto translate = [](auto var) {
    return 100. * ((var > 0.04045) ? std::pow((var + 0.055) / 1.055, 2.4)
                                   : var / 12.92);
  };
  auto var_R = translate(self.dataspace_value[1]);
  auto var_G = translate(self.dataspace_value[2]);
  auto var_B = translate(self.dataspace_value[3]);

  // Observer. = 2°, Illuminant = D65

  return make_vec(
      var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805,
      var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722,
      var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505);
}

strong_value<hunter_lab_u::neutral_unit>
hunter_lab_u::to_neutral(strong_value<hunter_lab_u::concrete_type> self)
{
  const auto l = self.dataspace_value[0];
  const auto a = self.dataspace_value[1];
  const auto b = self.dataspace_value[2];

  const auto x = (a / 17.5) * (l / 10.0);
  const auto y = l * l / 100.;
  const auto z = b / 7.0 * l / 10.0;

  ossia::xyz xyz{(float)((x + y) / 1.02), (float)y, (float)(-(z - y) / 0.847)};
  return xyz;
}

hunter_lab_u::value_type
hunter_lab_u::from_neutral(strong_value<hunter_lab_u::neutral_unit> self)
{
  // taken from
  // https://github.com/berendeanicolae/ColorSpace/blob/master/src/Conversion.cpp
  // should be parametrized on white
  ossia::xyz xy{self};

  const auto x = xy.dataspace_value[0];
  const auto y = xy.dataspace_value[1];
  const auto z = xy.dataspace_value[2];
  const auto sqrt_y = std::sqrt(y);

  const auto l = 10.0 * sqrt_y;
  const auto a = (y != 0) ? (17.5 * (1.02 * x - y) / sqrt_y) : 0.;
  const auto b = (y != 0) ? (7.0 * (y - 0.847 * z) / sqrt_y) : 0.;

  return make_vec(l, a, b);
}

// ORIENTATION //
strong_value<euler_u::neutral_unit>
euler_u::to_neutral(strong_value<euler_u::concrete_type> self)
{
  const auto yaw = self.dataspace_value[0] * deg_to_rad * -0.5;
  const auto pitch = self.dataspace_value[1] * deg_to_rad * 0.5;
  const auto roll = self.dataspace_value[2] * deg_to_rad * 0.5;

  const auto sinYaw = std::sin(yaw);
  const auto cosYaw = std::cos(yaw);
  const auto sinPitch = std::sin(pitch);
  const auto cosPitch = std::cos(pitch);
  const auto sinRoll = std::sin(roll);
  const auto cosRoll = std::cos(roll);
  const auto cosPitchCosRoll = cosPitch * cosRoll;
  const auto sinPitchSinRoll = sinPitch * sinRoll;

  return std::array<double, 4>{
      cosYaw * sinPitch * cosRoll - sinYaw * cosPitch * sinRoll, // X
      cosYaw * cosPitch * sinRoll + sinYaw * sinPitch * cosRoll, // Y
      sinYaw * cosPitchCosRoll + cosYaw * sinPitchSinRoll,       // Z
      cosYaw * cosPitchCosRoll - sinYaw * sinPitchSinRoll        // W
  };
}

euler_u::value_type
euler_u::from_neutral(strong_value<euler_u::neutral_unit> self)
{
  const auto x = self.dataspace_value[0];
  const auto y = self.dataspace_value[1];
  const auto z = self.dataspace_value[2];
  const auto w = self.dataspace_value[3];

  return {
      (float)(rad_to_deg * std::atan2(-2. * (z * w - x * y), w * w - x * x + y * y - z * z)),
      (float)(rad_to_deg * std::asin(2. * (w * x + y * z))),
      (float)(rad_to_deg * std::atan2(2. * (w * y + x * z), w * w - x * x - y * y + z * z))};
}

strong_value<axis_u::neutral_unit>
axis_u::to_neutral(strong_value<axis_u::concrete_type> self)
{
  const auto x = self.dataspace_value[0];
  const auto y = self.dataspace_value[1];
  const auto z = self.dataspace_value[2];
  const auto angle = self.dataspace_value[3] * deg_to_rad * 0.5;

  const auto sinAngle = std::sin(angle);

  // vector normalization:
  const auto norm = ossia::norm(x, y, z);
  const auto n = norm > 0.0 ? 1.0 / norm : norm;

  /* x = x * n;
    y = y * n;
    z = z * n; */

  return std::array<double, 4>{
      x * n * sinAngle, // X
      y * n * sinAngle, // Y
      z * n * sinAngle, // Z
      std::cos(angle)   // W
  };
}

axis_u::value_type
axis_u::from_neutral(strong_value<axis_u::neutral_unit> self)
{
  const auto x = self.dataspace_value[0];
  const auto y = self.dataspace_value[1];
  const auto z = self.dataspace_value[2];
  const auto w = self.dataspace_value[3];

  const auto sin_a = std::sqrt(1.0 - w * w);

  const auto sin_a2 = std::fabs(sin_a) < 0.0005 ? 1.0 : 1.0 / sin_a;

  return {(float)(x * sin_a2), (float)(y * sin_a2), (float)(z * sin_a2),
          (float)(rad_to_deg * 2.0 * std::atan2(sin_a, w))};
}

// POSITION //

strong_value<spherical_u::neutral_unit>
spherical_u::to_neutral(strong_value<spherical_u::concrete_type> self)
{
  const auto a = self.dataspace_value[0] * deg_to_rad;
  const auto e = self.dataspace_value[1] * deg_to_rad;
  const auto d = self.dataspace_value[2];

  const auto temp = std::cos(e) * d;

  return strong_value<neutral_unit>{(float)(std::sin(a) * temp),
                                    (float)(std::cos(a) * temp),
                                    (float)(std::sin(e) * d)};
}

spherical_u::value_type
spherical_u::from_neutral(strong_value<spherical_u::neutral_unit> self)
{
  const auto x = self.dataspace_value[0];
  const auto y = self.dataspace_value[1];
  const auto z = self.dataspace_value[2];

  const auto temp = ipow(x, 2) + ipow(y, 2);

  return {(float)(std::atan2(y, x) * rad_to_deg),
          (float)(std::atan2(z, std::sqrt(temp)) * rad_to_deg),
          (float)(std::sqrt(temp + ipow(z, 2)))};
}

strong_value<cylindrical_u::neutral_unit>
cylindrical_u::to_neutral(strong_value<cylindrical_u::concrete_type> self)
{
  const auto d = self.dataspace_value[0];
  const auto a = self.dataspace_value[1] * deg_to_rad;
  const auto z = self.dataspace_value[2];

  return {(float)(std::sin(a) * d), (float)(std::cos(a) * d), z};
}

cylindrical_u::value_type
cylindrical_u::from_neutral(strong_value<cylindrical_u::neutral_unit> self)
{
  const auto x = self.dataspace_value[0];
  const auto y = self.dataspace_value[1];
  const auto z = self.dataspace_value[2];

  return {(float)(ossia::norm(x, y)), (float)(std::atan2(y, x) * rad_to_deg),
          z};
}
}
