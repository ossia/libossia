#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/detail/algorithms.hpp>

#include <ossia/detail/math.hpp>
#include <brigand/algorithms/transform.hpp>
#include <ratio>
#include <type_traits>
namespace ossia
{
// These algorithms are a more statically typed version
// of the ones found in the Jamoma dataspace library.
// Credits : Tim Place, Nils Peters, Trond Lossius, and certainly others.
// This library also tries to avoid dynamic allocation whenever possible.

template<typename T, typename U>
using enable_if_same_dataspace = std::enable_if_t<
std::is_same<
 typename T::dataspace_type,
 typename U::dataspace_type>::value>;

template<typename T, typename U>
using enable_if_different_dataspace = std::enable_if_t<
!std::is_same<
 typename T::dataspace_type,
 typename U::dataspace_type>::value>;

template<typename Unit>
struct strong_value
{
  using unit_type = Unit;
  using value_type = typename Unit::value_type;
  using dataspace_type = typename Unit::dataspace_type;
  using neutral_unit = typename Unit::neutral_unit;
  value_type value;

  strong_value() = default;

  // Constructor that takes anyything able to initialize val
  template<typename U,
           typename = std::enable_if_t<
             std::is_constructible<value_type, U>::value>>
  constexpr strong_value(U other):
      value{other}
  {
  }

  // Conversion constructor
  template<typename U,
           typename = enable_if_same_dataspace<U, unit_type>>
  constexpr strong_value(strong_value<U> other):
    value{unit_type::from_neutral(U::to_neutral(other))}
  {
  }

  // Copy constructor
  constexpr strong_value<Unit>(const strong_value<Unit>& other):
      value{other.value}
  {
  }

  friend bool operator==(
      const strong_value& lhs,
      const strong_value& rhs)
  { return lhs.value == rhs.value; }
  friend bool operator!=(
      const strong_value& lhs,
      const strong_value& rhs)
  { return lhs.value != rhs.value; }
};

template<typename T, typename Ratio_T>
struct linear_unit : public T
{
  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<typename T::neutral_unit>
    to_neutral(strong_value<typename T::concrete_type> self)
  {
    return {self.value.value * ratio()};
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR typename T::value_type
    from_neutral(strong_value<typename T::neutral_unit> self)
  {
    return self.value.value / ratio();
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR double ratio()
  {
    return double(Ratio_T::num) / double(Ratio_T::den);
  }
};

template<typename T>
struct unit_traits
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text() { return T::text(); }
};

template<typename T>
struct unit_traits<strong_value<T>>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text() { return unit_traits<T>::text(); }
};

template<typename T>
struct dataspace_traits : public std::false_type {};
}
