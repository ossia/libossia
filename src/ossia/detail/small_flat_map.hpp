#pragma once
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/small_vector.hpp>
namespace ossia
{

template <typename K, typename V, std::size_t N, typename Compare = std::less<void>>
using small_flat_map
    = boost::container::flat_map<K, V, Compare, ossia::small_vector<std::pair<K, V>, N>>;

template <typename K, typename V, std::size_t N, typename Compare = std::less<void>>
using static_flat_map = boost::container::flat_map<
    K, V, Compare, ossia::static_vector<std::pair<K, V>, N>>;

/* does not work as it tries to call pair(piecewise_construct, ...)
template <typename K, typename V, typename Compare = std::less<void>>
using flat_pod_map = fc::flat_map<ossia::pod_vector<pod_pair<K, V>>, Compare>;

template <typename K, typename V, std::size_t N, typename Compare =
std::less<void>> using small_flat_pod_map =
fc::flat_map<ossia::small_pod_vector<pod_pair<K, V>, N>, Compare>;

template <typename K, typename V, std::size_t N, typename Compare =
std::less<void>> using static_flat_pod_map =
fc::flat_map<ossia::static_vector<pod_pair<K, V>, N>, Compare>;
*/
}
