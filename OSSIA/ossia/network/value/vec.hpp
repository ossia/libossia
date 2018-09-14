#pragma once
#include <ossia/detail/config.hpp>

#include <array>

namespace ossia
{
class value;

using vec2f = std::array<float, 2ul>;
using vec3f = std::array<float, 3ul>;
using vec4f = std::array<float, 4ul>;

template <typename T, typename U>
inline vec2f make_vec(T f1, U f2)
{
  return std::array<float, 2>{(float)f1, (float)f2};
}

template <typename T, typename U, typename V>
inline vec3f make_vec(T f1, U f2, V f3)
{
  return std::array<float, 3>{(float)f1, (float)f2, (float)f3};
}

template <typename T, typename U, typename V, typename W>
inline vec4f make_vec(T f1, U f2, V f3, W f4)
{
  return std::array<float, 4>{(float)f1, (float)f2, (float)f3, (float)f4};
}
template <std::size_t N>
std::array<float, N> fill_vec(float f)
{
  std::array<float, N> arr;
  arr.fill(f);
  return arr;
}
}
