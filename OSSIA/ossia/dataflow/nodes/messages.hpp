#pragma once
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/state/message.hpp>

namespace ossia::nodes
{
class messages final : public ossia::graph_node
{
  public:
    messages()
    {

    }

    void run(ossia::token_request, ossia::execution_state& e) override
    {
      for(auto& msg : data)
      {
        e.insert(msg.dest.address(), ossia::tvalue{ msg.message_value, msg.dest.index, msg.dest.unit });
      }
    }

    std::vector<ossia::message> data;
};
}
