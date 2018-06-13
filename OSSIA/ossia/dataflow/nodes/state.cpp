#include <ossia/dataflow/nodes/state.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia::nodes
{
state_writer::state_writer(const ossia::state& other): data{other} { }

state_writer::state_writer(ossia::state&& other): data{std::move(other)} { }

state_writer::~state_writer() = default;

void state_writer::run(ossia::token_request, ossia::exec_state_facade e) noexcept
{
  e.insert(data);
}

}
