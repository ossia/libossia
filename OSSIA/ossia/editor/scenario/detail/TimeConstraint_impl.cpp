#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include <iostream>

namespace ossia
{
namespace detail
{
time_constraint_impl::time_constraint_impl(
    time_constraint::ExecutionCallback callback,
    std::shared_ptr<time_event> startEvent,
    std::shared_ptr<time_event> endEvent, time_value nominal, time_value min,
    time_value max)
    : clock_impl([=](time_value t, time_value t2, unsigned char c) {
      return ClockCallback(t, t2, c);
    })
    , mCallback(callback)
    , mStartEvent(startEvent)
    , mEndEvent(endEvent)
    , mDurationNominal(nominal)
    , mDurationMin(min)
    , mDurationMax(max)
{
  clock_impl::setDuration(mDurationNominal);
}

time_constraint_impl::~time_constraint_impl()
{
}

void time_constraint_impl::start()
{
  if (mRunning)
    throw execution_error("time_constraint_impl::start: "
                          "time constraint is already running");

  // set clock duration using maximal duration
  setDuration(mDurationMax);

  // start all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->start();
  }

  // launch the clock
  do_start();
}

void time_constraint_impl::stop()
{
  // stop the clock
  do_stop();

  // stop all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->stop();
  }
}

ossia::state time_constraint_impl::offset(time_value date)
{
  if (mRunning)
    throw execution_error("time_constraint_impl::offset: "
                          "time constraint is running");

  do_setOffset(date);

  const auto& processes = timeProcesses();
  ossia::state state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    state.add(timeProcess->offset(date));
  }

  return state;
}

ossia::state time_constraint_impl::state()
{
  if (!mRunning)
    throw execution_error("time_constraint_impl::state: "
                          "time constraint is not running");

  const auto& processes = timeProcesses();
  ossia::state state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    state.add(timeProcess->state());
  }

  return state;
}

void time_constraint_impl::pause()
{
  mPaused = true;

  // pause all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->pause();
  }
}

void time_constraint_impl::resume()
{
  mPaused = false;

  // reset the time reference
  mLastTime = clock_type::now();

  // resume all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->resume();
  }
}

void time_constraint_impl::setCallback(
    time_constraint::ExecutionCallback callback)
{
  mCallback = callback;
}

const time_value& time_constraint_impl::getDurationNominal() const
{
  return mDurationNominal;
}

time_constraint&
time_constraint_impl::setDurationNominal(time_value durationNominal)
{
  mDurationNominal = durationNominal;

  if (mDurationNominal < mDurationMin)
    setDurationMin(mDurationNominal);

  if (mDurationNominal > mDurationMax)
    setDurationMax(mDurationNominal);

  clock_impl::setDuration(mDurationNominal);

  return *this;
}

const time_value& time_constraint_impl::getDurationMin() const
{
  return mDurationMin;
}

time_constraint& time_constraint_impl::setDurationMin(time_value durationMin)
{
  mDurationMin = durationMin;

  if (mDurationMin > mDurationNominal)
    setDurationNominal(mDurationMin);

  return *this;
}

const time_value& time_constraint_impl::getDurationMax() const
{
  return mDurationMax;
}

time_constraint& time_constraint_impl::setDurationMax(time_value durationMax)
{
  mDurationMax = durationMax;

  if (durationMax < mDurationNominal)
    setDurationNominal(mDurationMax);

  return *this;
}

const std::shared_ptr<time_event>& time_constraint_impl::getStartEvent() const
{
  return mStartEvent;
}

const std::shared_ptr<time_event>& time_constraint_impl::getEndEvent() const
{
  return mEndEvent;
}

void time_constraint_impl::addTimeProcess(
    std::shared_ptr<time_process> timeProcess)
{
  assert(timeProcess.get());
  // store a TimeProcess if it is not already stored
  if (find(timeProcesses().begin(), timeProcesses().end(), timeProcess)
      == timeProcesses().end())
  {
    timeProcesses().push_back(timeProcess);
    timeProcess->parent = shared_from_this();
  }
}

void time_constraint_impl::removeTimeProcess(
    std::shared_ptr<time_process> timeProcess)
{
  auto it = find(timeProcesses().begin(), timeProcesses().end(), timeProcess);
  if (it != timeProcesses().end())
  {
    timeProcesses().erase(it);
    timeProcess.reset();
  }
}

void time_constraint_impl::ClockCallback(
    time_value position, time_value date, unsigned char droppedTicks)
{
  if (mCallback)
    (mCallback)(position, date, state());
}
}
}
