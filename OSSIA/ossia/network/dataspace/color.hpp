#pragma once
#include <ossia/network/dataspace/dataspace_base.hpp>

#include <ossia/network/domain/domain_base_impl.hpp>
// Algorithms taken from http://www.easyrgb.com
namespace ossia
{

struct argb_u;
struct color_u;
template <typename Impl>
struct color_unit
{
  using is_unit = std::true_type;
  using is_multidimensional = std::true_type;
  // number of dimensiosn -> decltype(value)::size_value
  using neutral_unit = argb_u;
  using concrete_type = Impl;
  using dataspace_type = color_u;
};

struct OSSIA_EXPORT argb_u : public color_unit<argb_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("argb"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("argb"));
  }

  static ossia::vecf_domain<4> domain() {
    return vecf_domain<4>{
      ossia::make_vec(0.f,0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f,1.f)
    };
  }

  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }

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
};

struct OSSIA_EXPORT rgba8_u : public color_unit<rgba8_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("rgba8"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("rgba"));
  }

  using value_type = vec4f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return make_vec(
        self.dataspace_value[3] / 255., self.dataspace_value[0] / 255.,
        self.dataspace_value[1] / 255., self.dataspace_value[2] / 255.);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return make_vec(
        self.dataspace_value[1] * 255., self.dataspace_value[2] * 255.,
        self.dataspace_value[3] * 255., self.dataspace_value[0] * 255.);
  }

  static ossia::vecf_domain<4> domain() {
    return vecf_domain<4>{
      ossia::make_vec(0.f,0.f,0.f,0.f)
    , ossia::make_vec(255.f,255.f,255.f,255.f)
    };
  }

  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }

};

struct OSSIA_EXPORT rgba_u : public color_unit<rgba_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("rgba"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("rgba"));
  }

  using value_type = vec4f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return {self.dataspace_value[3], self.dataspace_value[0],
            self.dataspace_value[1], self.dataspace_value[2]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[1], self.dataspace_value[2],
            self.dataspace_value[3], self.dataspace_value[0]};
  }

  static ossia::vecf_domain<4> domain() {
    return vecf_domain<4>{
      ossia::make_vec(0.f,0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT rgb_u : public color_unit<rgb_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("rgb"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("rgb"));
  }

  using value_type = vec3f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return {1., self.dataspace_value[0], self.dataspace_value[1],
            self.dataspace_value[2]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[1], self.dataspace_value[2],
            self.dataspace_value[3]};
  }

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }

};

struct OSSIA_EXPORT bgr_u : public color_unit<bgr_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("bgr"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("bgr"));
  }

  using value_type = vec3f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return {1., self.dataspace_value[2], self.dataspace_value[1],
            self.dataspace_value[0]};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.dataspace_value[3], self.dataspace_value[2],
            self.dataspace_value[1]};
  }

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT argb8_u : public color_unit<argb8_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("argb8"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("argb"));
  }

  using value_type = vec4f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return make_vec(
        self.dataspace_value[0] / 255., self.dataspace_value[1] / 255.,
        self.dataspace_value[2] / 255., self.dataspace_value[3] / 255.);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return make_vec(
        self.dataspace_value[0] * 255., self.dataspace_value[1] * 255.,
        self.dataspace_value[2] * 255., self.dataspace_value[3] * 255.);
  }

  static ossia::vecf_domain<4> domain() {
    return vecf_domain<4>{
      ossia::make_vec(0.f,0.f,0.f,0.f)
    , ossia::make_vec(255.f,255.f,255.f,255.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT hsv_u : public color_unit<hsv_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("hsv"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("hsv"));
  }

  using value_type = vec3f;
  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT hsl_u : public color_unit<hsl_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("hsl"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("hsl"));
  }

  using value_type = vec3f;

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT cmy8_u : public color_unit<cmy8_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cmy8"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("cmy"));
  }

  using value_type = vec3f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return make_vec(
        1., (255. - self.dataspace_value[0]) / 255.,
        (255. - self.dataspace_value[1]) / 255.,
        (255. - self.dataspace_value[2]) / 255.);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return make_vec(
        255. * (1. - self.dataspace_value[1]),
        255. * (1. - self.dataspace_value[2]),
        255. * (1. - self.dataspace_value[3]));
  }

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(255.f,255.f,255.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT cmyk8_u : public color_unit<cmyk8_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cmyk8"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("cmyk"));
  }

  using value_type = vec4f;
};

struct OSSIA_EXPORT xyz_u : public color_unit<xyz_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cie_xyz"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("xyz"));
  }

  using value_type = vec3f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self);

  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT yxy_u : public color_unit<yxy_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cie_Yxy"));
  }

  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("Yxy"));
  }

  using value_type = vec3f;

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT hunter_lab_u : public color_unit<hunter_lab_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("hunter_lab"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("lab"));
  }
  using value_type = vec3f;

  static strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self);
  static value_type from_neutral(strong_value<neutral_unit> self);

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT cie_lab_u : public color_unit<cie_lab_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cie_lab"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("lab"));
  }
  using value_type = vec3f;

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};

struct OSSIA_EXPORT cie_luv_u : public color_unit<cie_luv_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("cie_luv"));
  }
  static constexpr auto array_parameters()
  {
    constexpr_return(ossia::make_string_view("luv"));
  }
  using value_type = vec3f;

  static ossia::vecf_domain<3> domain() {
    return vecf_domain<3>{
      ossia::make_vec(0.f,0.f,0.f)
    , ossia::make_vec(1.f,1.f,1.f)
    };
  }
  static constexpr auto bounding() { return ossia::bounding_mode::CLIP; }
};
}
