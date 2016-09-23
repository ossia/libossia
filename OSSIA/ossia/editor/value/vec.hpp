#pragma once
#include <array>
#include <ossia_export.h>

namespace ossia
{
class value;

/**
 * @class Vec vec.hpp ossia/editor/value/value.hpp
 *
 * A class optimized for holding a fixed number of values.
 * For instance, we know for sure that a 3D position has three components.
 * Hence we can use Vec instead of Tuple in order to avoid a dynamic allocation.
 *
 * \see Tuple
 */
template <typename T, int N>
class Vec
{
public:
  static const constexpr int size_value = N;

  std::array<T, N> value;

  constexpr Vec() { }
  constexpr Vec(std::array<float, N> v) : value(std::move(v))
  {
  }
  constexpr Vec(std::array<double, N> v)
  {
    for(int i = 0; i < N; i++)
    {
      value[i] = v[i];
    }
  }

  constexpr Vec(const Vec&) = default;
  constexpr Vec(Vec&&) = default;
  constexpr Vec& operator=(const Vec&) = default;
  constexpr Vec& operator=(Vec&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};
}

namespace ossia
{
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;
}
