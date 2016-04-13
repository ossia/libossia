#include "Editor/JamomaTimeConstraint.h"
#include <cassert>
#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<TimeConstraint> TimeConstraint::create(TimeConstraint::ExecutionCallback callback,
                                                    shared_ptr<TimeEvent> startEvent,
                                                    shared_ptr<TimeEvent> endEvent,
                                                    const TimeValue& nominal,
                                                    const TimeValue& min,
                                                    const TimeValue& max)
  {
    auto timeConstraint = make_shared<JamomaTimeConstraint>(callback, startEvent, endEvent, nominal, min, max);

    // store the TimeConstraint into the start event as a next constraint
    if (std::find(startEvent->nextTimeConstraints().begin(),
                  startEvent->nextTimeConstraints().end(),
                  timeConstraint) == startEvent->nextTimeConstraints().end())
    {
      startEvent->nextTimeConstraints().push_back(timeConstraint);
    }

    // store the TimeConstraint into the end event as a previous constraint
    if (std::find(endEvent->previousTimeConstraints().begin(),
                  endEvent->previousTimeConstraints().end(),
                  timeConstraint) == endEvent->previousTimeConstraints().end())
    {
      endEvent->previousTimeConstraints().push_back(timeConstraint);
    }

    return timeConstraint;
  }
}

JamomaTimeConstraint::JamomaTimeConstraint(TimeConstraint::ExecutionCallback callback,
                                           shared_ptr<TimeEvent> startEvent,
                                           shared_ptr<TimeEvent> endEvent,
                                           const TimeValue& nominal,
                                           const TimeValue& min,
                                           const TimeValue& max) :
JamomaClock(std::bind(&JamomaTimeConstraint::ClockCallback, this, _1, _2, _3)),
mCallback(callback),
mStartEvent(startEvent),
mEndEvent(endEvent),
mDurationNominal(nominal),
mDurationMin(min),
mDurationMax(max)
{
  mCurrentState = State::create();
  mOffsetState = State::create();
}

JamomaTimeConstraint::JamomaTimeConstraint(const JamomaTimeConstraint * other) :
JamomaClock(other),
mCallback(other->mCallback),
mStartEvent(other->mStartEvent),
mEndEvent(other->mEndEvent),
mDurationNominal(other->mDurationNominal),
mDurationMin(other->mDurationMin),
mDurationMax(other->mDurationMax)
{
    mCurrentState = State::create();
    mOffsetState = State::create();
}

shared_ptr<TimeConstraint> JamomaTimeConstraint::clone() const
{
  return make_shared<JamomaTimeConstraint>(this);
}

JamomaTimeConstraint::~JamomaTimeConstraint()
{}

TimeConstraint::~TimeConstraint()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeConstraint::start()
{
  if (mRunning)
    throw runtime_error("time constraint is running");
  
  // set clock duration using maximal duration
  setDuration(mDurationMax);
  
  // start all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
    if(t)
      t->start();
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
    JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
    if(t)
      t->stop();
  }
}

shared_ptr<State> JamomaTimeConstraint::offset(const TimeValue& date)
{
  if (mRunning)
    throw runtime_error("time constraint is running");
  
  do_setOffset(date);
  
  // clear internal offset state
  const auto& processes = timeProcesses();
  mOffsetState->stateElements().clear();
  mOffsetState->stateElements().reserve(processes.size());
  
  // get the offset state of each TimeProcess at offset
  for (const auto& timeProcess : processes)
  {
    if(auto state = timeProcess->offset(date))
      mOffsetState->stateElements().push_back(std::move(state));
    else
      std::cerr << "Warning: empty state for process: " << typeid(*timeProcess).name() << "\n";
  }
  
  return mOffsetState;
}

shared_ptr<State> JamomaTimeConstraint::state()
{
  if (!mRunning)
    throw runtime_error("time constraint is not running");
  
  // clear internal current state
  const auto& processes = timeProcesses();
  mCurrentState->stateElements().clear();
  mCurrentState->stateElements().reserve(processes.size());

  // get the state of each TimeProcess at current clock position and date
  for (const auto& timeProcess : processes)
  {
    if(auto state = timeProcess->state())
      mCurrentState->stateElements().push_back(std::move(state));
    else
      std::cerr << "Warning: empty state for process: " << typeid(*timeProcess).name() << "\n";
  }

  return mCurrentState;
}

void JamomaTimeConstraint::pause()
{
  mPaused = true;

  // pause all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
    if(t)
        t->pause();
  }
}

void JamomaTimeConstraint::resume()
{
  mPaused = false;

  // reset the time reference
  mLastTime = steady_clock::now();

  // resume all jamoma time processes
  for (const auto& timeProcess : timeProcesses())
  {
    JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
    if(t)
        t->resume();
  }
}

# pragma mark -
# pragma mark Accessors

void JamomaTimeConstraint::setCallback(TimeConstraint::ExecutionCallback callback)
{
  mCallback = callback;
}

const TimeValue & JamomaTimeConstraint::getDurationNominal() const
{
  return mDurationNominal;
}

TimeConstraint & JamomaTimeConstraint::setDurationNominal(const TimeValue& durationNominal)
{
  mDurationNominal = durationNominal;
  
  if (mDurationNominal < mDurationMin)
    setDurationMin(mDurationNominal);
  
  if (mDurationNominal > mDurationMax)
    setDurationMax(mDurationNominal);
  
  return *this;
}

const TimeValue & JamomaTimeConstraint::getDurationMin() const
{
  return mDurationMin;
}

TimeConstraint & JamomaTimeConstraint::setDurationMin(const TimeValue& durationMin)
{
  mDurationMin = durationMin;
  
  if (mDurationMin > mDurationNominal)
    setDurationNominal(mDurationMin);
  
  return *this;
}

const TimeValue & JamomaTimeConstraint::getDurationMax() const
{
  return mDurationMax;
}

TimeConstraint & JamomaTimeConstraint::setDurationMax(const TimeValue& durationMax)
{
  mDurationMax = durationMax;
  
  if (durationMax < mDurationNominal)
    setDurationNominal(mDurationMax);
  
  return *this;
}

const shared_ptr<TimeEvent> & JamomaTimeConstraint::getStartEvent() const
{
  return mStartEvent;
}

const shared_ptr<TimeEvent> & JamomaTimeConstraint::getEndEvent() const
{
  return mEndEvent;
}

# pragma mark -
# pragma mark TimeProcesses

void JamomaTimeConstraint::addTimeProcess(shared_ptr<TimeProcess> timeProcess)
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

void JamomaTimeConstraint::removeTimeProcess(std::shared_ptr<TimeProcess> timeProcess)
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

void JamomaTimeConstraint::ClockCallback(const TimeValue& position, const TimeValue& date, unsigned char droppedTicks)
{
  if (mCallback)
    (mCallback)(position, date, state());
}
