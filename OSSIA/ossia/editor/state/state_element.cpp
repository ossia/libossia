#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>

namespace OSSIA
{
void launch(const StateElement& e)
{
    if(e)
        eggs::variants::apply(StateExecutionVisitor{}, e);
}
}
