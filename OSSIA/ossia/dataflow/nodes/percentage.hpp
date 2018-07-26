#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/network/value/destination.hpp>

namespace ossia::nodes
{
class percentage final : public ossia::graph_node
{
  public:
    percentage(ossia::destination d)
    {
      outputs().push_back(ossia::make_outlet<ossia::value_port>(&d.address()));
    }

    void run(ossia::token_request tk, ossia::exec_state_facade e) noexcept override
    {
      outputs().back()->data.target<ossia::value_port>()->write_value((float)tk.position, tk.tick_start());
    }
};
}
