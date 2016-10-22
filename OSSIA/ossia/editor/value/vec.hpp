#pragma once
#include <ossia/detail/config.hpp>
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
template <typename T, std::size_t N>
class OSSIA_EXPORT Vec
{
public:
  static const constexpr std::size_t size_value = N;
  using value_type = T;
  std::array<T, N> value = {};

  OSSIA_DECL_RELAXED_CONSTEXPR Vec() { }
  OSSIA_DECL_RELAXED_CONSTEXPR Vec(std::array<float, N> v) : value(std::move(v))
  {
  }
  OSSIA_DECL_RELAXED_CONSTEXPR Vec(std::array<double, N> v)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      value[i] = v[i];
    }
  }

  OSSIA_DECL_RELAXED_CONSTEXPR Vec(const Vec&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Vec(Vec&&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Vec& operator=(const Vec&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Vec& operator=(Vec&&) = default;

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;


  bool operator==(const Vec& rhs) const { return value == rhs.value; }
  bool operator!=(const Vec& rhs) const { return value != rhs.value; }
};
}

namespace ossia
{
using Vec2f = Vec<float, 2ul>;
using Vec3f = Vec<float, 3ul>;
using Vec4f = Vec<float, 4ul>;

inline Vec2f make_vec(float f1, float f2) { return std::array<float, 2>{f1, f2}; }
inline Vec3f make_vec(float f1, float f2, float f3) { return std::array<float, 3>{f1, f2, f3}; }
inline Vec4f make_vec(float f1, float f2, float f3, float f4) { return std::array<float, 4>{f1, f2, f3, f4}; }
}
