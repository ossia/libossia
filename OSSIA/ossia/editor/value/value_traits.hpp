#pragma once
#include <ossia/editor/value/value.hpp>

namespace ossia
{

template <typename T>
struct value_trait;

template <>
struct value_trait<ossia::Impulse>
{
  using ossia_type = ossia::Impulse;
  using value_type = void;
  static const constexpr auto ossia_enum = val_type::IMPULSE;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = false;
};

template <>
struct value_trait<ossia::Int>
{
  using ossia_type = ossia::Int;
  using value_type = int;
  static const constexpr auto ossia_enum = val_type::INT;
  static const constexpr bool is_numeric = true;
  static const constexpr bool is_array = false;
};

template <>
struct value_trait<ossia::Float>
{
  using ossia_type = ossia::Float;
  using value_type = float;
  static const constexpr auto ossia_enum = val_type::FLOAT;
  static const constexpr bool is_numeric = true;
  static const constexpr bool is_array = false;
};

template <>
struct value_trait<ossia::Bool>
{
  using ossia_type = ossia::Bool;
  using value_type = bool;
  static const constexpr auto ossia_enum = val_type::BOOL;
  static const constexpr bool is_numeric = true;
  static const constexpr bool is_array = false;
};

template <>
struct value_trait<ossia::Char>
{
  using ossia_type = ossia::Char;
  using value_type = char;
  static const constexpr auto ossia_enum = val_type::CHAR;
  static const constexpr bool is_numeric = true;
  static const constexpr bool is_array = false;
};

template <>
struct value_trait<ossia::String>
{
  using ossia_type = ossia::String;
  using value_type = std::string;
  static const constexpr auto ossia_enum = val_type::STRING;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = false;
};

template <>
struct value_trait<ossia::Tuple>
{
  using ossia_type = ossia::Tuple;
  using value_type = std::vector<value>;
  static const constexpr auto ossia_enum = val_type::TUPLE;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = true;
};

template <>
struct value_trait<ossia::Destination>
{
  using ossia_type = ossia::Destination;
  using value_type = ossia::net::address_base*;
  static const constexpr auto ossia_enum = val_type::DESTINATION;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = false;
};

template <>
struct value_trait<ossia::Vec2f>
{
  using ossia_type = ossia::Vec2f;
  using value_type = std::array<float, 2>;
  static const constexpr auto ossia_enum = val_type::VEC2F;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = true;
};

template <>
struct value_trait<ossia::Vec3f>
{
  using ossia_type = ossia::Vec3f;
  using value_type = std::array<float, 3>;
  static const constexpr auto ossia_enum = val_type::VEC3F;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = true;
};

template <>
struct value_trait<ossia::Vec4f>
{
  using ossia_type = ossia::Vec4f;
  using value_type = std::array<float, 4>;
  static const constexpr auto ossia_enum = val_type::VEC4F;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = true;
};

template <>
struct value_trait<ossia::value>
{
  using ossia_type = ossia::value;
  using value_type = ossia::value;
  static const constexpr bool is_numeric = false;
  static const constexpr bool is_array = false;
};



// Reverse mapping from implementation type to ossia type
template <typename T>
struct matching_value { };
template <>
struct matching_value<bool> { using type = Bool; };
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

/**
 * @brief is_numeric True if the value is of a numeric type
 * (int, bool, float, char...)
 */
OSSIA_EXPORT
bool is_numeric(const ossia::value& v);

/**
 * @brief is_numeric True if the value is of an array type
 * (tuple, vec...)
 */
OSSIA_EXPORT
bool is_array(const ossia::value& v);

}
