#pragma once
#include <array>

namespace ossia
{
class value;
template <typename T, int N>
class Vec
{
public:
  static const constexpr int size_value = N;

  std::array<T, N> value;

  Vec() = default;
  Vec(std::array<T, N> v) : value(std::move(v))
  {
  }

  Vec(const Vec&) = default;
  Vec(Vec&&) = default;
  Vec& operator=(const Vec&) = default;
  Vec& operator=(Vec&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};

using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;
}
