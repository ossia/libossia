#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/detail/algorithms.hpp>

#include <ossia/detail/math.hpp>
#include <brigand/algorithms/transform.hpp>
#include <ratio>
namespace ossia
{
// These algorithms are a more statically typed version
// of the ones found in the Jamoma dataspace library.
// Credits : Tim Place, Nils Peters, Trond Lossius, and certainly others.
// This library also tries to avoid dynamic allocation whenever possible.
template<typename Unit>
struct strong_value
{
  using unit_type = Unit;
  using value_type = typename Unit::value_type;
  value_type val;

  template<typename U,
           typename = std::enable_if_t<
             std::is_constructible<value_type, U>::value>>
  constexpr strong_value(U other): val{other} { }

  constexpr strong_value(value_type f): val{f} { }

  constexpr strong_value(typename value_trait<value_type>::value_type f): val{f} { }
  template<typename U,
           typename = std::enable_if_t<
              std::is_same<
               typename U::dataspace_type,
               typename unit_type::dataspace_type>::value>>
  constexpr strong_value(strong_value<U> other):
    val{unit_type::from_neutral(U::to_neutral(other))}
  {
  }

  constexpr strong_value<Unit>(const strong_value<Unit>& other): val{other.val} { }
};

template<typename T, typename Ratio_T>
struct linear_unit : public T
{
  static constexpr strong_value<typename T::neutral_unit>
    to_neutral(strong_value<typename T::concrete_type> self)
  {
    return {self.val.value * ratio()};
  }

  static constexpr typename T::value_type
    from_neutral(strong_value<typename T::neutral_unit> self)
  {
    return self.val.value / ratio();
  }

  static constexpr double ratio()
  {
    return double(Ratio_T::num) / double(Ratio_T::den);
  }
};

template<typename T>
struct unit_traits
{
  static constexpr auto text() { return T::text(); }
};
}
