#pragma once
#include <ossia/dataflow/graph_node.hpp>

namespace ossia::nodes
{
class percentage final : public ossia::graph_node
{
  public:
    percentage(ossia::destination d)
    {
      outputs().push_back(ossia::make_outlet<ossia::value_port>(&d.address()));
    }

    void run(ossia::token_request tok, ossia::execution_state& e) override
    {
      outputs().back()->data.target<ossia::value_port>()->add_raw_value(ossia::tvalue{(float)tok.position});
    }
};
}
