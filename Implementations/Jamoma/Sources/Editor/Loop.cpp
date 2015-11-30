#include "Editor/JamomaLoop.h"

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Loop> Loop::create(const TimeValue& patternDuration,
                                TimeConstraint::ExecutionCallback patternConstraintCallback,
                                TimeEvent::ExecutionCallback patternStartEventCallback,
                                TimeEvent::ExecutionCallback patternEndEventCallback)
  {
    return make_shared<JamomaLoop>(patternDuration, patternConstraintCallback, patternStartEventCallback, patternEndEventCallback);
  }
}

JamomaLoop::JamomaLoop(const TimeValue& patternDuration,
                       TimeConstraint::ExecutionCallback patternConstraintCallback,
                       TimeEvent::ExecutionCallback patternStartEventCallback,
                       TimeEvent::ExecutionCallback patternEndEventCallback) :
JamomaTimeProcess(State::create(), State::create()),
mPatternStartEventCallback(patternStartEventCallback),
mPatternEndEventCallback(patternEndEventCallback),
mPatternConstraintCallback(patternConstraintCallback)
{
  mPatternStartNode = TimeNode::create();
  mPatternStartNode->emplace(mPatternStartNode->timeEvents().begin(), std::bind(&JamomaLoop::PatternStartEventCallback, this, _1));

  mPatternEndNode = TimeNode::create();
  mPatternEndNode->emplace(mPatternEndNode->timeEvents().begin(), std::bind(&JamomaLoop::PatternEndEventCallback, this, _1));

  mPatternConstraint = TimeConstraint::create(std::bind(&JamomaLoop::PatternConstraintCallback, this, _1, _2, _3),
                                              mPatternStartNode->timeEvents()[0],
                                              mPatternEndNode->timeEvents()[0]);

  // set pattern TimeConstraint's clock in external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(mPatternConstraint);
  clock->setDriveMode(Clock::DriveMode::EXTERNAL);

  // set all pattern TimeConstraint's durations equal by default
  mPatternConstraint->setDurationNominal(patternDuration);
  mPatternConstraint->setDurationMin(patternDuration);
  mPatternConstraint->setDurationMax(patternDuration);

  mCurrentState = State::create();
}

JamomaLoop::JamomaLoop(const JamomaLoop * other) :
JamomaTimeProcess(other->mStartState, other->mEndState),
mPatternStartNode(other->mPatternStartNode->clone()),
mPatternEndNode(other->mPatternEndNode->clone()),
mPatternConstraint(other->mPatternConstraint->clone())
{}

shared_ptr<Loop> JamomaLoop::clone() const
{
  return make_shared<JamomaLoop>(this);
}

JamomaLoop::~JamomaLoop()
{}

Loop::~Loop()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaLoop::state(const TimeValue& position, const TimeValue& date)
{
  // if date hasn't been processed already
  if (date != mLastDate)
  {
    // reset internal State
    mCurrentState->stateElements().clear();

    // if the time goes backward
    if (date < mLastDate)
    {
      TimeValue offset = std::fmod((double)date, (double)mPatternConstraint->getDurationNominal());

      if (mPatternStartNode->timeEvents()[0]->getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(mPatternStartNode->timeEvents()[0]->getState());

      mPatternConstraint->setOffset(offset);
    }

    // process the loop from the pattern start TimeNode
    Container<TimeEvent> statusChangedEvents;
    shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mPatternStartNode);
    n->process(statusChangedEvents);

    // make time flow for the pattern constraint
    if (mPatternConstraint->getRunning())
    {
      if (mPatternConstraint->getDriveMode() == Clock::DriveMode::EXTERNAL)
      {
        mPatternConstraint->tick();
      }
      else
        throw runtime_error("the pattern constraint clock is supposed to be in EXTERNAL drive mode");
    }

    // if the pattern end event happened : reset the loop
    if (mPatternEndNode->timeEvents()[0]->getStatus() == TimeEvent::Status::HAPPENED)
    {
      flattenAndFilter(mPatternEndNode->timeEvents()[0]->getState());

      mPatternConstraint->stop();
      mPatternConstraint->setOffset(Zero);
    }

    mLastDate = date;
  }

  //! \see mCurrentState is filled below in JamomaLoop::PatternConstraintCallback
  return mCurrentState;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaLoop::start()
{}

void JamomaLoop::stop()
{}

void JamomaLoop::pause()
{
  mPatternConstraint->pause();
}

void JamomaLoop::resume()
{
  mPatternConstraint->resume();
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeConstraint> JamomaLoop::getPatternTimeConstraint() const
{
  return mPatternConstraint;
}

const shared_ptr<TimeNode> JamomaLoop::getPatternStartTimeNode() const
{
  return mPatternStartNode;
}

const shared_ptr<TimeNode> JamomaLoop::getPatternEndTimeNode() const
{
  return mPatternEndNode;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaLoop::flattenAndFilter(const shared_ptr<StateElement> element)
{
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

void JamomaLoop::PatternConstraintCallback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> state)
{
  if (mPatternConstraintCallback)
  {
    // add the state of the pattern TimeConstraint
    flattenAndFilter(mPatternConstraint->state(position, date));

    (mPatternConstraintCallback)(position, date, mCurrentState);
  }
}

void JamomaLoop::PatternStartEventCallback(TimeEvent::Status newStatus)
{
  if (mPatternStartEventCallback)
    (mPatternStartEventCallback)(newStatus);
}

void JamomaLoop::PatternEndEventCallback(TimeEvent::Status newStatus)
{
  if (mPatternEndEventCallback)
    (mPatternEndEventCallback)(newStatus);
}
