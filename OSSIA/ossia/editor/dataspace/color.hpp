#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>

namespace ossia
{

class rgb_u;
template<typename Impl>
struct color_unit
{
  using neutral_unit = argb_u;
  using concrete_type = Impl;
  using dataspace_type = struct color_dataspace;
};

struct argb_u : public color_unit<argb_u>
{
  using value_type = Vec4f;

  static constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.val.value;
  }
};

struct rgba_u : public color_unit<rgba_u>
{
  using value_type = Vec4f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{self.val.value[3], self.val.value[0], self.val.value[1], self.val.value[2]}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{self.val.value[1], self.val.value[2], self.val.value[3], self.val.value[0]}};
  }
};

struct rgb_u : public color_unit<rgb_u>
{
  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{1., self.val.value[0], self.val.value[1], self.val.value[2]}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{self.val.value[1], self.val.value[2], self.val.value[3]}};
  }
};

struct bgr_u : public color_unit<bgr_u>
{
  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{1., self.val.value[2], self.val.value[1], self.val.value[0]}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{self.val.value[3], self.val.value[2], self.val.value[1]}};
  }
};

struct argb8_u : public color_unit<argb8_u>
{
  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{self.val.value[0] / 255., self.val.value[1] / 255., self.val.value[2] / 255., self.val.value[3] / 255.}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{self.val.value[0] * 255., self.val.value[1] * 255., self.val.value[2] * 255., self.val.value[3] * 255.}};
  }
};

struct hsv_u : public color_unit<hsv_u>
{
  using value_type = Vec3f;

};

struct hsl_u : public color_unit<hsl_u>
{
  using value_type = Vec3f;

};

struct cmy8_u : public color_unit<cmy8_u>
{
  using value_type = Vec3f;

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{(255. - self.val.value[1]) / 255.,
              (255. - self.val.value[2]) / 255.,
              (255. - self.val.value[3]) / 255.}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{{255. * (1. - self.val.value[1]),
              255. * (1. - self.val.value[2]),
              255. * (1. - self.val.value[3])}}};
  }
};

struct cmyk8_u : public color_unit<cmyk8_u>
{
  using value_type = Vec4f;

};

struct xyz_u : public color_unit<xyz_u>
{
  using value_type = Vec3f;

};

struct yxy_u : public color_unit<yxy_u>
{
  using value_type = Vec3f;
};

struct hunter_lab_u : public color_unit<hunter_lab_u>
{
  using value_type = Vec3f;
};

struct cie_lab_u : public color_unit<cie_lab_u>
{
  using value_type = Vec3f;
};

struct cie_luv_u : public color_unit<cie_luv_u>
{
  using value_type = Vec3f;
};
}
