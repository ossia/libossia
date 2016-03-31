#include "Editor/JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

JamomaTimeProcess::JamomaTimeProcess() :
mLastDate(Infinite)
{
  ;
}

TimeProcess::~TimeProcess()
{}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeProcess::flattenAndFilter(shared_ptr<State> state, const shared_ptr<StateElement>& element)
{
    if (!element)
        return;
    
    switch (element->getType())
    {
        case StateElement::Type::MESSAGE :
        {
            shared_ptr<Message> messageToAppend = dynamic_pointer_cast<Message>(element);
            if(!messageToAppend)
                return;
            
            // find message with the same address to replace it
            bool found = false;
            for (auto it = state->stateElements().begin();
                 it != state->stateElements().end();
                 it++)
            {
                shared_ptr<Message> messageToCheck = dynamic_pointer_cast<Message>(*it);
                if(!messageToCheck)
                    return;
                // replace if addresses are the same
                if (messageToCheck->getAddress() == messageToAppend->getAddress())
                {
                    *it = element;
                    found = true;
                    break;
                }
            }
            
            // if not found append it
            if (!found)
                state->stateElements().push_back(element);
            
            break;
        }
        case StateElement::Type::STATE :
        {
            shared_ptr<State> stateToFlatAndFilter = dynamic_pointer_cast<State>(element);
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
            state->stateElements().push_back(element);
            break;
        }
    }
}
