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

  // create a pattern TimeConstraint with all durations equal by default
  mPatternConstraint = TimeConstraint::create(std::bind(&JamomaLoop::PatternConstraintCallback, this, _1, _2, _3),
                                              mPatternStartNode->timeEvents()[0],
                                              mPatternEndNode->timeEvents()[0],
                                              patternDuration,
                                              patternDuration,
                                              patternDuration);

  // set pattern TimeConstraint's clock in external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(mPatternConstraint);
  clock->setDriveMode(Clock::DriveMode::EXTERNAL);

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

    // append offset state if needed
    if (!mOffsetState->stateElements().empty())
    {
      flattenAndFilter(mCurrentState, mOffsetState);
      mOffsetState->stateElements().clear();
    }

    // process the loop from the pattern start TimeNode
    Container<TimeEvent> statusChangedEvents;
    shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mPatternStartNode);
    n->process(statusChangedEvents);
    
    // add the state of each newly HAPPENED TimeEvent
    for (const auto& timeEvent : statusChangedEvents)
      if (timeEvent->getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(mCurrentState, timeEvent->getState());

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

void JamomaLoop::offset(const TimeValue& offset)
{
  TimeValue patternOffset = std::fmod((double)offset, (double)mPatternConstraint->getDurationNominal());
  mPatternConstraint->setOffset(patternOffset);
  
  // compile mOffsetState with all HAPPENED event's states
  if (mPatternStartNode->timeEvents()[0]->getStatus() == TimeEvent::Status::HAPPENED)
    flattenAndFilter(mOffsetState, mPatternStartNode->timeEvents()[0]->getState());
}

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

void JamomaLoop::PatternConstraintCallback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> state)
{
  if (mPatternConstraintCallback)
  {
    // add the state of the pattern TimeConstraint
    flattenAndFilter(mCurrentState, mPatternConstraint->state(position, date));

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
