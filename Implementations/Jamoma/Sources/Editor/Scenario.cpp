#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<Scenario> Scenario::create()
{
  return make_shared<JamomaScenario>();
}

JamomaScenario::JamomaScenario()
{}

JamomaScenario::JamomaScenario(const JamomaScenario * other)
{}

JamomaScenario::~JamomaScenario()
{}

shared_ptr<Scenario> JamomaScenario::clone() const
{
  return make_shared<JamomaScenario>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaScenario::play(bool log, string name) const
{}

# pragma mark -
# pragma mark Edition

void JamomaScenario::addConstraint(const shared_ptr<TimeConstraint> constraint, const shared_ptr<TimeNode> startNode)
{}

void JamomaScenario::addConstraint(const shared_ptr<TimeConstraint> constraint, const shared_ptr<TimeNode> startNode, const shared_ptr<TimeNode> endNode)
{
  // store
  mTimeContraints.push_back(constraint);
  mTimeNodes.push_back(startNode);
  mTimeNodes.push_back(endNode);
}

void JamomaScenario::removeConstraint(const shared_ptr<TimeConstraint> constraint)
{
  Container<TimeConstraint>::iterator it;
  
  for (it = mTimeContraints.begin(); it != mTimeContraints.end(); it++)
  {
    if (*it == constraint)
    {
      mTimeContraints.erase(it);
      break;
    }
  }
}

# pragma mark -
# pragma mark Accessors

TimeValue JamomaScenario::getLength() const
{

}

void JamomaScenario::setLength(TimeValue)
{}

bool JamomaScenario::isKiller() const
{
  return mIsKiller;
}

void JamomaScenario::setKiller(bool isKiller)
{
  mIsKiller = isKiller;
}

const shared_ptr<TimeNode> & JamomaScenario::getStartNode() const
{
  return mTimeNodes[0];
}

void JamomaScenario::setStartNode(shared_ptr<TimeNode> startNode)
{
  mTimeNodes[0] = startNode;
}

const shared_ptr<TimeNode> & JamomaScenario::getEndNode() const
{
  return mTimeNodes[1];
}

void JamomaScenario::setEndNode(shared_ptr<TimeNode> endNode)
{
  mTimeNodes[1] = endNode;
}