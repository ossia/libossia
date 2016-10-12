#pragma once
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/value/value_conversion.hpp>

namespace ossia
{
template<typename T>
T convert(float f)
{
  return f;
}

namespace detail
{
// Remove with C++17
template<typename... Args>
using void_t = void;

template<typename T, typename = void>
struct is_iterable_t : public std::false_type{};

template<typename T>
struct is_iterable_t<T, void_t<typename T::value_type>> : public std::true_type{};

template<typename T>
const constexpr bool is_iterable_v = is_iterable_t<T>::value;

template<typename T, typename U, typename = void>
struct whole_value_merger_helper;

template<typename T, typename U>
const constexpr bool both_iterable = is_iterable_t<decltype(T::value)>::value && is_iterable_t<decltype(U::value)>::value;
template<typename T, typename U>
const constexpr bool first_iterable = is_iterable_t<decltype(T::value)>::value && !is_iterable_t<decltype(U::value)>::value;
template<typename T, typename U>
const constexpr bool second_iterable = !is_iterable_t<decltype(T::value)>::value && is_iterable_t<decltype(U::value)>::value;
template<typename T, typename U>
const constexpr bool neither_iterable = !is_iterable_t<decltype(T::value)>::value && !is_iterable_t<decltype(U::value)>::value;

template<typename T, typename U>
using enable_if_both_iterable =    std::enable_if_t<both_iterable<T,U>>;
template<typename T, typename U>
using enable_if_first_iterable =   std::enable_if_t<first_iterable<T,U>>;
template<typename T, typename U>
using enable_if_second_iterable =  std::enable_if_t<second_iterable<T,U>>;
template<typename T, typename U>
using enable_if_neither_iterable = std::enable_if_t<neither_iterable<T,U>>;

// Case where both T and U are array types, e.g. RGB && Tuple, or CMYK && Vec2f...
template<typename T, typename U>
struct whole_value_merger_helper<T, U, enable_if_both_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value)
  {
    // copy the most possible values. T's value will be fixed at compile time.
    const auto n = std::min(value_unit.value.value.size(), value.value.size());
    for(std::size_t i = 0; i < n; i++)
    {
      value_unit.value.value[i] = ossia::convert<std::remove_reference_t<decltype(value_unit.value.value[i])>>(value.value[i]);
    }
    return value_unit;
  }
};

// Case "rgb" and "float" -> does not make sense, we return the input
template<typename T, typename U>
struct whole_value_merger_helper<T, U, enable_if_first_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value)
  {
    return value_unit;
  }
};

// Case "centimeter" and "tuple" -> does not make sense, we return the input
template<typename T, typename U>
struct whole_value_merger_helper<T, U, enable_if_second_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value)
  {
    return value_unit;
  }
};

// Case "centimeter" and "float"
template<typename T, typename U>
struct whole_value_merger_helper<T, U, enable_if_neither_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value)
  {
    value_unit.value.value = value.value;
    return value_unit;
  }
};

template<typename T, typename U, typename = void>
struct partial_value_merger_helper;

// T and U are both array types, and we want to merge a single element of U into T.
// This is only valid for destination_index with one depth level : e.g. vu[i] == v[i]
// since all units only have one depth level.
template<typename T, typename U>
struct partial_value_merger_helper<T, U, enable_if_both_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value, const ossia::destination_index& idx)
  {
    auto i = idx[0];
    if(value_unit.value.value.size() > i && value.value.size() > i)
    {
      value_unit.value.value[i] = ossia::convert<std::remove_reference_t<decltype(value_unit.value.value[i])>>(value.value[i]);
    }

    return value_unit;
  }

  template<std::size_t N>
  ossia::value_with_unit operator()(T value_unit, const U& value, const std::bitset<N>& idx)
  {
    if(handle_vec(value_unit.value, value, idx))
      return value_unit;
    return {};
  }

  template<std::size_t N>
  bool handle_vec(Vec<float, N>& src, const Vec<float, N>& incoming, const std::bitset<N>& idx)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      if(idx.test(i))
      {
        src.value[i] = incoming.value[i];
      }
    }
    return true;
  }

  template<typename... Args>
  bool handle_vec(Args&&...)
  {
    return false;
  }
};

template<typename T, typename U>
struct partial_value_merger_helper<T, U, enable_if_first_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value, const ossia::destination_index& idx)
  {
    auto i = idx[0];
    value_unit.value.value[i] = ossia::convert<std::remove_reference_t<decltype(value_unit.value.value[i])>>(value.value);
    return value_unit;
  }

  template<std::size_t N>
  ossia::value_with_unit operator()(T value_unit, const U& value, const std::bitset<N>& idx)
  {
    return {};
  }
};

template<typename T, typename U>
struct partial_value_merger_helper<T, U, enable_if_second_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value, const ossia::destination_index& idx)
  {
    return value_unit;
  }

  template<std::size_t N>
  ossia::value_with_unit operator()(T value_unit, const U& value, const std::bitset<N>& idx)
  {
    return {};
  }
};

template<typename T, typename U>
struct partial_value_merger_helper<T, U, enable_if_neither_iterable<decltype(T::value), U>>
{
  ossia::value_with_unit operator()(T value_unit, const U& value, const ossia::destination_index& idx)
  {
    return value_unit;
  }

  template<std::size_t N>
  ossia::value_with_unit operator()(T value_unit, const U& value, const std::bitset<N>& idx)
  {
    return {};
  }
};


struct value_merger
{
  const ossia::destination_index& index;

  template<typename T, typename U>
  ossia::value_with_unit operator()(const strong_value<T>& value_unit, const U& value)
  {
      auto n = index.size();
      switch(n)
      {
        case 0:
          return whole_value_merger_helper<strong_value<T>, U>{}(value_unit, value);
        case 1:
          // The index is where the value should go
          return partial_value_merger_helper<strong_value<T>, U>{}(value_unit, value, index);
        default:
          // Cannot merge anything
          return value_unit;
      }
  }

  template<typename T>
  ossia::value_with_unit operator()(const strong_value<T>& value_unit, Impulse value)
  {
    return value_unit;
  }
  template<typename T>
  ossia::value_with_unit operator()(const strong_value<T>& value_unit, const Destination& value)
  {
    return value_unit;
  }
  template<typename T>
  ossia::value_with_unit operator()(const strong_value<T>& value_unit, const Behavior& value)
  {
    return value_unit;
  }
};

template<std::size_t N>
struct vec_value_merger
{
  const std::bitset<N>& index;

  template<typename T, typename U>
  ossia::value_with_unit operator()(const strong_value<T>& value_unit, const U& value)
  {
    return {};
  }

  template<typename T>
  ossia::value_with_unit operator()(const strong_value<T>& value_unit, const Vec<float, N>& value)
  {
    if(index.all())
      return whole_value_merger_helper<strong_value<T>, Vec<float, N>>{}(value_unit, value);
    else if(index.any())
      return partial_value_merger_helper<strong_value<T>, Vec<float, N>>{}(value_unit, value, index);
    else
      return value_unit;
  }
};
}
}
