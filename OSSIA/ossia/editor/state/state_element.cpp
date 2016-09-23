#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_print_visitor.hpp>
#include <ossia/editor/state/state_element.hpp>

template class eggs::variant<ossia::message, ossia::state, ossia::custom_state, ossia::piecewise_message>;
namespace ossia
{
void launch(const state_element& e)
{
  ossia::apply(state_execution_visitor{}, e);
}

std::ostream& print(std::ostream& out, const state_element& e)
{
  ossia::apply(state_print_visitor{out, {}}, e);
  return out;
}
}
