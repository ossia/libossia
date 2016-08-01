#pragma once
#include <initializer_list>
#include <ossia_export.h>
#include <vector>

namespace ossia
{
class value;

/**
 * @class Tuple tuple.hpp ossia/editor/value/value.hpp
 *
 * A tuple is a vector of \ref ossia::value.
 * Using a tuple incurs a dynamic allocation.
 */
struct OSSIA_EXPORT Tuple
{
  using value_type = std::vector<ossia::value>;
  value_type value;

  Tuple();
  Tuple(const Tuple&) = default;
  Tuple(Tuple&&) = default;
  Tuple& operator=(const Tuple&) = default;
  Tuple& operator=(Tuple&&) = default;

  explicit Tuple(const ossia::value& v);

  Tuple(std::initializer_list<ossia::value> v);

  Tuple(const value_type& v);
  Tuple(value_type&& v);

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}
