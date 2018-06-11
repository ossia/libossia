#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia::nodes
{
struct OSSIA_EXPORT state final : public ossia::graph_node
{
  public:
    state(const ossia::state& other);
    state(ossia::state&& other);
    state() = default;

    ~state() override;

    void run(ossia::token_request, ossia::exec_state_facade e) noexcept override;

    ossia::state data;
};

}

