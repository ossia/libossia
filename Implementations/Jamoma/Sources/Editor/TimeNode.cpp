#include "Editor/JamomaTimeNode.h"

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

JamomaTimeNode::JamomaTimeNode()
{}
  
JamomaTimeNode::JamomaTimeNode(const JamomaTimeNode * other)
{}

shared_ptr<TimeNode> JamomaTimeNode::clone() const
{
  return make_shared<JamomaTimeNode>(this);
}

JamomaTimeNode::~JamomaTimeNode()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeNode::play(bool log, string name) const
{
  for (auto & timeEvent : timeEvents())
    timeEvent->happen();
}

# pragma mark -
# pragma mark Accessors

TimeValue JamomaTimeNode::getDate() const
{
  if (timeEvents().empty())
    return Zero;
  
  else if (timeEvents()[0]->previousTimeConstraints().empty())
    return Zero;
    
  else
    return timeEvents()[0]->previousTimeConstraints()[0]->getDuration() + timeEvents()[0]->previousTimeConstraints()[0]->getStartEvent()->getTimeNode()->getDate();
}
  
TimeValue JamomaTimeNode::getSimultaneityMargin() const
{
  return mSimultaneityMargin;
}
  
void JamomaTimeNode::setSimultaneityMargin(const TimeValue& simultaneityMargin)
{
  mSimultaneityMargin = simultaneityMargin;
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
  Container<TimeEvent> pendingEvents;
  
  for (auto& timeEvent : timeEvents())
  {
    shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
    
    switch (timeEvent->getStatus())
    {
      // check if NONE TimeEvent could becomes PENDING
      case TimeEvent::Status::NONE:
      {
        // NONE TimeEvent without previous TimeConstraint is PENDING
        if (timeEvent->previousTimeConstraints().empty())
        {
          e->setStatus(TimeEvent::Status::PENDING);
          pendingEvents.push_back(timeEvent);
        }
        
        // NONE TimeEvent without previous TimeConstraints is PENDING
        // if each previous TimeConstraint ends or reaches their minimal duration
        // and it starts to observe its Expression if all previous TimeConstraints have reach their minimal duration
        else
        {
          // for each previous TimeConstraints
          bool setPending = false;
          bool observeExpression = true;
          for (auto& timeConstraint : timeEvent->previousTimeConstraints())
          {
            shared_ptr<JamomaTimeConstraint> c = dynamic_pointer_cast<JamomaTimeConstraint>(timeConstraint);
            
            // when running
            if (c->getRunning())
            {
              // when all constraint durations are equals
              if (c->getDurationMin() == c->getDuration() &&
                  c->getDurationMax() == c->getDuration())
              {
                // wait and avoid expression observation
                observeExpression = false;
                break;
              }
              // when the minimal duration is not reached
              else if (c->getDate() < c->getDurationMin())
              {
                // wait and avoid expression observation
                observeExpression = false;
                break;
              }
              // when the minimal duration is reached
              else if (c->getDate() >= c->getDurationMin() &&
                       c->getDate() < c->getDurationMax())
              {
                // observe expression if all other previous constraints allow it too
                observeExpression &= true;
              }
              else if (c->getDate() >= c->getDurationMax())
              {
                observeExpression = false;
              }
            }
            
            setPending = true;
          }
          
          if (setPending)
          {
            e->setStatus(TimeEvent::Status::PENDING);
            pendingEvents.push_back(timeEvent);
          }
          
          e->setObserveExpression(observeExpression);
        }
        
        break;
      }
      
      // PENDING TimeEvent is ready for evaluation
      case TimeEvent::Status::PENDING:
      {
        pendingEvents.push_back(timeEvent);
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
  
  // if all TimeEvents are PENDING
  if (pendingEvents.size() == timeEvents().size())
  {
    // dispatched them into TimeEvents to happen and TimeEvents to dispose
    Container<TimeEvent> eventsToHappen, eventsToDispose;
    bool noEventObserveExpression = true;
    for (auto& timeEvent : pendingEvents)
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      
      if (e->getObserveExpression())
        noEventObserveExpression = false;
      
      //! \note evaluation of expression on Destination implies the value is pulled (see in Value.cpp)
      //! and as everything is synchrone, is there a real interest to observe the value ?
      
      timeEvent->getExpression() != nullptr ? timeEvent->getExpression()->evaluate() ? eventsToHappen.push_back(timeEvent) : eventsToDispose.push_back(timeEvent) : eventsToHappen.push_back(timeEvent);
    }
    
    // if at least one TimeEvent happens
    // or if all TimeEvents needs to be disposed and none of them is observing its Expression
    if (eventsToHappen.size() > 0 ||
        (eventsToDispose.size() == timeEvents().size() && noEventObserveExpression))
    {
      for (auto& timeEvent : eventsToHappen)
      {
        timeEvent->happen();
        statusChangedEvents.push_back(timeEvent);
      }
      
      for (auto& timeEvent : eventsToDispose)
      {
        timeEvent->dispose();
        statusChangedEvents.push_back (timeEvent);
      }
    }
  }
}