#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"
#include "Editor/JamomaTimeEvent.h"

#include <iostream> //! \todo to remove. only here for debug purpose
#include <algorithm>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Scenario> Scenario::create(TimeProcess::ExecutionCallback callback,
                                        shared_ptr<State> startState,
                                        shared_ptr<State> endState)
  {
    return make_shared<JamomaScenario>(callback, startState, endState);
  }
}

JamomaScenario::JamomaScenario(TimeProcess::ExecutionCallback callback,
                               shared_ptr<State> startState,
                               shared_ptr<State> endState) :
JamomaTimeProcess(callback, startState, endState),
mKiller(false)
{
  // create the start and the end TimeNodes
  mTimeNodes.push_back(TimeNode::create());
  mTimeNodes.push_back(TimeNode::create());

  // pass callback to the Clock
  Clock::ExecutionCallback clockCallback = std::bind(&JamomaScenario::ClockCallback, this, _1, _2);
  mClock->setExecutionCallback(clockCallback);
}

JamomaScenario::JamomaScenario(const JamomaScenario * other) :
JamomaTimeProcess(other->mCallback, other->mStartState, other->mEndState)
{}

shared_ptr<Scenario> JamomaScenario::clone() const
{
  return make_shared<JamomaScenario>(this);
}

JamomaScenario::~JamomaScenario()
{}

# pragma mark -
# pragma mark Execution

void JamomaScenario::play(bool log, string name) const
{
  // setup clock duration with parent constraint duration
  mClock->setDuration(mParent->getDuration());
  
  // reset internal State
  mCurrentState->stateElements().clear();
  
  // reset TimeEvent's status and build state
  for (const auto& timeNode : mTimeNodes)
  {
    TimeEvent::Status status = timeNode->getDate() < mClock->getOffset() ?
    TimeEvent::Status::HAPPENED : TimeEvent::Status::WAITING;
    
    for (auto& timeEvent : timeNode->timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      e->setStatus(status);
      
      if (status == TimeEvent::Status::HAPPENED)
        mCurrentState->stateElements().push_back(e->getState());
    }
  }
  
  // notify our owner
  //! \todo remove redundancy before
  (mCallback)(mClock->getPosition(), mClock->getDate(), mCurrentState);
  
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
    
    // activate start node
    timeConstraint->getStartEvent()->getTimeNode()->play();
  }
  
  mClock->go();
}

void JamomaScenario::stop() const
{
  mClock->stop();
}

void JamomaScenario::pause() const
{
  mClock->pause();
}

void JamomaScenario::resume() const
{
  mClock->resume();
}

shared_ptr<State> JamomaScenario::state() const
{
  return mCurrentState;
}

# pragma mark -
# pragma mark Edition

void JamomaScenario::addTimeConstraint(const shared_ptr<TimeConstraint> constraint)
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

void JamomaScenario::removeTimeConstraint(const shared_ptr<TimeConstraint> constraint)
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
  return mKiller;
}

void JamomaScenario::setKiller(bool isKiller)
{
  mKiller = isKiller;
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

const shared_ptr<TimeConstraint> & JamomaScenario::getParentTimeConstraint() const
{
  return mParent;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaScenario::ClockCallback(const TimeValue& position, const TimeValue& date)
{
  // reset internal State
  mCurrentState->stateElements().clear();
  
  // process each TimeNode
  for (const auto& timeNode : mTimeNodes)
  {
    TimeValue d = timeNode->getDate();
    
    for (auto& timeEvent : timeNode->timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      
      if (date > d && timeEvent->getStatus() == TimeEvent::Status::WAITING)
      {
        mCurrentState->stateElements().push_back(e->getState());
        e->setStatus(TimeEvent::Status::HAPPENED);
      }
    }
  }
  
  // process each running TimeProcess
  for (const auto& timeConstraint : mTimeContraints)
  {
    for (auto& timeProcess : timeConstraint->timeProcesses())
    {
      if (timeProcess->getClock()->getRunning())
      {
        if (timeProcess->getClock()->getExternal())
        {
          if (timeProcess->getClock()->tick())
            mCurrentState->stateElements().push_back(timeProcess->state());
        }
        else
          mCurrentState->stateElements().push_back(timeProcess->state());
      }
    }
  }
  
  // notify our owner
  (mCallback)(position, date, mCurrentState);
}