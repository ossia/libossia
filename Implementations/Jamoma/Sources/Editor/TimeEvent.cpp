#include "Editor/JamomaTimeEvent.h"

#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(TimeEvent::ExecutionCallback callback,
                                 shared_ptr<TimeNode> aTimeNode,
                                 shared_ptr<Expression> anExpression) :
mCallback(callback),
mTimeNode(aTimeNode),
mExpression(anExpression),
mStatus(TimeEvent::Status::NONE)
{
  mState = State::create();
}

JamomaTimeEvent::~JamomaTimeEvent()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeEvent::happen()
{
  if (mStatus != TimeEvent::Status::PENDING)
    throw runtime_error("only PENDING event can happens");
  
  mStatus = TimeEvent::Status::HAPPENED;
  
  // stop previous TimeConstraints
  for (auto& timeConstraint : previousTimeConstraints())
  {
    timeConstraint->stop();
  }
  
  // start next TimeConstraints
  for (auto& timeConstraint : nextTimeConstraints())
  {
    timeConstraint->start();
  }
  
  (mCallback)(mStatus);
}

void JamomaTimeEvent::dispose()
{
  if (mStatus != TimeEvent::Status::PENDING)
    throw runtime_error("only PENDING event can be disposed");
  
  mStatus = TimeEvent::Status::DISPOSED;
  
  (mCallback)(mStatus);
}

# pragma mark -
# pragma mark Edition

void JamomaTimeEvent::addState(const std::shared_ptr<State> state)
{
  // store a state if it is not already stored
  if (std::find(mState->stateElements().begin(),
           mState->stateElements().end(),
           state) == mState->stateElements().end())
  {
    mState->stateElements().push_back(state);
  }
}

void JamomaTimeEvent::removeState(const std::shared_ptr<State> state)
{
  mState->stateElements().erase(std::find(mState->stateElements().begin(), mState->stateElements().end(), state));
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

void JamomaTimeEvent::process()
{
  switch (mStatus)
  {
    case TimeEvent::Status::NONE:
    {
      // nothing to do
      
      //! \debug
      cout << "TimeEvent::process() : NONE => do nothing" << endl;
      
      break;
    }
    case TimeEvent::Status::PENDING:
    {
      // if the event has an expression: evaluate its expression to make it happen or to dispose it
      // else make the event happen
      
      //! \debug
      cout << "TimeEvent::process() : PENDING => evaluate expression if exist else happen" << endl;
      
      mExpression != nullptr ? mExpression->evaluate() ? happen() : dispose() : happen();
      break;
    }
    case TimeEvent::Status::HAPPENED:
    {
      // process the next TimeConstraints
      
      //! \debug
      //cout << "TimeEvent::process() : HAPPENED => process next constraints" << endl;
      
      for (auto& timeConstraint : nextTimeConstraints())
      {
        shared_ptr<JamomaTimeConstraint> c = dynamic_pointer_cast<JamomaTimeConstraint>(timeConstraint);
        c->process();
      }
      break;
    }
    case TimeEvent::Status::DISPOSED:
    {
      // nothing to do
      
      //! \debug
      cout << "TimeEvent::process() : DISPOSED => do nothing" << endl;
      
      break;
    }
  }
}

void JamomaTimeEvent::setStatus(Status status)
{
  mStatus = status;
  (mCallback)(mStatus);
}
