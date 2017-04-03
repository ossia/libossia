#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <algorithm>
#include <iostream>
namespace ossia
{
std::shared_ptr<time_constraint> time_constraint::create(
    time_constraint::ExecutionCallback callback,
    time_event& startEvent,
    time_event& endEvent, time_value nominal, time_value min,
    time_value max)
{
  auto timeConstraint = std::make_shared<time_constraint>(
        callback, startEvent, endEvent, nominal, min, max);

  startEvent.nextTimeConstraints().push_back(timeConstraint);
  endEvent.previousTimeConstraints().push_back(timeConstraint);

  return timeConstraint;
}


time_constraint::time_constraint(
    time_constraint::ExecutionCallback callback,
    time_event& startEvent,
    time_event& endEvent, time_value nominal, time_value min,
    time_value max)
  : mCallback(callback)
  , mStartEvent(startEvent)
  , mEndEvent(endEvent)
  , mDurationNominal(nominal)
  , mDurationMin(min)
  , mDurationMax(max)
{
  mClock = std::make_unique<clock>(make_callback(), mDurationMax, One);
}

time_constraint::time_constraint(
    time_constraint::ExecutionCallback callback,
    std::unique_ptr<clock> c,
    time_event& startEvent,
    time_event& endEvent, time_value nominal, time_value min,
    time_value max)
  : mClock{std::move(c)}
  , mCallback(callback)
  , mStartEvent(startEvent)
  , mEndEvent(endEvent)
  , mDurationNominal(nominal)
  , mDurationMin(min)
  , mDurationMax(max)
{
  mClock->setCallback(make_callback());
}

time_constraint::~time_constraint()
{
}

void time_constraint::start()
{
  if (mClock->getRunning())
  {
    throw execution_error("time_constraint::start: "
                          "time constraint is already running");
    return;
  }

  // set clock duration using maximal duration
  mClock->setDuration(mDurationMax);

  // start all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    if(timeProcess->enabled())
      timeProcess->start();
  }

  // launch the clock
  mClock->do_start();
}

void time_constraint::stop()
{
  // stop the clock
  mClock->do_stop();

  // stop all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->stop();
  }

  mClock->mDate = 0;
  mClock->mPosition = 0;
  mClock->mLastTime = clock_type::time_point{};
  mClock->mElapsedTime = 0;
}

ossia::state time_constraint::offset(time_value date)
{
  if (mClock->getRunning())
  {
    throw execution_error("time_constraint::offset: "
                          "time constraint is running");
    return {};
  }

  mClock->do_setOffset(date);

  const auto& processes = timeProcesses();
  ossia::state state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    if(timeProcess->enabled())
    {
      auto res = timeProcess->offset(date);
      if(res)
        state.add(std::move(res));
    }
  }

  return state;
}

ossia::state time_constraint::state()
{
  if (!mClock->getRunning())
  {
    throw execution_error("time_constraint::state: "
                          "time constraint is not running");
    return {};
  }

  return state_impl();
}

void time_constraint::pause()
{
  mClock->pause();

  // pause all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->pause();
  }
}

void time_constraint::resume()
{
  mClock->resume();

  // resume all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->resume();
  }
}

void time_constraint::setCallback(
    time_constraint::ExecutionCallback callback)
{
  mCallback = callback;
  mClock->setCallback(make_callback());
}

void time_constraint::setStatelessCallback(time_constraint::ExecutionCallback cb)
{
  mCallback = cb;
  mClock->setCallback(make_stateless_callback());
}

const time_value& time_constraint::getDurationNominal() const
{
  return mDurationNominal;
}

time_constraint&
time_constraint::setDurationNominal(time_value durationNominal)
{
  mDurationNominal = durationNominal;

  if (mDurationNominal < mDurationMin)
    setDurationMin(mDurationNominal);

  if (mDurationNominal > mDurationMax)
    setDurationMax(mDurationNominal);

  mClock->setDuration(mDurationNominal);

  return *this;
}

const time_value& time_constraint::getDurationMin() const
{
  return mDurationMin;
}

time_constraint& time_constraint::setDurationMin(time_value durationMin)
{
  mDurationMin = durationMin;

  if (mDurationMin > mDurationNominal)
    setDurationNominal(mDurationMin);

  return *this;
}

const time_value& time_constraint::getDurationMax() const
{
  return mDurationMax;
}

time_constraint& time_constraint::setDurationMax(time_value durationMax)
{
  mDurationMax = durationMax;

  if (durationMax < mDurationNominal)
    setDurationNominal(mDurationMax);

  return *this;
}

time_event& time_constraint::getStartEvent() const
{
  return mStartEvent;
}

time_event& time_constraint::getEndEvent() const
{
  return mEndEvent;
}

void time_constraint::addTimeProcess(
    std::shared_ptr<time_process> timeProcess)
{
  if(!timeProcess)
    return;

  // store a TimeProcess if it is not already stored
  if (find(mTimeProcesses, timeProcess) == mTimeProcesses.end())
  {
    timeProcess->mParent = this;
    mTimeProcesses.push_back(std::move(timeProcess));
  }
}

void time_constraint::removeTimeProcess(
    time_process* timeProcess)
{
  auto it = find_if(mTimeProcesses, [=] (const auto& other) { return other.get() == timeProcess; });
  if (it != mTimeProcesses.end())
  {
    mTimeProcesses.erase(it);
  }
}

clock::ExecutionCallback time_constraint::make_callback()
{
  if(mCallback)
  {
    return [this] (time_value t, time_value t2, unsigned char c) {
      mCallback(t, t2, state_impl());
    };
  }
  else
  {
    return [this] (time_value t, time_value t2, unsigned char c) { };
  }
}


clock::ExecutionCallback time_constraint::make_stateless_callback()
{
  if(mCallback)
  {
    return [this] (time_value t, time_value t2, unsigned char c) {
      mCallback(t, t2, {});
    };
  }
  else
  {
    return [this] (time_value t, time_value t2, unsigned char c) { };
  }
}

ossia::state time_constraint::state_impl()
{
  const auto& processes = timeProcesses();
  ossia::state state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    if(timeProcess->enabled())
    {
      auto res = timeProcess->state();
      if(res)
        state.add(std::move(res));
    }
  }

  return state;
}

}
