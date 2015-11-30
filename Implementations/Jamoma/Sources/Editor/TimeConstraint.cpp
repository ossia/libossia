#include "Editor/JamomaTimeConstraint.h"

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
}

JamomaTimeConstraint::JamomaTimeConstraint(const JamomaTimeConstraint * other) :
JamomaClock(other),
mCallback(other->mCallback),
mStartEvent(other->mStartEvent),
mEndEvent(other->mEndEvent),
mDurationNominal(other->mDurationNominal),
mDurationMin(other->mDurationMin),
mDurationMax(other->mDurationMax)
{}

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
    throw runtime_error("cannot start a running time constraint");
  
  // launch the clock
  do_start();
}

void JamomaTimeConstraint::stop()
{
  do_stop();
}

shared_ptr<StateElement> JamomaTimeConstraint::state(const TimeValue& position, const TimeValue& date)
{
  // clear internal State, Message and Value
  const auto& processes = timeProcesses();
  mCurrentState->stateElements().clear();
  mCurrentState->stateElements().reserve(processes.size());

  // get the state of each TimeProcess for the position and the date
  for (const auto& timeProcess : processes)
  {
    mCurrentState->stateElements().push_back(timeProcess->state(position, date));
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

Clock & JamomaTimeConstraint::setOffset(const TimeValue& offset)
{
  do_setOffset(offset);

  // edit TimeEvent status
  TimeEvent::Status startStatus = mOffset >= Zero ? mOffset == Zero ? TimeEvent::Status::PENDING : TimeEvent::Status::HAPPENED : TimeEvent::Status::NONE;
  TimeEvent::Status endStatus = mOffset > mDurationMin ? mOffset <= mDurationMax ? TimeEvent::Status::PENDING : TimeEvent::Status::HAPPENED : TimeEvent::Status::NONE;

  //! \note maybe we should initialized TimeEvents with an Expression returning false to DISPOSED status ?

  shared_ptr<JamomaTimeEvent> start = dynamic_pointer_cast<JamomaTimeEvent>(mStartEvent);
  start->setStatus(startStatus);

  shared_ptr<JamomaTimeEvent> end = dynamic_pointer_cast<JamomaTimeEvent>(mEndEvent);
  end->setStatus(endStatus);

  return *this;
}

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
  
  // set clock duration using maximal duration
  setDuration(mDurationMax);
  
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
  // store a TimeProcess if it is not already stored
  if (find(timeProcesses().begin(),
           timeProcesses().end(),
           timeProcess) == timeProcesses().end())
  {
    timeProcesses().push_back(timeProcess);
    mStartEvent->addState(timeProcess->getStartState());
    mEndEvent->addState(timeProcess->getEndState());

    JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
    if (t)
    {
        t->setParentTimeConstraint(shared_from_this());
    }
  }
}

void JamomaTimeConstraint::removeTimeProcess(std::shared_ptr<TimeProcess> timeProcess)
{
  auto it = find(timeProcesses().begin(), timeProcesses().end(), timeProcess);
  if (it != timeProcesses().end())
  {
      timeProcesses().erase(it);

      if (timeProcess)
      {
          mStartEvent->removeState(timeProcess->getStartState());
          mEndEvent->removeState(timeProcess->getEndState());

          JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
          if (t)
          {
              t->setParentTimeConstraint(nullptr);
          }
      }
  }
}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeConstraint::ClockCallback(const TimeValue& position, const TimeValue& date, unsigned char droppedTicks)
{
  if (mCallback)
    (mCallback)(position, date, state(position, date));
}
