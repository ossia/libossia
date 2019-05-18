#pragma once
#include <ossia/network/value/value.hpp>
#include <ossia/detail/hash.hpp>

#include <functional>

namespace std
{
template<>
struct hash<ossia::impulse>
{

  std::size_t operator()(const ossia::impulse& v) const
  {
    if constexpr(sizeof(std::size_t) == 8)
      return 4412470058203171781ul;
    else
      return 3958788809ul;
  }
};
template<>
struct hash<std::vector<ossia::value>>
{
  inline
  std::size_t operator()(const std::vector<ossia::value>& v) const;
};

template<std::size_t N>
struct hash<std::array<float, N>>
{
  std::size_t operator()(const std::array<float, N>& v) const
  {
    std::size_t seed = 0;
    for(std::size_t i = 0; i < N; i++)
      ossia::hash_combine(seed, v[i]);
    return seed;
  }
};

template<>
struct hash<ossia::value>
{
  std::size_t operator()(const ossia::value& v) const
  {
    std::size_t seed = 0;
    ossia::hash_combine(seed, v.v.which());
    if(v.valid())
      ossia::apply_nonnull([&] (const auto& val) { ossia::hash_combine(seed, val); }, v.v);
    return seed;
  }
};

inline
std::size_t std::hash<std::vector<ossia::value>>::operator()(const std::vector<ossia::value>& v) const
{
  std::size_t seed = 0;
  for(const auto& val : v)
    ossia::hash_combine(seed, hash<ossia::value>{}(val));
  return seed;
}

}
