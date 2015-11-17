#include "Editor/JamomaState.h"

namespace OSSIA
{
  shared_ptr<State> State::create()
  {
    return make_shared<JamomaState>();
  }
}

# pragma mark -
# pragma mark Life cycle

JamomaState::JamomaState()
{}

JamomaState::JamomaState(const JamomaState * other)
{}

shared_ptr<State> JamomaState::clone() const
{
  return make_shared<JamomaState>(this);
}

JamomaState::~JamomaState()
{}

State::~State()
{}

# pragma mark -
# pragma mark Execution

void JamomaState::launch() const
{
  for (const auto& elt : stateElements())
    elt->launch();
}
