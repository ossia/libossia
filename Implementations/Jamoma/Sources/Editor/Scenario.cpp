#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<Scenario> Scenario::create(shared_ptr<TimeConstraint> parentConstraint)
{
  return make_shared<JamomaScenario>(parentConstraint);
}

JamomaScenario::JamomaScenario(shared_ptr<TimeConstraint> parentConstraint) :
mParentConstraint(parentConstraint)
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

void JamomaScenario::addConstraint(const shared_ptr<TimeConstraint> constraint)
{
  // store
  mTimeContraints.push_back(constraint);
  
  //! \todo store constraint's start and end node if they aren't already stored
  //mTimeNodes.push_back(constraint->getStartEvent()->getTimeNode());
  //mTimeNodes.push_back(constraint->getStartEvent()->getTimeNode());
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

void JamomaScenario::addTimeNode(const std::shared_ptr<TimeNode>)
{
  
}

void JamomaScenario::removeTimeNode(const std::shared_ptr<TimeNode>)
{
  
}

# pragma mark -
# pragma mark Accessors

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

const shared_ptr<TimeNode> & JamomaScenario::getEndNode() const
{
  return mTimeNodes[1];
}

const std::shared_ptr<TimeConstraint> & JamomaScenario::getParentTimeConstraint() const
{
  return mParentConstraint;
}

const std::shared_ptr<State> & JamomaScenario::getStartState() const
{
  
}

const std::shared_ptr<State> & JamomaScenario::getEndState() const
{
  
}