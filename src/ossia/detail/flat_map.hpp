#pragma once
#include <boost/container/flat_map.hpp>
namespace ossia
{
/* Too hard to make work
template <typename T, typename U>
struct pod_pair {
  using first_type = T;
  using second_type = U;
  T first;
  U second;
};

template<typename K, typename V>
using pod_pair_type = std::conditional_t<
    std::is_trivial_v<pod_pair<K,V>> && std::is_aggregate_v<pod_pair<K,V>>,
    pod_pair<K,V>,
    std::pair<K,V>>;

template<typename K, typename V>
using pod_pair_allocator = std::conditional_t<
    std::is_trivial_v<pod_pair<K,V>> && std::is_aggregate_v<pod_pair<K,V>>,
    pod_allocator<pod_pair<K,V>>,
    std::allocator<std::pair<K,V>>
>;

template <typename K, typename V, typename Compare = std::less<void>, typename
Alloc = pod_pair_allocator<K, V>> using flat_map =
fc::flat_map<std::vector<pod_pair_type<K,V>, Alloc>, Compare>;
*/
template <
    typename K, typename V, typename Compare = std::less<void>,
    typename Alloc = std::allocator<std::pair<K, V>>>
using flat_map = boost::container::flat_map<K, V, Compare, Alloc>;
}
