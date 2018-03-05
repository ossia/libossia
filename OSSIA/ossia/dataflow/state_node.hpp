#pragma once
#include <ossia/detail/apply.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/dataflow/execution_state.hpp>

namespace ossia {
struct state_node final : public ossia::graph_node
{
  public:
    state_node(const ossia::state& other): data{other} { }
    state_node(ossia::state&& other): data{std::move(other)} { }
    state_node(): data{} { }

    ~state_node() override
    {

    }

    void run(ossia::token_request, ossia::execution_state& e) override
    {
      e.insert(data);
    }

    ossia::state data;
};

}

