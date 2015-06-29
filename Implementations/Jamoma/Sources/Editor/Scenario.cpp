#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"
#include "Editor/JamomaTimeEvent.h"

#include <iostream> //! \todo to remove. only here for debug purpose
#include <algorithm>

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
  // create the start and the end TimeNodes
  mTimeNodes.push_back(TimeNode::create());
  mTimeNodes.push_back(TimeNode::create());

  // pass callback to the Clock
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
  // reset TimeEvent's status
  for (const auto& timeNode : mTimeNodes)
  {
    TimeEvent::Status status = timeNode->getDate() < mClock->getOffset() ?
    TimeEvent::Status::HAPPENED : TimeEvent::Status::WAITING;
    
    for (auto& timeEvent : timeNode->timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      e->mStatus = status;
    }
  }
  
  // activate TimeProcess's clock
  for (const auto& timeConstraint : mTimeContraints)
  {
    TimeValue offset = Zero;
    TimeEvent::Status startStatus = timeConstraint->getStartEvent()->getStatus();
    TimeEvent::Status endStatus = timeConstraint->getEndEvent()->getStatus();
    
    if (startStatus == TimeEvent::Status::HAPPENED &&
        endStatus == TimeEvent::Status::WAITING)
    {
      offset = mClock->getOffset() - timeConstraint->getStartEvent()->getTimeNode()->getDate();
    }
    
    for (auto& timeProcess : timeConstraint->timeProcesses())
    {
      timeProcess->getClock()->setOffset(offset);
    }
    
    if (offset != Zero)
      timeConstraint->getStartEvent()->getTimeNode()->play();
  }
  
  mClock->go();
}

shared_ptr<State> JamomaScenario::state(const TimeValue& position, const TimeValue& date) const
{
  // on start
  if (position == Zero)
  {
    return mTimeNodes[0]->state();
  }
  // on end
  else if (position == One)
  {
    return mTimeNodes[1]->state();
  }

  //! \todo the algorithm !
  return State::create();
}

# pragma mark -
# pragma mark Edition

void JamomaScenario::addConstraint(const shared_ptr<TimeConstraint> constraint)
{
  // store the TimeConstraint if it is not already stored
  if (std::find(mTimeContraints.begin(),
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
  // store a TimeNode if it is not already stored
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
  cout << "Scenario : " << double(position) << ", " << double(date) << "\n";
  
  // on start
  if (position == Zero)
  {
    cout << "Scenario starts\n";
    mTimeNodes[0]->play();
  }
  // on end
  else if (position == One)
  {
    cout << "Scenario ends\n";
    mTimeNodes[1]->play();
  }
  
  // process each running TimeProcess in external drive mode
  for (const auto& timeConstraint : mTimeContraints)
    for (auto& timeProcess : timeConstraint->timeProcesses())
      if (timeProcess->getClock()->getRunning() &&
          timeProcess->getClock()->getExternal())
        timeProcess->getClock()->tick();
  
  state(position, date)->launch();
}