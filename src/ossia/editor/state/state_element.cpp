// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/value_port.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_print_visitor.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{

void control_message::launch()
{
  // TODO launch should have a timestamp
  port->write_value(this->value, 0);
}

void launch(state_element& e)
{
  ossia::apply(state_execution_visitor{}, e);
}

std::ostream& print(std::ostream& out, const state_element& e)
{
#if defined(OSSIA_HAS_FMT)
  ossia::apply(state_print_visitor{out, {}}, e);
#endif
  return out;
}
}
