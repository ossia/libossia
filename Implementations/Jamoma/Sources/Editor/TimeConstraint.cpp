#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<TimeConstraint> TimeConstraint::create(shared_ptr<TimeNode> startNode,
                                                  shared_ptr<TimeNode> endNode,
                                                  const TimeValue& nominal,
                                                  const TimeValue& min,
                                                  const TimeValue& max)
{
    auto constraint = make_shared<JamomaTimeConstraint>(startNode, endNode, nominal, min, max);
    
    // store the constraint into the start node as a next constraint
    if (find(startNode->nextTimeConstraints().begin(),
             startNode->nextTimeConstraints().end(),
             constraint) == startNode->nextTimeConstraints().end())
    {
        startNode->nextTimeConstraints().push_back(constraint);
    }
    
    // store the constraint into the end node as a previous constraint
    if (find(endNode->previousTimeConstraints().begin(),
             endNode->previousTimeConstraints().end(),
             constraint) == endNode->previousTimeConstraints().end())
    {
        endNode->previousTimeConstraints().push_back(constraint);
    }
    
    return constraint;
}

JamomaTimeConstraint::JamomaTimeConstraint(shared_ptr<TimeNode> startNode,
                                           shared_ptr<TimeNode> endNode,
                                           const TimeValue& nominal,
                                           const TimeValue& min,
                                           const TimeValue& max) :
mStartNode(startNode),
mEndNode(endNode),
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
{
  for (const auto & timeprocess : timeProcesses())
    timeprocess->play();
}

# pragma mark -
# pragma mark Accessors

const TimeValue & JamomaTimeConstraint::getDuration() const
{
  return mDuration;
}

const TimeValue & JamomaTimeConstraint::getDurationMin() const
{
  return mDurationMin;
}

const TimeValue & JamomaTimeConstraint::getDurationMax() const
{
  return mDurationMax;
}

const shared_ptr<TimeNode> & JamomaTimeConstraint::getStartNode() const
{
  return mStartNode;
}

const shared_ptr<TimeNode> & JamomaTimeConstraint::getEndNode() const
{
  return mEndNode;
}