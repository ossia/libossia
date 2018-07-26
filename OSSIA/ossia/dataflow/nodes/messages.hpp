#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/data.hpp>
#include <ossia/editor/state/message.hpp>

namespace ossia::nodes
{
class messages final : public ossia::graph_node
{
  public:
    messages()
    {

    }

    void run(ossia::token_request, ossia::exec_state_facade e) noexcept override
    {
      for(auto& msg : data)
      {
        e.insert(msg.dest.address(), ossia::typed_value{ msg.message_value, msg.dest.index, msg.dest.unit });
      }
    }

    std::vector<ossia::message> data;
};
}
