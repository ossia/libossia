#include <ossia/editor/loop/loop.hpp>

#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
loop::loop(
    time_value patternDuration,
    time_constraint::ExecutionCallback patternConstraintCallback,
    time_event::ExecutionCallback patternStartEventCallback,
    time_event::ExecutionCallback patternEndEventCallback)
    : mPatternStartEventCallback(std::move(patternStartEventCallback))
    , mPatternEndEventCallback(std::move(patternEndEventCallback))
    , mPatternConstraintCallback(std::move(patternConstraintCallback))
{
  mPatternStartNode = std::make_shared<time_node>();
  mPatternStartNode->emplace(
      mPatternStartNode->timeEvents().begin(),
      [&](time_event::Status result) { PatternStartEventCallback(result); });

  mPatternEndNode = std::make_shared<time_node>();
  mPatternEndNode->emplace(
      mPatternEndNode->timeEvents().begin(),
      [&](time_event::Status result) { PatternEndEventCallback(result); });

  // create a pattern TimeConstraint with all durations equal by default
  mPatternConstraint = time_constraint::create(
      [=](time_value position, time_value date, const ossia::state& state) {
        return PatternConstraintCallback(position, date, state);
      },
      *mPatternStartNode->timeEvents()[0], *mPatternEndNode->timeEvents()[0],
      patternDuration, patternDuration, patternDuration);

  // set pattern TimeConstraint's clock in external mode
  mPatternConstraint->setDriveMode(clock::DriveMode::EXTERNAL);
}

loop::~loop()
{
  mPatternStartNode->cleanup();
  mPatternEndNode->cleanup();
}

state_element loop::offset(time_value offset)
{
  if (parent()->getRunning())
  {
    throw execution_error("loop::offset: "
                           "parent time constraint is running");
    return {};
  }
  // reset internal mOffsetState
  mOffsetState.clear();

  time_value patternOffset = std::fmod(
      (double)offset, (double)mPatternConstraint->getDurationNominal());
  flatten_and_filter(mOffsetState, mPatternConstraint->offset(patternOffset));

  // compile mOffsetState with all HAPPENED event's states
  if (mPatternConstraint->getStartEvent().getStatus()
      == time_event::Status::HAPPENED)
    flatten_and_filter(
        mOffsetState, mPatternConstraint->getStartEvent().getState());

  return mOffsetState;
}

state_element loop::state()
{
  auto& p = *parent();
  if (!p.getRunning())
  {
    throw execution_error("loop::state: "
                          "parent time constraint is not running");
    return {};
  }

  // if date hasn't been processed already
  time_value date = p.getDate();
  if (date != mLastDate)
  {
    auto prev_last_date = mLastDate;
    mLastDate = date;

    // reset internal State
    mCurrentState.clear();

    // process the loop from the pattern start TimeNode
    ptr_container<time_event> statusChangedEvents;
    mPatternStartNode->process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    for (const auto& timeEvent : statusChangedEvents)
      if (timeEvent->getStatus() == time_event::Status::HAPPENED)
        flatten_and_filter(mCurrentState, timeEvent->getState());

    // make time flow for the pattern constraint
    if (mPatternConstraint->getRunning())
    {
      if (mPatternConstraint->getDriveMode() != clock::DriveMode::EXTERNAL)
      {
        throw execution_error("loop::state: "
            "the pattern constraint clock is supposed to "
            "be in EXTERNAL drive mode");
        return {};
      }

      if (mPatternConstraint->getRunning())
      {
        // don't tick if the pattern constraint is starting to avoid double
        // ticks
        auto& startEvent = mPatternConstraint->getStartEvent();
        bool not_starting = none_of(
            statusChangedEvents, [&](const std::shared_ptr<time_event>& ev) {
              return ev->getStatus() == time_event::Status::HAPPENED
                     && ev.get() == &startEvent;
            });

        if (not_starting)
        {
          // no such event found : not starting
          // no such event found : not starting
          if (prev_last_date == Infinite)
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
        flatten_and_filter(mCurrentState, mPatternConstraint->state());
    }

    // if the pattern end event happened : stop and reset the loop
    if (mPatternConstraint->getEndEvent().getStatus()
        == time_event::Status::HAPPENED)
      stop();
  }

  //! \see mCurrentState is filled below in
  //! loop::PatternConstraintCallback
  return mCurrentState;
}

void loop::start()
{
}

void loop::stop()
{
  mPatternConstraint->stop();

  mPatternConstraint->offset(Zero);

  mPatternConstraint->getStartEvent().setStatus(time_event::Status::PENDING);
  mPatternConstraint->getEndEvent().setStatus(time_event::Status::NONE);
}

void loop::pause()
{
  mPatternConstraint->pause();
}

void loop::resume()
{
  mPatternConstraint->resume();
}

const std::shared_ptr<time_constraint>
loop::getPatternTimeConstraint() const
{
  return mPatternConstraint;
}

const std::shared_ptr<time_node> loop::getPatternStartTimeNode() const
{
  return mPatternStartNode;
}

const std::shared_ptr<time_node> loop::getPatternEndTimeNode() const
{
  return mPatternEndNode;
}

void loop::PatternConstraintCallback(
    time_value position, time_value date, const ossia::state&)
{
  if (mPatternConstraintCallback)
  {
    // add the state of the pattern TimeConstraint
    flatten_and_filter(mCurrentState, mPatternConstraint->state());

    (mPatternConstraintCallback)(position, date, mCurrentState);
  }
}

void loop::PatternStartEventCallback(time_event::Status newStatus)
{
  if (mPatternStartEventCallback)
    (mPatternStartEventCallback)(newStatus);
}

void loop::PatternEndEventCallback(time_event::Status newStatus)
{
  if (mPatternEndEventCallback)
    (mPatternEndEventCallback)(newStatus);
}
}
