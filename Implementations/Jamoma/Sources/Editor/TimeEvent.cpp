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
mStatus(TimeEvent::Status::NONE),
mObserveExpression(false),
mResultCallback(std::bind(&JamomaTimeEvent::resultCallback, this, _1))
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

TimeEvent & JamomaTimeEvent::setExpression(const std::shared_ptr<Expression> expression)
{
  if (mExpression != nullptr)
    mExpression->removeCallback(mResultCallbackIndex);

  mExpression = expression;
  mObserveExpression = false;

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

  // NONE TimeEvent without previous TimeConstraints is PENDING
  // if each previous TimeConstraint ends or reaches their minimal duration
  // and it starts to observe its Expression if all previous TimeConstraints have reach their minimal duration
  else
  {
    // for each previous TimeConstraints
    bool setPending = false;
    bool observe = true;
    for (auto& timeConstraint : previousTimeConstraints())
    {
      shared_ptr<JamomaTimeConstraint> c = dynamic_pointer_cast<JamomaTimeConstraint>(timeConstraint);

      // when running
      if (c->getRunning())
      {
        // when all constraint durations are equals
        if (c->getDurationMin() == c->getDuration() &&
            c->getDurationMax() == c->getDuration())
        {
          // don't observe expression
          observe = false;

          // stay NONE status
          break;
        }
        // when the minimal duration is not reached
        else if (c->getDate() < c->getDurationMin())
        {
          // don't observe expression
          observe = false;

          // stay NONE status
          break;
        }
        // when the minimal duration is reached
        else if (c->getDate() >= c->getDurationMin() &&
                 c->getDate() < c->getDurationMax())
        {
          // observe expression if all other previous constraints allow it too
          observe &= true;

          // and access to PENDING status (see below)
        }
        else if (c->getDate() >= c->getDurationMax())
        {
          // don't observe expression
          observe = false;

          // and access to PENDING status (see below)
        }
      }

      setPending = true;
    }

    // access to PENDING status once all previous TimeConstraints allow it
    if (setPending)
    {
      setStatus(TimeEvent::Status::PENDING);
    }

    // observe the expression to observe all Destination value contained into it
    observeExpressionResult(observe);
  }
}

void JamomaTimeEvent::setStatus(Status status)
{
  mStatus = status;
  (mCallback)(mStatus);
}

bool JamomaTimeEvent::isObservingExpression()
{
  return mObserveExpression;
}

void JamomaTimeEvent::observeExpressionResult(bool observe)
{
  if (mExpression == nullptr)
    return;

  if (observe != mObserveExpression)
  {
    mObserveExpression = observe;

    if (mObserveExpression)
    {
      // start expression observation
      mResultCallbackIndex = mExpression->addCallback(mResultCallback);
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
