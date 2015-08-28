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
      
      if (e->isObservingExpression())
        noEventObserveExpression = false;
      
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