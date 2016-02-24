#include "Editor/JamomaScenario.h"
#include <Misc/Util.h>

#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Scenario> Scenario::create()
  {
    return make_shared<JamomaScenario>();
  }
}

JamomaScenario::JamomaScenario() :
JamomaTimeProcess()
{
  // create the start TimeNode
  mTimeNodes.push_back(TimeNode::create());

  mCurrentState = State::create();
  mOffsetState = State::create();
}

JamomaScenario::JamomaScenario(const JamomaScenario * other) :
JamomaTimeProcess()
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

shared_ptr<StateElement> JamomaScenario::offset(const TimeValue& offset)
{
  if (parent->getRunning())
    throw runtime_error("parent time constraint is running");
  
  // reset internal mOffsetState
  mOffsetState->stateElements().clear();
  
  // TimeConstraints setup
  for (const auto& timeConstraint : mTimeContraints)
  {
    // offset TimeConstraint's Clock
    TimeValue constraintOffset = offset - timeConstraint->getStartEvent()->getTimeNode()->getDate();
    flattenAndFilter(mOffsetState, timeConstraint->offset(constraintOffset));
    
    // setup start and end TimeEvent's status
    TimeEvent::Status startStatus = TimeEvent::Status::NONE;
    TimeEvent::Status endStatus = TimeEvent::Status::NONE;
    
    if (constraintOffset >= Zero && constraintOffset <= timeConstraint->getDurationMax())
    {
      startStatus = constraintOffset == Zero ? TimeEvent::Status::PENDING : TimeEvent::Status::HAPPENED;
      endStatus = constraintOffset > timeConstraint->getDurationMin() ? TimeEvent::Status::PENDING : TimeEvent::Status::NONE;
    }
    else if (constraintOffset > timeConstraint->getDurationMax())
    {
      startStatus = TimeEvent::Status::HAPPENED;
      endStatus = TimeEvent::Status::HAPPENED;
    }
    
    //! \note maybe we should initialized TimeEvents with an Expression returning false to DISPOSED status ?
    
    shared_ptr<JamomaTimeEvent> start = dynamic_pointer_cast<JamomaTimeEvent>(timeConstraint->getStartEvent());
    start->setStatus(startStatus);
    
    shared_ptr<JamomaTimeEvent> end = dynamic_pointer_cast<JamomaTimeEvent>(timeConstraint->getEndEvent());
    end->setStatus(endStatus);
  }
  
  // TimeNodes setup
  for (const auto& timeNode : mTimeNodes)
  {
    for (auto& timeEvent : timeNode->timeEvents())
    {
      // compile mOffsetState with all HAPPENED event's states
      if (timeEvent->getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(mOffsetState, timeEvent->getState());
    }
  }
  
  return mOffsetState;
}

shared_ptr<StateElement> JamomaScenario::state()
{
  if (!parent->getRunning())
    throw runtime_error("parent time constraint is not running");
  
  // if date hasn't been processed already
  TimeValue date = parent->getDate();
  if (date != mLastDate)
  {
    mLastDate = date;
    
    // reset internal mCurrentState
    mCurrentState->stateElements().clear();

    // process the scenario from the first TimeNode to the running constraints
    Container<TimeEvent> statusChangedEvents;
    shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mTimeNodes[0]);
    n->process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    for (const auto& timeEvent : statusChangedEvents)
      if (timeEvent->getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(mCurrentState, timeEvent->getState());

    // make the time of each running TimeConstraint flows and add their state
    // note : this means TimeConstraint's state can overwrite TimeEvent's state
    for (const auto& timeConstraint : mTimeContraints)
    {
      if (timeConstraint->getRunning() &&
          timeConstraint->getDriveMode() == Clock::DriveMode::EXTERNAL)
      {
        // don't tick if the TimeConstraint is starting to avoid double ticks
        auto& startEv = timeConstraint->getStartEvent();
        bool not_starting = none_of(
                    statusChangedEvents,
                    [&] (const std::shared_ptr<TimeEvent>& ev) {
            return ev->getStatus() == TimeEvent::Status::HAPPENED &&
                   ev == startEv;
        });

        if(not_starting)
        {
          // No such event found; not starting
          timeConstraint->tick();
        }
      }
        
      // if the time constraint is still running after the tick
      if (timeConstraint->getRunning())
          flattenAndFilter(mCurrentState, timeConstraint->state());
    }

    // if all the TimeEvents are not NONE : the Scenario is done
    bool done = !any_of(mTimeNodes,
           [] (const std::shared_ptr<TimeNode>& tn)
    {
        return any_of(tn->timeEvents(),
            [] (const std::shared_ptr<TimeEvent>& ev)
        {
            return ev->getStatus() == TimeEvent::Status::NONE;
        });
    });

    // if the Scenario is done : stop the parent TimeConstraint
    if (done)
    {
      if (date > parent->getDurationMin())
        ;//! \todo mParent->stop(); // if the parent TimeConstraint's Clock is in EXTERNAL drive mode, it creates a deadlock.
    }
  }

  return mCurrentState;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaScenario::start()
{
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
  if (!contains(mTimeContraints, timeConstraint))
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
  remove_one(mTimeContraints, timeConstraint);

  // set the TimeConstraint's clock in none external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint);
  clock->setDriveMode(Clock::DriveMode::INTERNAL);
}

void JamomaScenario::addTimeNode(const shared_ptr<TimeNode> timeNode)
{
  // store a TimeNode if it is not already stored
  if (!contains(mTimeNodes, timeNode))
  {
    mTimeNodes.push_back(timeNode);
  }
}

void JamomaScenario::removeTimeNode(const shared_ptr<TimeNode> timeNode)
{
  remove_one(mTimeNodes, timeNode);
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaScenario::getStartTimeNode() const
{
  return mTimeNodes[0];
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
