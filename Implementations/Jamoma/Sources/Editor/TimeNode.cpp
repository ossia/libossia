#include "Editor/JamomaTimeNode.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<TimeNode> TimeNode::create()
{
  return make_shared<JamomaTimeNode>();
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
{}

# pragma mark -
# pragma mark Accessors

TimeValue JamomaTimeNode::getDate() const
{
  return TimeValue();
}
  
TimeValue JamomaTimeNode::getSimultaneityMargin() const
{
  return TimeValue();
}
  
void JamomaTimeNode::setSimultaneityMargin(TimeValue)
{}

# pragma mark -
# pragma mark TimeEvents

JamomaTimeNode::iterator JamomaTimeNode::emplace(const_iterator pos,
                                                 std::shared_ptr<State> state,
                                                 std::shared_ptr<Expression> expression)
{
  return timeEvents().insert(pos, std::make_shared<JamomaTimeEvent>(shared_from_this(), state, expression));
}
