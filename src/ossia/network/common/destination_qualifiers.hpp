#pragma once
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/dataspace/dataspace.hpp>

namespace ossia
{
struct OSSIA_EXPORT destination_qualifiers
{
  ossia::destination_index accessors;
  ossia::unit_t unit;

  bool operator==(const destination_qualifiers& a) const
  {
    return accessors == a.accessors && unit == a.unit;
  }

  bool operator!=(const destination_qualifiers& a) const
  {
    return accessors != a.accessors || unit != a.unit;
  }
};
static_assert(std::is_nothrow_default_constructible_v<ossia::destination_index>);
static_assert(std::is_nothrow_default_constructible_v<ossia::unit_t>);
static_assert(std::is_nothrow_default_constructible_v<ossia::destination_qualifiers>);
static_assert(std::is_nothrow_move_constructible_v<ossia::destination_index>);
static_assert(std::is_nothrow_move_constructible_v<ossia::unit_t>);
static_assert(std::is_nothrow_move_constructible_v<ossia::destination_qualifiers>);
static_assert(std::is_nothrow_move_assignable_v<ossia::destination_index>);
static_assert(std::is_nothrow_move_assignable_v<ossia::unit_t>);
static_assert(std::is_nothrow_move_assignable_v<ossia::destination_qualifiers>);
}
