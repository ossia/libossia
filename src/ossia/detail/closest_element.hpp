#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
#include <utility>
#include <map>
#include <cassert>

namespace ossia
{
namespace detail
{
struct lower_bound_helper
{
  template<typename T, typename U>
  auto operator()(const T& vec, const U& val) const noexcept
  {
    return std::lower_bound(vec.begin(), vec.end(), val);
  }
  template<typename K, typename V, typename U>
  auto operator()(const std::map<K, V>& vec, const U& val) const noexcept
  {
    return vec.lower_bound(val);
  }
};
struct map_key_helper
{
  template<typename T>
  auto operator()(const T& it) const noexcept
  {
    return *it;
  }
  template<typename K, typename V>
  auto operator()(typename std::map<K, V>::iterator it) const noexcept
  {
    return it->first;
  }
  template<typename K, typename V>
  auto operator()(typename std::map<K, V>::const_iterator it) const noexcept
  {
    return it->first;
  }
  template<typename... Args>
  auto operator()(typename std::vector<std::pair<Args...>>::iterator it) const noexcept
  {
    return it->first;
  }
  template<typename... Args>
  auto operator()(typename std::vector<std::pair<Args...>>::const_iterator it) const noexcept
  {
    return it->first;
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
  if (it != vec.cend())
  {
    if (it != vec.cbegin())
    {
      auto prev = it - 1;
      if (abs(map_key_helper{}(prev) - val) < abs(map_key_helper{}(it) - val))
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
auto closest_next_element(T it, T end, const U& val) noexcept
{
  using namespace std;
  using namespace ossia::detail;
  auto start_it = it;
  auto next_it = it + 1;
  while(next_it != end)
  {
    if (abs(map_key_helper{}(it) - val) < abs(map_key_helper{}(next_it) - val))
      return it;
    it = next_it;
    ++next_it;
  }

  return it;
}

template <typename K, typename V, typename U>
auto closest_next_element(
    typename std::vector<std::pair<K,V>>::const_iterator it,
    typename std::vector<std::pair<K,V>>::const_iterator end,
    const U& val) noexcept
{
  using namespace std;
  using namespace ossia::detail;
  auto next_it = it + 1;
  while(next_it != end)
  {
    constexpr auto get_key = [] (auto it) { return map_key_helper{}.operator()<K,V>(it); };
    // why the hell is this needed ?!
    if (std::abs(get_key(it) - val) < std::abs(get_key(next_it) - val))
      return it;
    it = next_it;
    ++next_it;
  }

  return it;
}
}
