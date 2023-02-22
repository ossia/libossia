#pragma once
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/optional.hpp>
namespace ossia
{
class graph_node;
struct bench_map : ossia::hash_map<const ossia::graph_node*, std::optional<int64_t>>
{
  using ossia::hash_map<
      const ossia::graph_node*, std::optional<int64_t>>::hash_map;
  bool measure{};
};
}
