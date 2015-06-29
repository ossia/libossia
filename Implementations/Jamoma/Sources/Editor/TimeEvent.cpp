#include "Editor/JamomaTimeEvent.h"

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(shared_ptr<TimeNode> aTimeNode,
                                 shared_ptr<Expression> anExpression) :
mTimeNode(aTimeNode),
mExpression(anExpression)
{
  mState = State::create();
}

JamomaTimeEvent::~JamomaTimeEvent()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeEvent::play(bool log, string name) const
{
  // propagate execution to next TimeConstraints
  for (auto& timeConstraint : nextTimeConstraints())
  {
    timeConstraint->play();
  }
}

# pragma mark -
# pragma mark Edition

void JamomaTimeEvent::addState(const std::shared_ptr<State> state)
{
  // store a state if it is not already stored
  if (find(mState->stateElements().begin(),
           mState->stateElements().end(),
           state) == mState->stateElements().end())
  {
    mState->stateElements().push_back(state);
  }
}

void JamomaTimeEvent::removeState(const std::shared_ptr<State> state)
{
  mState->stateElements().erase(find(mState->stateElements().begin(), mState->stateElements().end(), state));
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaTimeEvent::getTimeNode() const
{
  return mTimeNode;
}
  
const shared_ptr<State> & JamomaTimeEvent::getState() const
{
  return mState;
}

const shared_ptr<Expression> & JamomaTimeEvent::getExpression() const
{
  return mExpression;
}

TimeEvent::Status JamomaTimeEvent::getStatus() const
{
  return mStatus;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeEvent::setStatus(Status status)
{
  mStatus = status;
  
  //! \todo notify it changes
}