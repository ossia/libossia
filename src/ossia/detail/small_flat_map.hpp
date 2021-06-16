#pragma once
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/pod_vector.hpp>
namespace ossia
{

template <typename K, typename V, std::size_t N, typename Compare = std::less<void>>
using small_flat_map = fc::flat_map<ossia::small_vector<std::pair<K, V>, N>, Compare>;

template <typename T, typename U>
struct pod_pair {
  T first;
  U second;
};

template <typename K, typename V, std::size_t N, typename Compare = std::less<void>>
using small_flat_pod_map = fc::flat_map<ossia::small_pod_vector<pod_pair<K, V>, N>, Compare>;
}
