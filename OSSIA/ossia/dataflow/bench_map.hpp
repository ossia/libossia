#pragma once
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/optional.hpp>
namespace ossia
{
class graph_node;
struct bench_map
    : ossia::fast_hash_map<const ossia::graph_node*, ossia::optional<int64_t>>
{
  using ossia::fast_hash_map<
      const ossia::graph_node*, ossia::optional<int64_t>>::fast_hash_map;
  bool measure{};
};
}
