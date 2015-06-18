#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<Scenario> Scenario::create(shared_ptr<TimeConstraint> parentConstraint,
                                      shared_ptr<State> startState,
                                      shared_ptr<State> endState,
                                      shared_ptr<Clock> clock)
{
  return make_shared<JamomaScenario>(parentConstraint, startState, endState, clock);
}

JamomaScenario::JamomaScenario(shared_ptr<TimeConstraint> parentConstraint,
                               shared_ptr<State> startState,
                               shared_ptr<State> endState,
                               shared_ptr<Clock> clock) :
mParentConstraint(parentConstraint),
mStartState(startState),
mEndState(endState),
mClock(clock)
{
  // create the start and the end time nodes
  mTimeNodes.push_back(TimeNode::create());
  mTimeNodes.push_back(TimeNode::create());
}

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
{
  // reset all timenodes to waiting status
  for (auto & timenode : mTimeNodes)
    ; //! \todo timenode->setStatus(TimeNode::Status::WAITING);

  // sort timenodes in 2 lists depending of their timeconstraint position relative to the time offset
  TimeValue timeOffset = mClock->getOffset();
  
  Container<TimeNode> timenodesToSetHappened;
  Container<TimeNode> timenodesToRequestHappen;
  
  for (auto & timeconstraint : mTimeContraints)
  {
    auto startNode = timeconstraint->getStartEvent()->getTimeNode();
    auto endNode = timeconstraint->getEndEvent()->getTimeNode();
    
    TimeValue startNodeDate = startNode->getDate();
    TimeValue endNodeDate = endNode->getDate();
    
    if (startNodeDate < timeOffset && endNodeDate < timeOffset)
    {
      // if the start node is not already into the list of timenodes to request happened
      if (find(timenodesToRequestHappen.begin(),
               timenodesToRequestHappen.end(),
               startNode) == timenodesToRequestHappen.end())
        // if the start node is not already into the list of timenodes to set happened
        if (find(timenodesToSetHappened.begin(),
                 timenodesToSetHappened.end(),
                 startNode) == timenodesToSetHappened.end())
          // store the start node to set it happened
          timenodesToSetHappened.push_back(startNode);
    }
    else if (startNodeDate < timeOffset && endNodeDate > timeOffset)
    {
      // if the start node is not already into the list of timenodes to request happened
      if (find(timenodesToRequestHappen.begin(),
               timenodesToRequestHappen.end(),
               startNode) == timenodesToRequestHappen.end())
        // store the start node to request happened
        timenodesToRequestHappen.push_back(startNode);
      
      // remove the start node from the list of timenodes to set happened
      timenodesToSetHappened.erase(find(timenodesToSetHappened.begin(),
                                        timenodesToSetHappened.end(),
                                        startNode));
    }
  }
  
  for (auto & timenode : timenodesToSetHappened)
    ; //! \todo timenode->setStatus(TimeNode::Status::HAPPENED);
  
  for (auto & timenode : timenodesToRequestHappen)
    ; //! \todo timenode->happen();
}

# pragma mark -
# pragma mark Edition

void JamomaScenario::addConstraint(const shared_ptr<TimeConstraint> constraint)
{
  // store the constraint if it is not already stored
  if (find(mTimeContraints.begin(),
           mTimeContraints.end(),
           constraint) == mTimeContraints.end())
  {
    mTimeContraints.push_back(constraint);
  }
  
  // store constraint's start node if it is not already stored
  addTimeNode(constraint->getStartEvent()->getTimeNode());
  
  // store constraint's end node if it is not already stored
  addTimeNode(constraint->getEndEvent()->getTimeNode());
}

void JamomaScenario::removeConstraint(const shared_ptr<TimeConstraint> constraint)
{
  mTimeContraints.erase(find(mTimeContraints.begin(), mTimeContraints.end(), constraint));
}

void JamomaScenario::addTimeNode(const shared_ptr<TimeNode> timeNode)
{
  // store a time node if it is not already stored
  if (find(mTimeNodes.begin(),
           mTimeNodes.end(),
           timeNode) == mTimeNodes.end())
  {
    mTimeNodes.push_back(timeNode);
  }
}

void JamomaScenario::removeTimeNode(const shared_ptr<TimeNode> timeNode)
{
  mTimeNodes.erase(find(mTimeNodes.begin(), mTimeNodes.end(), timeNode));
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

const shared_ptr<TimeConstraint> & JamomaScenario::getParentTimeConstraint() const
{
  return mParentConstraint;
}

const shared_ptr<State> & JamomaScenario::getStartState() const
{
  return mStartState;
}

const shared_ptr<State> & JamomaScenario::getEndState() const
{
  return mEndState;
}

const shared_ptr<Clock> & JamomaScenario::getClock() const
{
  return mClock;
}
