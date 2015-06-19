#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"

#include <iostream> //! \todo to remove. only for debug purpose

# pragma mark -
# pragma mark Life cycle

shared_ptr<Scenario> Scenario::create(shared_ptr<State> startState,
                                      shared_ptr<State> endState,
                                      shared_ptr<Clock> clock)
{
  return make_shared<JamomaScenario>(startState, endState, clock);
}

JamomaScenario::JamomaScenario(shared_ptr<State> startState,
                               shared_ptr<State> endState,
                               shared_ptr<Clock> clock) :
mStartState(startState),
mEndState(endState),
mClock(clock)
{
  // create the start and the end time nodes
  mTimeNodes.push_back(TimeNode::create());
  mTimeNodes.push_back(TimeNode::create());
  
  // pass callback to the clock
  Clock::ExecutionCallback callback = std::bind(&JamomaScenario::ClockCallback, this, _1, _2);
  mClock->setExecutionCallback(callback);
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
  mClock->go();
  mTimeNodes[0]->play();
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

# pragma mark -
# pragma mark Implementation specific

void JamomaScenario::ClockCallback(const TimeValue& position, const TimeValue& date)
{
  cout << double(position) << ", " << double(date) << "\n";
}

