#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

#include <random>

namespace ossia::nodes
{
struct rand_float final : public ossia::nonowning_graph_node
{
  ossia::value_outlet value_out;

public:
  std::uniform_real_distribution<float> dist;
  rand_float(float min, float max)
      : dist{min, max}
  {
    m_outlets.push_back(&value_out);
  }

  std::string label() const noexcept override { return "rand_float"; }
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    thread_local std::mt19937 gen;
    auto& out = *value_out.target<ossia::value_port>();
    auto tm = e.timings(t);
    out.write_value(dist(gen), tm.start_sample);
  }
};
}
