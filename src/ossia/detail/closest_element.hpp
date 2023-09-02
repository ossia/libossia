#pragma once
#include <ossia/detail/hash_map.hpp>

#include <cmath>

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace ossia
{
namespace detail
{
struct lower_bound_helper
{
  template <typename T, typename U>
  auto operator()(const T& vec, const U& val) const noexcept
  {
    return std::lower_bound(vec.begin(), vec.end(), val);
  }
  template <typename T, typename U>
    requires requires(T vec, U val) { vec.lower_bound(val); }
  auto operator()(const T& vec, const U& val) const noexcept
  {
    return vec.lower_bound(val);
  }
};
struct map_key_helper
{
  template <typename T>
    requires requires(T it) { it->first; }
  auto operator()(const T& it) const noexcept
  {
    return it->first;
  }

  template <typename T>
  auto operator()(const T& it) const noexcept
  {
    return *it;
  }
};
}

template <typename T, typename U>
const auto& closest_element(const T& vec, const U& val) noexcept
{
  using namespace std;
  using namespace ossia::detail;
  assert(!vec.empty());

  auto it = lower_bound_helper{}(vec, val);
  if(it != vec.cend())
  {
    if(it != vec.cbegin())
    {
      auto prev = it - 1;
      if(abs(map_key_helper{}(prev)-val) < abs(map_key_helper{}(it)-val))
        return *prev;
      else
        return *it;
    }
    else
    {
      return *it;
    }
  }
  else
  {
    return *vec.crbegin();
  }
}
template <typename T, typename U>
auto closest_element_it(const T& vec, const U& val) noexcept
{
  using namespace std;
  using namespace ossia::detail;
  if(vec.empty())
    return vec.cend();

  auto it = lower_bound_helper{}(vec, val);
  if(it != vec.cend())
  {
    if(it != vec.cbegin())
    {
      auto prev = it - 1;
      if(std::abs(map_key_helper{}(prev)-val) < std::abs(map_key_helper{}(it)-val))
        return prev;
      else
        return it;
    }
    else
    {
      return it;
    }
  }
  else
  {
    return vec.end() - 1;
  }
}

template <typename T, typename U>
auto closest_next_element(T it, T end, const U& val) noexcept
{
  using namespace std;
  using namespace ossia::detail;
  auto next_it = it + 1;
  while(next_it != end)
  {
    if(std::abs(map_key_helper{}(it)-val) < std::abs(map_key_helper{}(next_it)-val))
      return it;
    it = next_it;
    ++next_it;
  }

  return it;
}

template <typename T, typename U>
  requires requires(T it) {
             it->first;
             it->second;
           }
auto closest_next_element(T it, T end, const U& val) noexcept
{
  using namespace std;
  using namespace ossia::detail;
  auto next_it = it + 1;
  while(next_it != end)
  {
    // why the hell is this needed ?!
    if(std::abs(map_key_helper{}(it)-val) < std::abs(map_key_helper{}(next_it)-val))
      return it;
    it = next_it;
    ++next_it;
  }

  return it;
}
}
