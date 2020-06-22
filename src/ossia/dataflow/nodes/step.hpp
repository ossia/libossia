#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/pod_vector.hpp>

namespace ossia::nodes
{
class step final : public ossia::graph_node
{
public:
  step()
  {
    m_outlets.push_back(new ossia::value_outlet);
  }

  ~step() override
  {
  }

  void
  run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    // We want to send a trigger for each value change that happened between
    // last_t and now
    if (t.forward())
    {
      auto& port = *m_outlets[0]->target<ossia::value_port>();
      const int64_t d = dur.impl * e.samplesToModel();
      int64_t quo = std::floor(double(t.date.impl) / d);
      int64_t prev_step = d * quo;

      while(t.in_range(time_value{prev_step}))
      {
        port.write_value(
            values[quo % values.size()], t.to_physical_time_in_tick(prev_step, e.modelToSamples()));
        prev_step += d;
        quo++;
      }
    }
  }
  ossia::float_vector values;
  ossia::time_value dur{};

  std::string label() const noexcept override
  {
    return "Step";
  }
};
}
