// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/apply.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_print_visitor.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{
void launch(state_element& e)
{
  ossia::apply(state_execution_visitor{}, e);
}

std::ostream& print(std::ostream& out, const state_element& e)
{
  ossia::apply(state_print_visitor{out, {}}, e);
  return out;
}
}
