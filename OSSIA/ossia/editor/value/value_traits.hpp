#pragma once
#include <ossia/editor/value/value.hpp>

namespace ossia
{

template <typename T, std::size_t N>
class Vec;

template <typename T>
struct value_trait;

template <>
struct value_trait<ossia::Impulse>
{
  using ossia_type = ossia::Impulse;
  using value_type = void;
  static const constexpr auto ossia_enum = val_type::IMPULSE;
  static const constexpr bool is_numeric = false;
};

template <>
struct value_trait<ossia::Int>
{
  using ossia_type = ossia::Int;
  using value_type = int;
  static const constexpr auto ossia_enum = val_type::INT;
  static const constexpr bool is_numeric = true;
};

template <>
struct value_trait<ossia::Float>
{
  using ossia_type = ossia::Float;
  using value_type = float;
  static const constexpr auto ossia_enum = val_type::FLOAT;
  static const constexpr bool is_numeric = true;
};

template <>
struct value_trait<ossia::Bool>
{
  using ossia_type = ossia::Bool;
  using value_type = bool;
  static const constexpr auto ossia_enum = val_type::BOOL;
  static const constexpr bool is_numeric = true;
};

template <>
struct value_trait<ossia::Char>
{
  using ossia_type = ossia::Char;
  using value_type = char;
  static const constexpr auto ossia_enum = val_type::CHAR;
  static const constexpr bool is_numeric = true;
};

template <>
struct value_trait<ossia::String>
{
  using ossia_type = ossia::String;
  using value_type = std::string;
  static const constexpr auto ossia_enum = val_type::STRING;
  static const constexpr bool is_numeric = false;
};

template <>
struct value_trait<ossia::Tuple>
{
  using ossia_type = ossia::Tuple;
  using value_type = std::vector<value>;
  static const constexpr auto ossia_enum = val_type::TUPLE;
  static const constexpr bool is_numeric = false;
};

template <>
struct value_trait<ossia::Behavior>
{
  using ossia_type = ossia::Behavior;
  using value_type = std::shared_ptr<curve_abstract>;
  static const constexpr auto ossia_enum = val_type::BEHAVIOR;
  static const constexpr bool is_numeric = false;
};

template <>
struct value_trait<ossia::Destination>
{
  using ossia_type = ossia::Destination;
  using value_type = ossia::net::address_base*;
  static const constexpr auto ossia_enum = val_type::DESTINATION;
  static const constexpr bool is_numeric = false;
};

template <>
struct value_trait<ossia::Vec<float, 2>>
{
  using ossia_type = ossia::Vec<float, 2>;
  using value_type = std::array<float, 2>;
  static const constexpr auto ossia_enum = val_type::VEC2F;
  static const constexpr bool is_numeric = false;
};

template <>
struct value_trait<ossia::Vec<float, 3>>
{
  using ossia_type = ossia::Vec<float, 3>;
  using value_type = std::array<float, 3>;
  static const constexpr auto ossia_enum = val_type::VEC3F;
  static const constexpr bool is_numeric = false;
};

template <>
struct value_trait<ossia::Vec<float, 4>>
{
  using ossia_type = ossia::Vec<float, 4>;
  using value_type = std::array<float, 4>;
  static const constexpr auto ossia_enum = val_type::VEC4F;
  static const constexpr bool is_numeric = false;
};


// Reverse mapping from implementation type to ossia type
template <typename T>
struct matching_value { using type = void; };
template <>
struct matching_value<int> { using type = Int; };
template <>
struct matching_value<float> { using type = Float; };
template <>
struct matching_value<double> { using type = Float; };
template <>
struct matching_value<char> { using type = Char; };
template <>
struct matching_value<std::string> { using type = String; };
template <>
struct matching_value<std::vector<value>> { using type = Tuple; };
template <std::size_t N>
struct matching_value<std::array<float, N>> { using type = Vec<float, N>; };


template <typename T>
using matching_value_t = typename matching_value<T>::type;
}
