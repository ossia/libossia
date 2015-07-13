#include "Editor/JamomaTimeNode.h"

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
  
JamomaTimeNode::~JamomaTimeNode()
{}
  
shared_ptr<TimeNode> JamomaTimeNode::clone() const
{
  return make_shared<JamomaTimeNode>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaTimeNode::play(bool log, string name) const
{
  for (auto & timeEvent : timeEvents())
    timeEvent->play();
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