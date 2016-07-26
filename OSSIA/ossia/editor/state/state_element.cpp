#include "StateElement.hpp"

namespace OSSIA
{
void launch(const StateElement& e)
{
    if(e)
        eggs::variants::apply(StateExecutionVisitor{}, e);
}
}
