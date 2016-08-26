#pragma once
#include <eggs/variant.hpp>
namespace ossia
{
template <typename T, int N>
class Vec;
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

class Impulse;
class Bool;
class Int;
class Float;
class Char;
class String;
class Tuple;
class Behavior;
class Destination;

namespace net
{
template <typename T>
struct domain_base;

using domain
    = eggs::variant<domain_base<Impulse>, domain_base<Bool>, domain_base<Int>,
                    domain_base<Float>, domain_base<Char>, domain_base<String>,
                    domain_base<Tuple>, domain_base<Vec2f>, domain_base<Vec3f>,
                    domain_base<Vec4f>, domain_base<Destination>,
                    domain_base<Behavior>>;
}
}
