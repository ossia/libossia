#pragma once
#include <eggs/variant.hpp>
#include <ossia/editor/value/value_base.hpp>
namespace ossia
{
class value;
namespace net
{
template <typename T>
struct domain_base;
using domain
    = eggs::variant<domain_base<Impulse>, domain_base<Bool>, domain_base<Int>,
                    domain_base<Float>, domain_base<Char>, domain_base<String>,
                    domain_base<Tuple>, domain_base<Vec2f>, domain_base<Vec3f>,
                    domain_base<Vec4f>, domain_base<ossia::value>>;
}
}
