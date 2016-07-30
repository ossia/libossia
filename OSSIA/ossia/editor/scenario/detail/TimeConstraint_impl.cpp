#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include <iostream>
#include <ossia/detail/algorithms.hpp>
namespace impl
{
JamomaTimeConstraint::JamomaTimeConstraint(time_constraint::ExecutionCallback callback,
                                           std::shared_ptr<time_event> startEvent,
                                           std::shared_ptr<time_event> endEvent,
                                           time_value nominal,
                                           time_value min,
                                           time_value max) :
JamomaClock([=] (time_value t, time_value t2, unsigned char c) { return ClockCallback(t, t2, c); }),
mCallback(callback),
mStartEvent(startEvent),
mEndEvent(endEvent),
mDurationNominal(nominal),
mDurationMin(min),
mDurationMax(max)
{
    JamomaClock::setDuration(mDurationNominal);
}

JamomaTimeConstraint::~JamomaTimeConstraint()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeConstraint::start()
{
  if (mRunning)
    throw std::runtime_error("time constraint is running");

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

void JamomaTimeConstraint::stop()
{
  // stop the clock
  do_stop();

  // stop all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->stop();
  }
}

State JamomaTimeConstraint::offset(time_value date)
{
  if (mRunning)
    throw std::runtime_error("time constraint is running");

  do_setOffset(date);

  const auto& processes = timeProcesses();
  ossia::State state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    state.add(timeProcess->offset(date));
  }

  return state;
}

State JamomaTimeConstraint::state()
{
  if (!mRunning)
    throw std::runtime_error("time constraint is not running");

  const auto& processes = timeProcesses();
  ossia::State state;
  state.reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    state.add(timeProcess->state());
  }

  return state;
}

void JamomaTimeConstraint::pause()
{
  mPaused = true;

  // pause all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    timeProcess->pause();
  }
}

void JamomaTimeConstraint::resume()
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

# pragma mark -
# pragma mark Accessors

void JamomaTimeConstraint::setCallback(time_constraint::ExecutionCallback callback)
{
  mCallback = callback;
}

const time_value & JamomaTimeConstraint::getDurationNominal() const
{
  return mDurationNominal;
}

time_constraint & JamomaTimeConstraint::setDurationNominal(time_value durationNominal)
{
  mDurationNominal = durationNominal;

  if (mDurationNominal < mDurationMin)
    setDurationMin(mDurationNominal);

  if (mDurationNominal > mDurationMax)
    setDurationMax(mDurationNominal);

  JamomaClock::setDuration(mDurationNominal);

  return *this;
}

const time_value & JamomaTimeConstraint::getDurationMin() const
{
  return mDurationMin;
}

time_constraint & JamomaTimeConstraint::setDurationMin(time_value durationMin)
{
  mDurationMin = durationMin;

  if (mDurationMin > mDurationNominal)
    setDurationNominal(mDurationMin);

  return *this;
}

const time_value & JamomaTimeConstraint::getDurationMax() const
{
  return mDurationMax;
}

time_constraint & JamomaTimeConstraint::setDurationMax(time_value durationMax)
{
  mDurationMax = durationMax;

  if (durationMax < mDurationNominal)
    setDurationNominal(mDurationMax);

  return *this;
}

const std::shared_ptr<time_event> & JamomaTimeConstraint::getStartEvent() const
{
  return mStartEvent;
}

const std::shared_ptr<time_event> & JamomaTimeConstraint::getEndEvent() const
{
  return mEndEvent;
}

# pragma mark -
# pragma mark TimeProcesses

void JamomaTimeConstraint::addTimeProcess(std::shared_ptr<time_process> timeProcess)
{
  assert(timeProcess.get());
  // store a TimeProcess if it is not already stored
  if (find(timeProcesses().begin(),
           timeProcesses().end(),
           timeProcess) == timeProcesses().end())
  {
    timeProcesses().push_back(timeProcess);
    timeProcess->parent = shared_from_this();
  }
}

void JamomaTimeConstraint::removeTimeProcess(std::shared_ptr<time_process> timeProcess)
{
  auto it = find(timeProcesses().begin(), timeProcesses().end(), timeProcess);
  if (it != timeProcesses().end())
  {
      timeProcesses().erase(it);
      timeProcess.reset();
  }
}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeConstraint::ClockCallback(time_value position, time_value date, unsigned char droppedTicks)
{
  if (mCallback)
      (mCallback)(position, date, state());
}

}
