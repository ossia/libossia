#pragma once
#include <cstdint>

#if defined(_GLIBCXX_DEBUG)
#define OSSIA_NO_FAST_CONTAINERS
#elif defined(_LIBCPP_DEBUG_LEVEL) && _LIBCPP_DEBUG_LEVEL > 0
#define OSSIA_NO_FAST_CONTAINERS
#elif defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL > 0
#define OSSIA_NO_FAST_CONTAINERS
#endif

#if !defined(OSSIA_NO_FAST_CONTAINERS)
#include <ankerl/unordered_dense.h>
namespace ossia
{
template <
    typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using fast_hash_map = ankerl::unordered_dense::map<K, V, H, E>;
}
#else
#include <unordered_map>
namespace ossia
{
template <
    typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using fast_hash_map = std::unordered_map<K, V, H, E>;
}
#endif
