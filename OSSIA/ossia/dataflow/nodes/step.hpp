#pragma once
#include <ossia/dataflow/graph_node.hpp>

namespace ossia::nodes
{
class step final :
    public ossia::graph_node
{
public:
  step()
  {
    m_outlets.push_back(ossia::make_outlet<ossia::value_port>());
  }
  ~step()
  {

  }

  void run(ossia::token_request t, ossia::execution_state& e) override
  {
    // We want to send a trigger for each value change that happened between last_t and now
    if(t.date > prev_date())
    {
      auto& port = *m_outlets[0]->data.target<ossia::value_port>();

      // TODO optimizeme... quite naive for now.
      // TODO maybe start from prev_date() + 1 ?
      for(int64_t i = prev_date().impl; i < t.date.impl; i++)
      {
        if(i % dur == 0)
        {
          port.add_value(values[(i / dur) % values.size()], i - prev_date() + t.offset);
        }
      }
    }
  }
  std::vector<float> values;
  ossia::time_value dur{};

  std::string label() const override
  {
    return "Step";
  }
};


}
