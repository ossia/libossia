#pragma once
#include <atomic>

namespace ossia
{
//! Whether the graph times its nodes. Each node's time goes to its
//! graph_node::bench_tap.
struct bench_state
{
  std::atomic_bool measure{};
};
}
