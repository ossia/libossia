#include <ossia/dataflow/nodes/state.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia::nodes
{
state::state(const ossia::state& other): data{other} { }

state::state(ossia::state&& other): data{std::move(other)} { }

state::~state() = default;

void state::run(ossia::token_request, ossia::exec_state_facade e) noexcept
{
  e.insert(data);
}

}
