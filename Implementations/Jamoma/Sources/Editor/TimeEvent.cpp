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
mExpression(anExpression),
mObserveExpression(false)
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

  observeExpressionResult(false);

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
  // NONE TimeEvent without previous TimeConstraint is PENDING
  if (previousTimeConstraints().empty())
  {
    setStatus(TimeEvent::Status::PENDING);
  }

  // NONE TimeEvent with previous TimeConstraints is PENDING
  // if each previous TimeConstraint ends or reaches their minimal duration
  // and it starts to observe its Expression if all previous TimeConstraints have reach their minimal duration
  else
  {
    TimeEvent::Status status = TimeEvent::Status::NONE;
    bool observe = true;
    
    // for each previous TimeConstraints
    for (auto& timeConstraint : previousTimeConstraints())
    {
      shared_ptr<JamomaTimeConstraint> c = dynamic_pointer_cast<JamomaTimeConstraint>(timeConstraint);
      
      // a previous TimeConstraint with a DISPOSED start event is ignored
      if (timeConstraint->getStartEvent()->getStatus() == TimeEvent::Status::DISPOSED)
        continue;
      
      // then any TimeConstraint with a none HAPPENED start event implies to quit
      if (timeConstraint->getStartEvent()->getStatus() != TimeEvent::Status::HAPPENED)
        return;
      
      // when the minimal duration is not reached
      if (c->getDate() < c->getDurationMin())
      {
        // don't observe expression
        observe = false;
        
        // force NONE status
        status = TimeEvent::Status::NONE;
        break;
      }
      // when the minimal duration is reached but not the maximal duration
      else if (c->getDate() >= c->getDurationMin() &&
               c->getDate() < c->getDurationMax())
      {
        // observe expression if all other previous constraints allow it too
        observe &= true;
        
        // access to PENDING status
        status = TimeEvent::Status::PENDING;
      }
      // when the maximal duration is reached
      else if (c->getDate() >= c->getDurationMax())
      {
        // don't observe expression
        observe = false;
        
        // access to PENDING status
        status = TimeEvent::Status::PENDING;
      }
    }

    // access to PENDING status once all previous TimeConstraints allow it
    setStatus(status);

    // observe the expression to observe all Destination value contained into it
    observeExpressionResult(observe);
  }
}

void JamomaTimeEvent::setStatus(Status status)
{
  mStatus = status;
  if (mCallback)
      (mCallback)(mStatus);
}

bool JamomaTimeEvent::isObservingExpression()
{
  return mObserveExpression;
}

void JamomaTimeEvent::observeExpressionResult(bool observe)
{
  if (!mExpression || *mExpression == *ExpressionTrue || *mExpression == *ExpressionFalse)
    return;

  if (observe != mObserveExpression)
  {
    mObserveExpression = observe;

    if (mObserveExpression)
    {
      // start expression observation
      mResultCallbackIndex = mExpression->addCallback(std::bind(&JamomaTimeEvent::resultCallback, this, _1));
    }
    else
    {
      // stop expression observation
      mExpression->removeCallback(mResultCallbackIndex);
    }
  }
}

void JamomaTimeEvent::resultCallback(bool result)
{
  //! \note the result of the expression is not exploited here.
  //! \note the observation of the expression is to observe all Destination value contained into it.
}
