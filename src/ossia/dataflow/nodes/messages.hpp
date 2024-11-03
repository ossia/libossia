#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/editor/state/message.hpp>

namespace ossia::nodes
{
// This is only used for testing so not very efficient by design
class messages final : public ossia::graph_node
{
public:
  messages() { }

  std::string label() const noexcept override { return "messages"; }
  void run(const ossia::token_request&, ossia::exec_state_facade e) noexcept override
  {
    for(auto& msg : data)
    {
      ossia::value_port p;
      p.add_local_value(
          ossia::typed_value{msg.message_value, msg.dest.index, msg.dest.unit});

      e.impl->insert(msg.dest.address(), p);
    }
  }

  std::vector<ossia::message> data;
};
}
