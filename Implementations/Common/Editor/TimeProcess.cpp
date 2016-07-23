#include "Editor/TimeProcess_impl.h"

using namespace std;

namespace OSSIA
{
# pragma mark -
# pragma mark Life cycle

TimeProcess::~TimeProcess()
{}

# pragma mark -
# pragma mark Implementation specific

void flattenAndFilter(State& state, const std::shared_ptr<StateElement>& element)
{
    auto element_ptr = element.get();
    if (!element_ptr)
        return;

    switch (element_ptr->getType())
    {
        case StateElement::Type::MESSAGE :
        {
            auto messageToAppend = dynamic_cast<const Message*>(element_ptr);
            if(!messageToAppend)
                return;

            // find message with the same address to replace it
            bool found = false;
            for(std::shared_ptr<StateElement>& elt : state.stateElements())
            {
              auto ptr = elt.get();

              auto messageToCheck = dynamic_cast<const Message*>(ptr);
              if(!messageToCheck)
                  return;
              // replace if addresses are the same
              if (messageToCheck->getAddress() == messageToAppend->getAddress())
              {
                  elt = element;
                  found = true;
                  break;
              }
            }

            // if not found append it
            if (!found)
                state.stateElements().push_back(element);

            break;
        }
        case StateElement::Type::STATE :
        {
            auto stateToFlatAndFilter = dynamic_cast<const State*>(element_ptr);
            if(!stateToFlatAndFilter)
                return;

            for (const auto& e : stateToFlatAndFilter->stateElements())
            {
                flattenAndFilter(state, e);
            }
            break;
        }

        default:
        {
            state.stateElements().push_back(element);
            break;
        }
    }
}
}
