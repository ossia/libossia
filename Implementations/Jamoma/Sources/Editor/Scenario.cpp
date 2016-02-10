#include "Editor/JamomaScenario.h"

#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Scenario> Scenario::create(shared_ptr<State> startState,
                                        shared_ptr<State> endState)
  {
    return make_shared<JamomaScenario>(startState, endState);
  }
}

JamomaScenario::JamomaScenario(shared_ptr<State> startState,
                               shared_ptr<State> endState) :
JamomaTimeProcess(startState, endState)
{
  // create the start and the end TimeNodes
  mTimeNodes.push_back(TimeNode::create());
  mTimeNodes.push_back(TimeNode::create());

  mCurrentState = State::create();
  mOffsetState = State::create();
}

JamomaScenario::JamomaScenario(const JamomaScenario * other) :
JamomaTimeProcess(other->mStartState, other->mEndState)
{}

shared_ptr<Scenario> JamomaScenario::clone() const
{
  return make_shared<JamomaScenario>(this);
}

JamomaScenario::~JamomaScenario()
{}

Scenario::~Scenario()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaScenario::state(const TimeValue& position, const TimeValue& date)
{
  // if date hasn't been processed already
  if (date != mLastDate)
  {
    // reset internal State
    mCurrentState->stateElements().clear();
    
    // append offset state if needed
    if (!mOffsetState->stateElements().empty())
    {
      flattenAndFilter(mCurrentState, mOffsetState);
      mOffsetState->stateElements().clear();
    }

    // process the scenario from the first TimeNode to the running constraints
    Container<TimeEvent> statusChangedEvents;
    shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mTimeNodes[0]);
    n->process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    for (const auto& timeEvent : statusChangedEvents)
      if (timeEvent->getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(mCurrentState, timeEvent->getState());

    // make the time of each running TimeConstraint flows and add their state
    for (const auto& timeConstraint : mTimeContraints)
    {
      if (timeConstraint->getRunning())
      {
        if (timeConstraint->getDriveMode() == Clock::DriveMode::EXTERNAL)
          timeConstraint->tick();

        flattenAndFilter(mCurrentState, timeConstraint->state(timeConstraint->getPosition(), timeConstraint->getDate()));
      }
    }

    // if all the TimeEvents are not NONE : the Scenario is done
    bool done = true;
    for (const auto& timeNode : mTimeNodes)
    {
      for (auto& timeEvent : timeNode->timeEvents())
      {
        done = timeEvent->getStatus() != TimeEvent::Status::NONE;

        if (!done)
          break;
      }

      if (!done)
        break;
    }

    // if the Scenario is done : stop the parent TimeConstraint
    if (done)
    {
      if (date > mParent->getDurationMin())
        ;//! \todo mParent->stop(); // if the parent TimeConstraint's Clock is in EXTERNAL drive mode, it creates a deadlock.
    }

    mLastDate = date;
  }

  return mCurrentState;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaScenario::offset(const TimeValue& offset)
{
  // offset each TimeConstraint's Clock considering its start event date
  for (const auto& timeConstraint : mTimeContraints)
  {
    TimeValue start = timeConstraint->getStartEvent()->getTimeNode()->getDate();
    timeConstraint->setOffset(offset - start);
  }
  
  // compile mOffsetState with all HAPPENED event's states
  for (const auto& timeNode : mTimeNodes)
  {
    for (auto& timeEvent : timeNode->timeEvents())
    {
      if (timeEvent->getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(mOffsetState, timeEvent->getState());
    }
  }
  
  // start each TimeConstraint if possible
  for (const auto& timeConstraint : mTimeContraints)
  {
    TimeEvent::Status startStatus = timeConstraint->getStartEvent()->getStatus();
    TimeEvent::Status endStatus = timeConstraint->getEndEvent()->getStatus();
    
    // the constraint is in the past
    if (startStatus == TimeEvent::Status::HAPPENED &&
        endStatus == TimeEvent::Status::HAPPENED)
    {}
    // the start of the constraint is pending
    else if (startStatus == TimeEvent::Status::PENDING &&
             endStatus == TimeEvent::Status::NONE)
    {}
    // the constraint is supposed to be running
    else if (startStatus == TimeEvent::Status::HAPPENED &&
             endStatus == TimeEvent::Status::NONE)
    {
      timeConstraint->start();
    }
    // the end of the constraint is pending
    else if (startStatus == TimeEvent::Status::HAPPENED &&
             endStatus == TimeEvent::Status::PENDING)
    {}
    // the constraint is in the future
    else if (startStatus == TimeEvent::Status::NONE &&
             endStatus == TimeEvent::Status::NONE)
    {}
    // error
    else
      throw runtime_error("TimeEvent's status configuration of the TimeConstraint is not handled");
  }
}

void JamomaScenario::start()
{
  //! \see in JamomaScenario::state how scenario's elements are managed in time
}

void JamomaScenario::stop()
{
  // stop each running TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    if (timeConstraint->getRunning())
    {
      timeConstraint->stop();
    }
  }
}

void JamomaScenario::pause()
{
  // pause all running TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    if (timeConstraint->getRunning())
    {
      timeConstraint->pause();
    }
  }
}

void JamomaScenario::resume()
{
  // resume all running TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    if (timeConstraint->getRunning())
    {
      timeConstraint->resume();
    }
  }
}

# pragma mark -
# pragma mark Edition

void JamomaScenario::addTimeConstraint(const shared_ptr<TimeConstraint> timeConstraint)
{
  // store the TimeConstraint if it is not already stored
  if (std::find(mTimeContraints.begin(),
                mTimeContraints.end(),
                timeConstraint) == mTimeContraints.end())
  {
    mTimeContraints.push_back(timeConstraint);
  }

  // store TimeConstraint's start node if it is not already stored
  addTimeNode(timeConstraint->getStartEvent()->getTimeNode());

  // store TimeConstraint's end node if it is not already stored
  addTimeNode(timeConstraint->getEndEvent()->getTimeNode());

  // set TimeConstraint's clock in external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint);
  clock->setDriveMode(Clock::DriveMode::EXTERNAL);
}

void JamomaScenario::removeTimeConstraint(const shared_ptr<TimeConstraint> timeConstraint)
{
  auto it = find(mTimeContraints.begin(), mTimeContraints.end(), timeConstraint);
  if (it != mTimeContraints.end())
  {
      mTimeContraints.erase(it);
  }

  // set the TimeConstraint's clock in none external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint);
  clock->setDriveMode(Clock::DriveMode::INTERNAL);
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
  auto it = find(mTimeNodes.begin(), mTimeNodes.end(), timeNode);
  if (it != mTimeNodes.end())
  {
      mTimeNodes.erase(it);
  }
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaScenario::getStartTimeNode() const
{
  return mTimeNodes[0];
}

const shared_ptr<TimeNode> & JamomaScenario::getEndTimeNode() const
{
  return mTimeNodes[1];
}

# pragma mark -
# pragma mark TimeNodes and TimeConstraints

const Container<TimeNode>& JamomaScenario::timeNodes() const
{
  return mTimeNodes;
}

const Container<TimeConstraint>& JamomaScenario::timeConstraints() const
{
  return mTimeContraints;
}