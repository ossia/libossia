#pragma once
#include <ossia/editor/value/value.hpp>

namespace OSSIA
{

template<typename T, int N>
class Vec;

template<typename T>
struct ValueTrait;

template<>
struct ValueTrait<OSSIA::Impulse>
{
        using ossia_type = OSSIA::Impulse;
        using value_type = void;
        static const constexpr auto ossia_enum = Type::IMPULSE;
};

template<>
struct ValueTrait<OSSIA::Int>
{
        using ossia_type = OSSIA::Int;
        using value_type = int;
        static const constexpr auto ossia_enum = Type::INT;
};

template<>
struct ValueTrait<OSSIA::Float>
{
        using ossia_type = OSSIA::Float;
        using value_type = float;
        static const constexpr auto ossia_enum = Type::FLOAT;
};

template<>
struct ValueTrait<OSSIA::Bool>
{
        using ossia_type = OSSIA::Bool;
        using value_type = bool;
        static const constexpr auto ossia_enum = Type::BOOL;
};

template<>
struct ValueTrait<OSSIA::Char>
{
        using ossia_type = OSSIA::Char;
        using value_type = char;
        static const constexpr auto ossia_enum = Type::CHAR;
};

template<>
struct ValueTrait<OSSIA::String>
{
        using ossia_type = OSSIA::String;
        using value_type = std::string;
        static const constexpr auto ossia_enum = Type::STRING;
};

template<>
struct ValueTrait<OSSIA::Tuple>
{
        using ossia_type = OSSIA::Tuple;
        using value_type = std::vector<Value>;
        static const constexpr auto ossia_enum = Type::TUPLE;
};

template<>
struct ValueTrait<OSSIA::Destination>
{
        using ossia_type = OSSIA::Destination;
        using value_type = OSSIA::net::Node*;
        static const constexpr auto ossia_enum = Type::DESTINATION;
};

template<>
struct ValueTrait<OSSIA::Vec<float, 2>>
{
        using ossia_type = OSSIA::Vec<float, 2>;
        using value_type = std::array<float, 2>;
        static const constexpr auto ossia_enum = Type::VEC2F;
};

template<>
struct ValueTrait<OSSIA::Vec<float, 3>>
{
        using ossia_type = OSSIA::Vec<float, 3>;
        using value_type = std::array<float, 3>;
        static const constexpr auto ossia_enum = Type::VEC3F;
};

template<>
struct ValueTrait<OSSIA::Vec<float, 4>>
{
        using ossia_type = OSSIA::Vec<float, 4>;
        using value_type = std::array<float, 4>;
        static const constexpr auto ossia_enum = Type::VEC4F;
};

}
