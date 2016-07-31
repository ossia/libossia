#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_print_visitor.hpp>

namespace ossia
{
void launch(const state_element& e)
{
    if(e)
        eggs::variants::apply(state_execution_visitor{}, e);
}

std::ostream& print(std::ostream& out, const state_element& e)
{
    if(e)
        eggs::variants::apply(state_print_visitor{out, {}}, e);
    else
        out << "no state";
    return out;
}
}
