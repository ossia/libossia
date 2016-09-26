#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>

namespace ossia
{
struct radian_u;
template<typename Impl>
struct angle_unit
{
  using is_unit = std::true_type;
  using neutral_unit = radian_u;
  using value_type = Float;
  using concrete_type = Impl;
  using dataspace_type = struct angle_dataspace;
};

struct radian_u : public angle_unit<radian_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("radian", "rad"); }

  static constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.value;
  }
};

struct degree_u : public angle_unit<degree_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("degree", "deg"); }

  static constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {self.value.value * ossia::deg_to_rad};
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {self.value.value * ossia::rad_to_deg};
  }
};

using degree = strong_value<degree_u>;
using radian = strong_value<radian_u>;

using angle_u =
  eggs::variant<
    degree_u, radian_u>;
}
