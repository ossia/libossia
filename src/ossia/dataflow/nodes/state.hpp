#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia::nodes
{
struct OSSIA_EXPORT state_writer final : public ossia::graph_node
{
public:
  state_writer(const ossia::state& other);
  state_writer(ossia::state&& other);
  state_writer() = default;

  ~state_writer() override;

  void run(const ossia::token_request&, ossia::exec_state_facade e) noexcept override;
  std::string label() const noexcept override;

  ossia::state data;
  std::vector<ossia::control_message> controls;
};
}
