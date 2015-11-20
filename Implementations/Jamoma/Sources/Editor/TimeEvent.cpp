#include "Editor/JamomaTimeEvent.h"

#include <assert.h>
#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(TimeEvent::ExecutionCallback callback,
                                 shared_ptr<TimeNode> aTimeNode,
                                 shared_ptr<Expression> anExpression) :
mCallback(callback),
mTimeNode(aTimeNode),
mStatus(TimeEvent::Status::NONE),
mExpression(anExpression)
{
  mState = State::create();
}

JamomaTimeEvent::~JamomaTimeEvent()
{}

void JamomaTimeEvent::setCallback(TimeEvent::ExecutionCallback callback)
{
    mCallback = callback;
}

TimeEvent::~TimeEvent()
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

  if (mCallback)
      (mCallback)(mStatus);
}

void JamomaTimeEvent::dispose()
{
  if (mStatus == TimeEvent::Status::HAPPENED)
    throw runtime_error("HAPPENED event cannot be disposed");
  
  mStatus = TimeEvent::Status::DISPOSED;
  
  // dispose next TimeConstraints end event if everything is disposed before
  for (auto& nextTimeConstraint : nextTimeConstraints())
  {
    bool dispose = true;
    
    for (auto& previousTimeConstraint : nextTimeConstraint->getEndEvent()->previousTimeConstraints())
    {
      if (previousTimeConstraint->getStartEvent()->getStatus() != TimeEvent::Status::DISPOSED)
      {
        dispose = false;
        break;
      }
    }
    
    if (dispose)
      nextTimeConstraint->getEndEvent()->dispose();
  }

  if (mCallback)
      (mCallback)(mStatus);
}

# pragma mark -
# pragma mark Edition

void JamomaTimeEvent::addState(const std::shared_ptr<State> state)
{
  // store a state if it is not already stored
  auto& se = mState->stateElements();
  if (std::find(se.begin(), se.end(), state) == se.end())
  {
    se.push_back(state);
  }
}

void JamomaTimeEvent::removeState(const std::shared_ptr<State> state)
{
  auto& se = mState->stateElements();
  auto it = std::find(se.begin(), se.end(), state);
  if (it != se.end())
  {
      se.erase(it);
  }
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

TimeEvent & JamomaTimeEvent::setExpression(const std::shared_ptr<Expression> expression)
{
  assert(expression != nullptr);

  mExpression = expression;

  return *this;
}

TimeEvent::Status JamomaTimeEvent::getStatus() const
{
  return mStatus;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeEvent::process()
{
  // NONE TimeEvent without previous TimeConstraint becomes PENDING
  if (previousTimeConstraints().empty())
  {
    setStatus(TimeEvent::Status::PENDING);
  }

  // NONE TimeEvent with previous TimeConstraints becomes PENDING
  // if each previous TimeConstraint reaches its minimal duration
  else
  {
    for (auto& timeConstraint : previousTimeConstraints())
    {
      shared_ptr<JamomaTimeConstraint> c = dynamic_pointer_cast<JamomaTimeConstraint>(timeConstraint);
      
      // previous TimeConstraints with a DISPOSED start event are ignored
      if (timeConstraint->getStartEvent()->getStatus() == TimeEvent::Status::DISPOSED)
        continue;
      
      // previous TimeConstraint with a none HAPPENED start event force to quit
      if (timeConstraint->getStartEvent()->getStatus() != TimeEvent::Status::HAPPENED)
        return;

      // previous TimeConstraint which doesn't reached its minimal duration force to quit
      if (c->getDate() < c->getDurationMin())
        return;
    }

    // access to PENDING status once all previous TimeConstraints allow it
    setStatus(TimeEvent::Status::PENDING);
  }
}

void JamomaTimeEvent::setStatus(Status status)
{
  mStatus = status;
  if (mCallback)
      (mCallback)(mStatus);
}