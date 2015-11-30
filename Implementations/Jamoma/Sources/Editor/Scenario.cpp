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

    // if the time goes backward
    if (date < mLastDate)
    {
      // offset or stop each TimeConstraint considering the date
      for (const auto& timeConstraint : mTimeContraints)
      {
        TimeValue start = timeConstraint->getStartEvent()->getTimeNode()->getDate();
        TimeValue end = start + timeConstraint->getDurationMax();

        if (date > start && date <= end)
          timeConstraint->setOffset(date - start);
        else
          timeConstraint->stop();
      }

      // add the state of each HAPPENED TimeEvent
      for (const auto& timeNode : mTimeNodes)
      {
        for (auto& timeEvent : timeNode->timeEvents())
        {
          if (timeEvent->getStatus() == TimeEvent::Status::HAPPENED)
            flattenAndFilter(timeEvent->getState());
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

    // process the scenario from the first TimeNode to the running constraints
    Container<TimeEvent> statusChangedEvents;
    shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mTimeNodes[0]);
    n->process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    for (const auto& timeEvent : statusChangedEvents)
      if (timeEvent->getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(timeEvent->getState());

    // make the time of each running TimeConstraint flows and add their state
    for (const auto& timeConstraint : mTimeContraints)
    {
      if (timeConstraint->getRunning())
      {
        if (timeConstraint->getDriveMode() == Clock::DriveMode::EXTERNAL)
          timeConstraint->tick();

        flattenAndFilter(timeConstraint->state(timeConstraint->getPosition(), timeConstraint->getDate()));
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

void JamomaScenario::start()
{}

void JamomaScenario::stop()
{}

void JamomaScenario::pause()
{
  // pause all running time constraints
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
  // resume all running time constraints
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

# pragma mark -
# pragma mark Implementation specific

void JamomaScenario::flattenAndFilter(const shared_ptr<StateElement>& element)
{
  if (!element)
      return;

  switch (element->getType())
  {
    case StateElement::Type::MESSAGE :
    {
      shared_ptr<Message> messageToAppend = dynamic_pointer_cast<Message>(element);

      // find message with the same address to replace it
      bool found = false;
      for (auto it = mCurrentState->stateElements().begin();
           it != mCurrentState->stateElements().end();
           it++)
      {
        shared_ptr<Message> messageToCheck = dynamic_pointer_cast<Message>(*it);

        // replace if addresses are the same
        if (messageToCheck->getAddress() == messageToAppend->getAddress())
        {
          *it = element;
          found = true;
          break;
        }
      }

      // if not found append it
      if (!found)
        mCurrentState->stateElements().push_back(element);

      break;
    }
    case StateElement::Type::STATE :
    {
      shared_ptr<State> stateToFlatAndFilter = dynamic_pointer_cast<State>(element);

      for (const auto& e : stateToFlatAndFilter->stateElements())
      {
        flattenAndFilter(e);
      }
      break;
    }

    default:
    {
      mCurrentState->stateElements().push_back(element);
      break;
    }
  }
}
