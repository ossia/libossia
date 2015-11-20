#include "Editor/JamomaTimeNode.h"

#include <assert.h>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<TimeNode> TimeNode::create()
  {
    return make_shared<JamomaTimeNode>();
  }
}

JamomaTimeNode::JamomaTimeNode() :
mExpression(ExpressionTrue)
{}

JamomaTimeNode::JamomaTimeNode(const JamomaTimeNode * other)
{}

shared_ptr<TimeNode> JamomaTimeNode::clone() const
{
  return make_shared<JamomaTimeNode>(this);
}

JamomaTimeNode::~JamomaTimeNode()
{}

TimeNode::~TimeNode()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeNode::setup(const TimeValue& date)
{
  {
    TimeEvent::Status status = getDate() <= date ? getDate() == date ? TimeEvent::Status::PENDING : TimeEvent::Status::HAPPENED : TimeEvent::Status::NONE;

    //! \note maybe we should initialized TimeEvents with an Expression returning false to DISPOSED status ?

    for (auto& timeEvent : timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      e->setStatus(status);
    }
  }
}

bool JamomaTimeNode::trigger()
{
  // prepare to remember which event changed its status
  // because it is needed in JamomaTimeNode::process
  mStatusChangedEvents.clear();
  
  // if all TimeEvents are not PENDING
  if (mPendingEvents.size() != timeEvents().size())
  {
    // stop expression observation because the TimeNode is not ready to be processed
    observeExpressionResult(false);
    
    // the triggering failed
    return false;
  }
  
  // dispatched them into TimeEvents to happen and TimeEvents to dispose
  Container<TimeEvent> eventsToHappen, eventsToDispose;

  for (auto& timeEvent : mPendingEvents)
  {
    shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
    
    // update any Destination value into the expression
    timeEvent->getExpression()->update();
    
    if (timeEvent->getExpression()->evaluate())
      eventsToHappen.push_back(timeEvent);
    else
      eventsToDispose.push_back(timeEvent);
  }
  
  // if at least one TimeEvent happens
  if (eventsToHappen.size() > 0)
  {
    for (auto& timeEvent : eventsToHappen)
    {
      timeEvent->happen();
      mStatusChangedEvents.push_back(timeEvent);
    }
    
    for (auto& timeEvent : eventsToDispose)
    {
      timeEvent->dispose();
      mStatusChangedEvents.push_back (timeEvent);
    }
    
    // stop expression observation now the TimeNode has been processed
    observeExpressionResult(false);
    
    // the triggering succeded
    return true;
  }
  
  // the triggering failed
  return false;
}

# pragma mark -
# pragma mark Accessors

TimeValue JamomaTimeNode::getDate() const
{
  // compute the date from each first previous time constraint
  // ignoring zero duration time constraint
  if (!timeEvents().empty())
  {
    for (auto & timeEvent : timeEvents())
    {
      if (!timeEvent->previousTimeConstraints().empty())
      {
        if (timeEvent->previousTimeConstraints()[0]->getDuration() > Zero)
          return timeEvent->previousTimeConstraints()[0]->getDuration() + timeEvent->previousTimeConstraints()[0]->getStartEvent()->getTimeNode()->getDate();
      }
    }
  }

  return Zero;
}

const std::shared_ptr<Expression> & JamomaTimeNode::getExpression() const
{
  return mExpression;
}

TimeNode & JamomaTimeNode::setExpression(const std::shared_ptr<Expression> expression)
{
  assert(expression != nullptr);
  mExpression = expression;
  return *this;
}

TimeValue JamomaTimeNode::getSimultaneityMargin() const
{
  return mSimultaneityMargin;
}

TimeNode & JamomaTimeNode::setSimultaneityMargin(const TimeValue& simultaneityMargin)
{
  mSimultaneityMargin = simultaneityMargin;
  return *this;
}

# pragma mark -
# pragma mark #TimeEvents

JamomaTimeNode::iterator JamomaTimeNode::emplace(const_iterator pos,
                                                 TimeEvent::ExecutionCallback callback,
                                                 shared_ptr<Expression> expression)
{
  return timeEvents().insert(pos, make_shared<JamomaTimeEvent>(callback, shared_from_this(), expression));
}

void JamomaTimeNode::process(Container<TimeEvent>& statusChangedEvents)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in JamomaTimeNode::trigger
  mPendingEvents.clear();

  for (auto& timeEvent : timeEvents())
  {
    switch (timeEvent->getStatus())
    {
      // check if NONE TimeEvent is ready to become PENDING
      case TimeEvent::Status::NONE:
      {
        shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
        e->process();

        // don't break if the TimeEvent became PENDING
        if (timeEvent->getStatus() == TimeEvent::Status::NONE)
          break;
      }

      // PENDING TimeEvent is ready for evaluation
      case TimeEvent::Status::PENDING:
      {
        mPendingEvents.push_back(timeEvent);
        break;
      }

      // HAPPENED TimeEvent propagates recursively the execution to the end of each next TimeConstraints
      case TimeEvent::Status::HAPPENED:
      {
        for (auto& timeConstraint : timeEvent->nextTimeConstraints())
        {
          shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(timeConstraint->getEndEvent()->getTimeNode());
          n->process(statusChangedEvents);
        }

        break;
      }

      // DISPOSED TimeEvent stops the propagation of the execution
      case TimeEvent::Status::DISPOSED:
      {
        break;
      }
    }
  }
  
  // if all TimeEvents are not PENDING
  if (mPendingEvents.size() != timeEvents().size())
    return;
  
  // false expression mute TimeNode triggering
  if (*mExpression == *ExpressionFalse)
    return;
  
  // observe and evaluate TimeNode's expression before to trig
  if (*mExpression != *ExpressionTrue)
  {
    // update any Destination value into the expression once
    if (!isObservingExpression())
      mExpression->update();
    
    // then start observation
    observeExpressionResult(true);
    
    if (!mExpression->evaluate())
      return;
  }

  // trigger the time node
  if (trigger())
  {
    // gather events which have changed their status
    for (auto& timeEvent : mStatusChangedEvents)
      statusChangedEvents.push_back(timeEvent);
  }
}

bool JamomaTimeNode::isObservingExpression()
{
  return mObserveExpression;
}

void JamomaTimeNode::observeExpressionResult(bool observe)
{
  if (!mExpression || *mExpression == *ExpressionTrue || *mExpression == *ExpressionFalse)
    return;

  if (observe != mObserveExpression)
  {
    bool wasObserving = mObserveExpression;
    mObserveExpression = observe;

    if (mObserveExpression)
    {
      // pull value
      
      // start expression observation
      mResultCallbackIndex = mExpression->addCallback(std::bind(&JamomaTimeNode::resultCallback, this, _1));
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
  //! \note the observation of the expression is to observe all Destination value contained into it.
}
