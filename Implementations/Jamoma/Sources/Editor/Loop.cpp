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
JamomaTimeProcess(),
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
  mOffsetState = State::create();
}

JamomaLoop::JamomaLoop(const JamomaLoop * other) :
JamomaTimeProcess(),
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

shared_ptr<StateElement> JamomaLoop::offset(const TimeValue& offset)
{
  if (parent->getRunning())
    throw runtime_error("parent time constraint is running");
  
  // reset internal mOffsetState
  mOffsetState->stateElements().clear();
  
  TimeValue patternOffset = std::fmod((double)offset, (double)mPatternConstraint->getDurationNominal());
  flattenAndFilter(mOffsetState, mPatternConstraint->offset(patternOffset));
  
  // compile mOffsetState with all HAPPENED event's states
  if (mPatternConstraint->getStartEvent()->getStatus() == TimeEvent::Status::HAPPENED)
    flattenAndFilter(mOffsetState, mPatternConstraint->getStartEvent()->getState());
  
  return mOffsetState;
}

shared_ptr<StateElement> JamomaLoop::state()
{
  if (!parent->getRunning())
    throw runtime_error("parent time constraint is not running");
  
  // if date hasn't been processed already
  TimeValue date = parent->getDate();
  if (date != mLastDate)
  {
    mLastDate = date;
    
    // reset internal State
    mCurrentState->stateElements().clear();

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
        // todo : don't tick if the TimeConstraint is starting to avoid double ticks
        mPatternConstraint->tick();
      }
      else
        throw runtime_error("the pattern constraint clock is supposed to be in EXTERNAL drive mode");
    }

    // if the pattern end event happened : reset the loop
    if (mPatternConstraint->getEndEvent()->getStatus() == TimeEvent::Status::HAPPENED)
    {
      mPatternConstraint->stop();
      mPatternConstraint->offset(Zero);
      
      shared_ptr<JamomaTimeEvent> start = dynamic_pointer_cast<JamomaTimeEvent>(mPatternConstraint->getStartEvent());
      start->setStatus(TimeEvent::Status::PENDING);
      
      shared_ptr<JamomaTimeEvent> end = dynamic_pointer_cast<JamomaTimeEvent>(mPatternConstraint->getEndEvent());
      end->setStatus(TimeEvent::Status::NONE);
    }
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

void JamomaLoop::PatternConstraintCallback(const TimeValue& position, const TimeValue& date, std::shared_ptr<State> state)
{
  if (mPatternConstraintCallback)
  {
    // add the state of the pattern TimeConstraint
    flattenAndFilter(mCurrentState, mPatternConstraint->state());

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
