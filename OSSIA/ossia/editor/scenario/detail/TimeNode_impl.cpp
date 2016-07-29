#include "TimeNode_impl.hpp"

namespace impl
{

JamomaTimeNode::JamomaTimeNode(time_node::execution_callback callback) :
mCallback(callback),
mExpression(ExpressionTrue())
{}

JamomaTimeNode::~JamomaTimeNode()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeNode::setCallback(time_node::execution_callback callback)
{
  mCallback = callback;
}

bool JamomaTimeNode::trigger()
{
  // if all TimeEvents are not PENDING
  if (mPendingEvents.size() != timeEvents().size())
  {
    // stop expression observation because the TimeNode is not ready to be processed
    observeExpressionResult(false);

    // the triggering failed
    return false;
  }

  // now TimeEvents will happen or be disposed
  for (auto& timeEvent : mPendingEvents)
  {
    auto& ev = *timeEvent;
    auto& expr = *ev.getExpression();
    // update any Destination value into the expression
    expr.update();

    if (expr.evaluate())
      ev.happen();
    else
      ev.dispose();
  }

  // stop expression observation now the TimeNode has been processed
  observeExpressionResult(false);

  // notify observers
  if (mCallback)
    (mCallback)();

  // the triggering succeded
  return true;
}

# pragma mark -
# pragma mark Accessors

time_value JamomaTimeNode::getDate() const
{
  // compute the date from each first previous time constraint
  // ignoring zero duration time constraint
  if (!timeEvents().empty())
  {
    for (auto & timeEvent : timeEvents())
    {
      if (!timeEvent->previousTimeConstraints().empty())
      {
        if (timeEvent->previousTimeConstraints()[0]->getDurationNominal() > Zero)
          return timeEvent->previousTimeConstraints()[0]->getDurationNominal() + timeEvent->previousTimeConstraints()[0]->getStartEvent()->getTimeNode()->getDate();
      }
    }
  }

  return Zero;
}

const std::shared_ptr<expression_base> & JamomaTimeNode::getExpression() const
{
  return mExpression;
}

time_node & JamomaTimeNode::setExpression(const std::shared_ptr<expression_base> exp)
{
  assert(exp != nullptr);
  mExpression = exp;
  return *this;
}

time_value JamomaTimeNode::getSimultaneityMargin() const
{
  return mSimultaneityMargin;
}

time_node & JamomaTimeNode::setSimultaneityMargin(time_value simultaneityMargin)
{
  mSimultaneityMargin = simultaneityMargin;
  return *this;
}

# pragma mark -
# pragma mark TimeEvents

JamomaTimeNode::iterator JamomaTimeNode::emplace(const_iterator pos,
                                                 time_event::ExecutionCallback callback,
                                                 std::shared_ptr<expression_base> exp)
{
  return timeEvents().insert(pos, std::make_shared<JamomaTimeEvent>(callback, shared_from_this(), exp));
}

void JamomaTimeNode::process(ptr_container<time_event>& statusChangedEvents)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in JamomaTimeNode::trigger
  mPendingEvents.clear();

  bool maximalDurationReached = false;

  for (auto& timeEvent : timeEvents())
  {
    std::shared_ptr<JamomaTimeEvent> e = std::dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);

    switch (timeEvent->getStatus())
    {
      // check if NONE TimeEvent is ready to become PENDING
      case time_event::Status::NONE:
      {
        bool minimalDurationReached = true;

        for (auto& timeConstraint : timeEvent->previousTimeConstraints())
        {
          // previous TimeConstraints with a DISPOSED start event are ignored
          if (timeConstraint->getStartEvent()->getStatus() == time_event::Status::DISPOSED)
            continue;

          // previous TimeConstraint with a none HAPPENED start event
          // can't have reached its minimal duration
          if (timeConstraint->getStartEvent()->getStatus() != time_event::Status::HAPPENED)
          {
            minimalDurationReached = false;
            break;
          }

          // previous TimeConstraint which doesn't reached its minimal duration force to quit
          if (timeConstraint->getDate() < timeConstraint->getDurationMin())
          {
            minimalDurationReached = false;
            break;
          }
        }

        // access to PENDING status once all previous TimeConstraints allow it
        if (minimalDurationReached)
          e->setStatus(time_event::Status::PENDING);
        else
          break;
      }

      // PENDING TimeEvent is ready for evaluation
      case time_event::Status::PENDING:
      {
        mPendingEvents.push_back(timeEvent);

        for (auto& timeConstraint : timeEvent->previousTimeConstraints())
        {
          if (timeConstraint->getDate() >= timeConstraint->getDurationMax())
            maximalDurationReached = true;
        }

        break;
      }

      // HAPPENED TimeEvent propagates recursively the execution to the end of each next TimeConstraints
      case time_event::Status::HAPPENED:
      {
        for (auto& timeConstraint : timeEvent->nextTimeConstraints())
        {
          std::shared_ptr<JamomaTimeNode> n = std::dynamic_pointer_cast<JamomaTimeNode>(timeConstraint->getEndEvent()->getTimeNode());
          n->process(statusChangedEvents);
        }

        break;
      }

      // DISPOSED TimeEvent stops the propagation of the execution
      case time_event::Status::DISPOSED:
      {
        break;
      }
    }
  }

  // if all TimeEvents are not PENDING
  if (mPendingEvents.size() != timeEvents().size())
    return;

  // false expression mute TimeNode triggering
  if (*mExpression == *ExpressionFalse())
    return;

  //! \todo force triggering if at leat one TimeEvent has
  // at least one TimeConstraint over its maximal duration

  // update the expression one time
  // then observe and evaluate TimeNode's expression before to trig
  // only if no maximal duration have been reached
  if (*mExpression != *ExpressionTrue() &&
      !maximalDurationReached)
  {
    if (!isObservingExpression())
      mExpression->update();

    observeExpressionResult(true);

    if (!mExpression->evaluate())
      return;
  }

  // trigger the time node
  if (trigger())
  {
    // former PENDING TimeEvents are now HAPPENED or DISPOSED
    for (auto& timeEvent : mPendingEvents)
      statusChangedEvents.push_back(timeEvent);
  }
}

bool JamomaTimeNode::isObservingExpression()
{
  return mObserveExpression;
}

void JamomaTimeNode::observeExpressionResult(bool observe)
{
  if (!mExpression || *mExpression == *ExpressionTrue() || *mExpression == *ExpressionFalse())
    return;

  if (observe != mObserveExpression)
  {
    bool wasObserving = mObserveExpression;
    mObserveExpression = observe;

    if (mObserveExpression)
    {
      // pull value

      // start expression observation
      mResultCallbackIndex = mExpression->addCallback([&] (bool result) { resultCallback(result); });
      mCallbackSet = true;
    }
    else
    {
      // stop expression observation
      if(wasObserving && mCallbackSet)
      {
        mExpression->removeCallback(mResultCallbackIndex);
        mCallbackSet = false;
      }
    }
  }
}

void JamomaTimeNode::resultCallback(bool result)
{
  //! \note the result of the expression is not exploited here.
  //! \note the observation of the expression allows to observe all Destination value contained into it.
}

}
