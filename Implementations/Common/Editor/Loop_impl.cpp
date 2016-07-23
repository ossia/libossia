#include "Loop_impl.h"
#include <Misc/Util.h>

namespace impl
{

JamomaLoop::JamomaLoop(TimeValue patternDuration,
                       TimeConstraint::ExecutionCallback patternConstraintCallback,
                       TimeEvent::ExecutionCallback patternStartEventCallback,
                       TimeEvent::ExecutionCallback patternEndEventCallback) :
JamomaTimeProcess(),
mPatternStartEventCallback(patternStartEventCallback),
mPatternEndEventCallback(patternEndEventCallback),
mPatternConstraintCallback(patternConstraintCallback)
{
  mPatternStartNode = TimeNode::create();
  mPatternStartNode->emplace(mPatternStartNode->timeEvents().begin(), [&] (TimeEvent::Status result) { PatternStartEventCallback(result); });

  mPatternEndNode = TimeNode::create();
  mPatternEndNode->emplace(mPatternEndNode->timeEvents().begin(), [&] (TimeEvent::Status result) { PatternEndEventCallback(result); });

  // create a pattern TimeConstraint with all durations equal by default
  mPatternConstraint = TimeConstraint::create([=] (TimeValue position, TimeValue date, std::shared_ptr<State> state) { return PatternConstraintCallback(position, date, state); },
                                              mPatternStartNode->timeEvents()[0],
                                              mPatternEndNode->timeEvents()[0],
                                              patternDuration,
                                              patternDuration,
                                              patternDuration);

  // set pattern TimeConstraint's clock in external mode
  mPatternConstraint->setDriveMode(Clock::DriveMode::EXTERNAL);

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

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaLoop::offset(TimeValue offset)
{
  if (parent->getRunning())
    throw runtime_error("parent time constraint is running");

  // reset internal mOffsetState
  mOffsetState->stateElements().clear();

  TimeValue patternOffset = std::fmod((double)offset, (double)mPatternConstraint->getDurationNominal());
  flattenAndFilter(*mOffsetState, mPatternConstraint->offset(patternOffset));

  // compile mOffsetState with all HAPPENED event's states
  if (mPatternConstraint->getStartEvent()->getStatus() == TimeEvent::Status::HAPPENED)
    flattenAndFilter(*mOffsetState, mPatternConstraint->getStartEvent()->getState());

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
    auto prev_last_date = mLastDate;
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
        flattenAndFilter(*mCurrentState, timeEvent->getState());

    // make time flow for the pattern constraint
    if (mPatternConstraint->getRunning())
    {
      if (mPatternConstraint->getDriveMode() != Clock::DriveMode::EXTERNAL)
        throw runtime_error("the pattern constraint clock is supposed to be in EXTERNAL drive mode");

      if (mPatternConstraint->getRunning())
      {
        // don't tick if the pattern constraint is starting to avoid double ticks
        auto& startEvent = mPatternConstraint->getStartEvent();
        bool not_starting = none_of(statusChangedEvents,
                                    [&] (const std::shared_ptr<TimeEvent>& ev)
                                    {
                                      return ev->getStatus() == TimeEvent::Status::HAPPENED && ev == startEvent;
                                    });

        if (not_starting)
        {
          // no such event found : not starting
            // no such event found : not starting
            if(prev_last_date == Infinite)
                mPatternConstraint->tick();
            else
                mPatternConstraint->tick((date - prev_last_date) * 1000.);
        }
        else
        {
            // TODO we should advance the loop a bit at least.
        }
      }

      // if the pattern constraint is still running after the tick
      if (mPatternConstraint->getRunning())
        flattenAndFilter(*mCurrentState, mPatternConstraint->state());
    }

    // if the pattern end event happened : stop and reset the loop
    if (mPatternConstraint->getEndEvent()->getStatus() == TimeEvent::Status::HAPPENED)
      stop();
  }

  //! \see mCurrentState is filled below in JamomaLoop::PatternConstraintCallback
  return mCurrentState;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaLoop::start()
{}

void JamomaLoop::stop()
{
  mPatternConstraint->stop();

  mPatternConstraint->offset(Zero);

  shared_ptr<JamomaTimeEvent> start = dynamic_pointer_cast<JamomaTimeEvent>(mPatternConstraint->getStartEvent());
  start->setStatus(TimeEvent::Status::PENDING);

  shared_ptr<JamomaTimeEvent> end = dynamic_pointer_cast<JamomaTimeEvent>(mPatternConstraint->getEndEvent());
  end->setStatus(TimeEvent::Status::NONE);
}

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

void JamomaLoop::PatternConstraintCallback(
    TimeValue position,
    TimeValue date,
    const std::shared_ptr<State>& state)
{
  if (mPatternConstraintCallback)
  {
    // add the state of the pattern TimeConstraint
    flattenAndFilter(*mCurrentState, mPatternConstraint->state());

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

}
