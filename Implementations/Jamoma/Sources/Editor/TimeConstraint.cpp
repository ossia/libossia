#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<TimeConstraint> TimeConstraint::create(TimeValue nominal, TimeValue min /*= nominal*/, TimeValue max /*= nominal*/)
{
  return make_shared<JamomaTimeConstraint>(nominal, min, max);
}

JamomaTimeConstraint::JamomaTimeConstraint(TimeValue nominal, TimeValue min /*= nominal*/, TimeValue max /*= nominal*/) :
mDuration(nominal),
mDurationMin(min),
mDurationMax(max)
{}

JamomaTimeConstraint::JamomaTimeConstraint(const JamomaTimeConstraint * other)
{}

JamomaTimeConstraint::~JamomaTimeConstraint()
{}

shared_ptr<TimeConstraint> JamomaTimeConstraint::clone() const
{
  return make_shared<JamomaTimeConstraint>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaTimeConstraint::play(bool log, string name) const
{}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaTimeConstraint::getStartNode() const
{
  return mStartNode;
}

void JamomaTimeConstraint::setStartNode(std::shared_ptr<TimeNode> startNode)
{
  mStartNode = startNode;
}

const shared_ptr<TimeNode> & JamomaTimeConstraint::getEndNode() const
{
  return mEndNode;
}

void JamomaTimeConstraint::setEndNode(std::shared_ptr<TimeNode> endNode)
{
  mEndNode = endNode;
}

const shared_ptr<State> & JamomaTimeConstraint::getStartState() const
{
  return mStartState;
}

void JamomaTimeConstraint::setStartState(shared_ptr<State> startState)
{
  mStartState = startState;
}

const shared_ptr<State> & JamomaTimeConstraint::getEndState() const
{
  return mEndState;
}

void JamomaTimeConstraint::setEndState(shared_ptr<State> endState)
{
  mEndState = endState;
}