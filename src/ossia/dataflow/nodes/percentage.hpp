#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/network/value/destination.hpp>

namespace ossia::nodes
{
class percentage final : public ossia::nonowning_graph_node
{
public:
  ossia::value_outlet outlet;
  percentage(ossia::destination d)
      : outlet{&d.address()}
  {
    m_outlets.push_back(&outlet);
  }

  std::string label() const noexcept override { return "percentage"; }
  void run(const ossia::token_request& tk, ossia::exec_state_facade e) noexcept override
  {
    const auto [tick_start, d] = e.timings(tk);
    outlet->write_value((float)tk.position(), tick_start);
  }
};
}
