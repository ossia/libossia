#pragma once
#include <cstdint>

// TODO currently flat_hash_map is not available on 32 bit systems.
#if (INTPTR_MAX == INT64_MAX)
#include <flat_hash_map.hpp>
namespace ossia
{
  template<typename K, typename V>
  using fast_hash_map = ska::flat_hash_map<K, V>;
}

#else
#include <unordered_map>
namespace ossia
{
  template<typename K, typename V>
  using fast_hash_map = std::unordered_map<K, V>;
}
#endif
