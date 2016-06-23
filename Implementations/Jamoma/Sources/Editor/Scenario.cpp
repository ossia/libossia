#include "Editor/JamomaScenario.h"
#include <Misc/Util.h>

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

  // reset internal offset list and state
  mPastEventList.clear();
  mOffsetState->stateElements().clear();

  // propagate offset from the first TimeNode
  process_offset(mTimeNodes[0], offset);

  // sort mPastEventList by date
  mPastEventList.sort();

  // build offset state from all ordered past events
  for (auto p : mPastEventList)
  {
    flattenAndFilter(mOffsetState, p.second->getState());
  }

  // offset all TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    // offset TimeConstraint's Clock
    TimeValue constraintOffset = offset - timeConstraint->getStartEvent()->getTimeNode()->getDate();

    if (constraintOffset >= Zero && constraintOffset <= timeConstraint->getDurationMax())
      flattenAndFilter(mOffsetState, timeConstraint->offset(constraintOffset));
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
    auto prev_last_date = mLastDate;
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
      if (timeConstraint->getDriveMode() != Clock::DriveMode::EXTERNAL)
        throw runtime_error("the pattern constraint clock is supposed to be in EXTERNAL drive mode");

      if (timeConstraint->getRunning())
      {
        // don't tick if the TimeConstraint is starting to avoid double ticks
        auto& startEvent = timeConstraint->getStartEvent();
        bool not_starting = none_of(statusChangedEvents,
                                    [&] (const std::shared_ptr<TimeEvent>& ev)
                                    {
                                      return ev->getStatus() == TimeEvent::Status::HAPPENED && ev == startEvent;
                                    });

        if (not_starting)
        {
          // no such event found : not starting
          if(prev_last_date == Infinite)
              timeConstraint->tick();
          else
              timeConstraint->tick((date - prev_last_date) * 1000.);
        }
        else
        {
            /*
            // We advance the constraint so that we don't loose time
            // TODO getDate is worst-case linear, maybe we should cache it to
            // have the executedDate in constant time ?
            if(prev_last_date == Infinite)
                timeConstraint->tick();
            else
                timeConstraint->tick(((date - timeConstraint->getStartEvent()->getTimeNode()->getDate())* 1000.));
            */
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
  timeConstraint->setDriveMode(Clock::DriveMode::EXTERNAL);
}

void JamomaScenario::removeTimeConstraint(const shared_ptr<TimeConstraint> timeConstraint)
{
  remove_one(mTimeContraints, timeConstraint);

  // set the TimeConstraint's clock in none external mode
  timeConstraint->setDriveMode(Clock::DriveMode::INTERNAL);
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

# pragma mark -
# pragma mark Implementation specific

void JamomaScenario::process_offset(shared_ptr<TimeNode> timenode, const TimeValue& offset)
{
  TimeValue date = timenode->getDate();

  for (const auto& event : timenode->timeEvents())
  {
    TimeEvent::Status eventStatus;

    // evaluate event status considering its time node date
    if (date < offset)
      eventStatus = event->getExpression()->evaluate() ? TimeEvent::Status::HAPPENED : TimeEvent::Status::DISPOSED;
    else if (date == offset)
      eventStatus = TimeEvent::Status::PENDING;
    else
      eventStatus = TimeEvent::Status::NONE;

    // evaluate event status considering previous time constraints
    for (const auto& timeConstraint : event->previousTimeConstraints())
    {
      TimeValue constraintOffset = offset - timeConstraint->getStartEvent()->getTimeNode()->getDate();

      if (constraintOffset < Zero)
      {
        eventStatus = TimeEvent::Status::NONE;
      }
      else if (constraintOffset >= Zero && constraintOffset <= timeConstraint->getDurationMax())
      {
        eventStatus = constraintOffset > timeConstraint->getDurationMin() ? TimeEvent::Status::PENDING : TimeEvent::Status::NONE;
      }
      else if (constraintOffset > timeConstraint->getDurationMax())
      {
        eventStatus = event->getExpression()->evaluate() ? TimeEvent::Status::HAPPENED : TimeEvent::Status::DISPOSED;
      }
    }

    // setup event status
    shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(event);
    e->setStatus(eventStatus);

    // add HAPPENED event to offset event list
    if (eventStatus == TimeEvent::Status::HAPPENED)
      mPastEventList.push_back(std::make_pair(date, event));

    // propagate offset processing to setup all TimeEvents
    for (const auto& timeConstraint : event->nextTimeConstraints())
    {
      process_offset(timeConstraint->getEndEvent()->getTimeNode(), offset);
    }
  }
}
