#pragma once
#include <ossia/detail/hash.hpp>

#include <cstdint>

#if defined(_GLIBCXX_DEBUG)
#define OSSIA_NO_FAST_CONTAINERS
#elif defined(_LIBCPP_DEBUG_LEVEL) && _LIBCPP_DEBUG_LEVEL > 0
#define OSSIA_NO_FAST_CONTAINERS
#elif defined(_LIBCPP_ENABLE_ASSERTIONS) && _LIBCPP_ENABLE_ASSERTIONS > 0
#define OSSIA_NO_FAST_CONTAINERS
#elif defined(_LIBCPP_ENABLE_DEBUG_MODE)
#define OSSIA_NO_FAST_CONTAINERS
#elif defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL > 0
#define OSSIA_NO_FAST_CONTAINERS

#endif

#if !defined(OSSIA_NO_FAST_CONTAINERS)
#include <ankerl/unordered_dense.h>
#include <unordered_map>
namespace ossia
{
template <
    typename K, typename H = ossia::hash<K>, typename E = ossia::equal_to<K>,
    typename A = std::allocator<K>>
using hash_set = ankerl::unordered_dense::set<K, H, E, A>;

template <
    typename K, typename V, typename H = ossia::hash<K>, typename E = ossia::equal_to<K>,
    typename A = std::allocator<std::pair<K, V>>>
using hash_map = ankerl::unordered_dense::map<K, V, H, E, A>;

template <
    typename K, typename V, typename H = ossia::hash<K>, typename E = ossia::equal_to<K>,
    typename A = void>
using hash_multimap = std::unordered_multimap<K, V, H, E>;
}
#else
#include <unordered_map>
#include <unordered_set>
namespace ossia
{
template <
    typename K, typename H = ossia::hash<K>, typename E = ossia::equal_to<K>,
    typename A = void>
using hash_set = std::unordered_set<K, H, E>;

template <
    typename K, typename V, typename H = ossia::hash<K>, typename E = ossia::equal_to<K>,
    typename A = void>
using hash_map = std::unordered_map<K, V, H, E>;

template <
    typename K, typename V, typename H = ossia::hash<K>, typename E = ossia::equal_to<K>,
    typename A = void>
using hash_multimap = std::unordered_multimap<K, V, H, E>;
}
#endif
