#pragma once
#include <cstdint>

// TODO currently flat_hash_map is not available on 32 bit systems.
#if (INTPTR_MAX == INT64_MAX)
#include <flat_hash_map.hpp>
namespace ossia
{
template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using fast_hash_map = ska::flat_hash_map<K, V, H, E>;
}

#else
#include <unordered_map>
namespace ossia
{
template <typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
using fast_hash_map = std::unordered_map<K, V, H, E>;
}
#endif
