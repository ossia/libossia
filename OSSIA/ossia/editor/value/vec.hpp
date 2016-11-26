#pragma once
#include <ossia/detail/config.hpp>
#include <array>
#include <ossia_export.h>

namespace ossia
{
class value;

using Vec2f = std::array<float, 2ul>;
using Vec3f = std::array<float, 3ul>;
using Vec4f = std::array<float, 4ul>;

inline Vec2f make_vec(float f1, float f2) { return std::array<float, 2>{f1, f2}; }
inline Vec3f make_vec(float f1, float f2, float f3) { return std::array<float, 3>{f1, f2, f3}; }
inline Vec4f make_vec(float f1, float f2, float f3, float f4) { return std::array<float, 4>{f1, f2, f3, f4}; }
template<std::size_t N>
std::array<float, N> fill_vec(float f)
{
    std::array<float, N> arr;
    arr.fill(f);
    return arr;
}
}
