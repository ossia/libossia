#pragma once
#include <ossia/editor/value/value.hpp>

namespace ossia
{

template<typename T, int N>
class Vec;

template<typename T>
struct ValueTrait;

template<>
struct ValueTrait<ossia::Impulse>
{
        using ossia_type = ossia::Impulse;
        using value_type = void;
        static const constexpr auto ossia_enum = Type::IMPULSE;
        static const constexpr bool is_numeric = false;
};

template<>
struct ValueTrait<ossia::Int>
{
        using ossia_type = ossia::Int;
        using value_type = int;
        static const constexpr auto ossia_enum = Type::INT;
        static const constexpr bool is_numeric = true;
};

template<>
struct ValueTrait<ossia::Float>
{
        using ossia_type = ossia::Float;
        using value_type = float;
        static const constexpr auto ossia_enum = Type::FLOAT;
        static const constexpr bool is_numeric = true;
};

template<>
struct ValueTrait<ossia::Bool>
{
        using ossia_type = ossia::Bool;
        using value_type = bool;
        static const constexpr auto ossia_enum = Type::BOOL;
        static const constexpr bool is_numeric = true;
};

template<>
struct ValueTrait<ossia::Char>
{
        using ossia_type = ossia::Char;
        using value_type = char;
        static const constexpr auto ossia_enum = Type::CHAR;
        static const constexpr bool is_numeric = true;
};

template<>
struct ValueTrait<ossia::String>
{
        using ossia_type = ossia::String;
        using value_type = std::string;
        static const constexpr auto ossia_enum = Type::STRING;
        static const constexpr bool is_numeric = false;
};

template<>
struct ValueTrait<ossia::Tuple>
{
        using ossia_type = ossia::Tuple;
        using value_type = std::vector<value>;
        static const constexpr auto ossia_enum = Type::TUPLE;
        static const constexpr bool is_numeric = false;
};

template<>
struct ValueTrait<ossia::Destination>
{
        using ossia_type = ossia::Destination;
        using value_type = ossia::net::node*;
        static const constexpr auto ossia_enum = Type::DESTINATION;
        static const constexpr bool is_numeric = false;
};

template<>
struct ValueTrait<ossia::Vec<float, 2>>
{
        using ossia_type = ossia::Vec<float, 2>;
        using value_type = std::array<float, 2>;
        static const constexpr auto ossia_enum = Type::VEC2F;
        static const constexpr bool is_numeric = false;
};

template<>
struct ValueTrait<ossia::Vec<float, 3>>
{
        using ossia_type = ossia::Vec<float, 3>;
        using value_type = std::array<float, 3>;
        static const constexpr auto ossia_enum = Type::VEC3F;
        static const constexpr bool is_numeric = false;
};

template<>
struct ValueTrait<ossia::Vec<float, 4>>
{
        using ossia_type = ossia::Vec<float, 4>;
        using value_type = std::array<float, 4>;
        static const constexpr auto ossia_enum = Type::VEC4F;
        static const constexpr bool is_numeric = false;
};

}
