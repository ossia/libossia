#pragma once
#include <ossia/detail/math.hpp>
#include <ossia/network/dataspace/dataspace_base.hpp>
#include <ossia/network/domain/domain_base_impl.hpp>
namespace ossia
{
struct angle_u;
struct radian_u;
template <typename Impl>
struct angle_unit
{
  using is_unit = std::true_type;
  using neutral_unit = radian_u;
  using value_type = float;
  using concrete_type = Impl;
  using dataspace_type = angle_u;
  using is_multidimensional = std::false_type;
};

struct OSSIA_EXPORT radian_u : public angle_unit<radian_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("radian", "rad"));
  }

  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }

  static ossia::domain_base<float> domain()
  {
    return {0.f, float(ossia::two_pi)};
  }

  static constexpr auto bounding()
  {
    return ossia::bounding_mode::WRAP;
  }
};

struct OSSIA_EXPORT degree_u : public angle_unit<degree_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("degree", "deg"));
  }

  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return {self.dataspace_value * ossia::deg_to_rad};
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value * ossia::rad_to_deg;
  }

  static ossia::domain_base<float> domain()
  {
    return {0.f, 360.f};
  }

  static constexpr auto bounding()
  {
    return ossia::bounding_mode::WRAP;
  }
};
}
