#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_print_visitor.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/detail/apply.hpp>

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
