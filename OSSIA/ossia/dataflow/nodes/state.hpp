#pragma once
#include <ossia/detail/apply.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/dataflow/execution_state.hpp>

namespace ossia::nodes
{
struct state final : public ossia::graph_node
{
  public:
    state(const ossia::state& other): data{other} { }
    state(ossia::state&& other): data{std::move(other)} { }
    state(): data{} { }

    ~state() override
    {

    }

    void run(ossia::token_request, ossia::execution_state& e) override
    {
      e.insert(data);
    }

    ossia::state data;
};
}

