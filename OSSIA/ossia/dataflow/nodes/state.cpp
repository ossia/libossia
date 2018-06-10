#include <ossia/dataflow/nodes/state.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia::nodes
{
state::state(const ossia::state& other): data{other} { }

state::state(ossia::state&& other): data{std::move(other)} { }

state::~state() = default;

void state::run(ossia::token_request, ossia::execution_state& e) noexcept
{
  e.insert(data);
}

}
