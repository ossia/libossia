#pragma once
#include <eggs/variant.hpp>
#include <Editor/Value/ValueBase.h>

namespace OSSIA
{

class SafeValue
{
        eggs::variant<Impulse, Bool, Int, Float, Char, String, Tuple, Vec2f, Vec3f, Vec4f, Destination, Behavior> v ;
};
}
