#pragma once
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <array>
#include <ossia/detail/instantiations.hpp>

/**
 * This header contains various range-style functions that mimic std::algorithm functions.
 * This won't be necessary anymore when ranges are introduced in C++20 (hopefully).
 */
namespace ossia
{
template <typename Vector>
using iterator_t = typename std::remove_reference<Vector>::type::iterator;

template <typename Vector, typename Value>
auto find(Vector&& v, const Value& val)
{
  return std::find(std::begin(v), std::end(v), val);
}

template <typename Vector, typename Fun>
auto find_if(Vector&& v, Fun fun)
{
  return std::find_if(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Value>
bool contains(Vector&& v, const Value& val)
{
  return find(v, val) != std::end(v);
}

template <typename Vector, typename Value>
void remove_one(Vector&& v, const Value& val)
{
  auto it = find(v, val);
  if (it != v.end())
  {
    v.erase(it);
  }
}

template <typename Vector, typename Fun>
bool any_of(Vector&& v, Fun fun)
{
  return std::any_of(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Fun>
bool none_of(Vector&& v, Fun fun)
{
  return std::none_of(std::begin(v), std::end(v), fun);
}

template <typename Vector, typename Fun>
auto remove_if(Vector&& v, Fun fun)
{
  return std::remove_if(std::begin(v), std::end(v), fun);
}

// https://gist.github.com/klmr/2775736
// note : license unclear, will be available in C++17.
using namespace ossia;
template <typename... T>
constexpr auto make_array(T&&... values) ->
        std::array<
            typename std::decay<
                typename std::common_type<T...>::type>::type,
            sizeof...(T)> {
    return std::array<
        typename std::decay<
            typename std::common_type<T...>::type>::type,
        sizeof...(T)>{std::forward<T>(values)...};
}
}
