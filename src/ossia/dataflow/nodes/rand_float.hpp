#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

#include <random>

namespace ossia::nodes
{
struct rand_float final : public ossia::nonowning_graph_node
{
  ossia::outlet value_out{ossia::value_port{}};

public:
  std::uniform_real_distribution<float> dist;
  rand_float(float min, float max) : dist{min, max}
  {
    m_outlets.push_back(&value_out);
  }

  void run(ossia::token_request t, ossia::exec_state_facade) noexcept override
  {
    thread_local std::mt19937 gen;
    auto& out = *value_out.data.target<ossia::value_port>();
    out.write_value(dist(gen), t.tick_start());
  }
};
}
