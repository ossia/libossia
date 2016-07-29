#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_print_visitor.hpp>

namespace ossia
{
void launch(const StateElement& e)
{
    if(e)
        eggs::variants::apply(StateExecutionVisitor{}, e);
}

std::ostream& print(std::ostream& out, const StateElement& e)
{
    if(e)
        eggs::variants::apply(StatePrintVisitor{out}, e);
    else
        out << "no state";
    return out;
}
}
