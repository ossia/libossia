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
JamomaClock(std::bind(&JamomaTimeConstraint::ClockCallback, this, _1, _2, _3), nominal),
mCallback(callback),
mStartEvent(startEvent),
mEndEvent(endEvent),
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
mDurationMin(other->mDurationMin),
mDurationMax(other->mDurationMax)
{}

shared_ptr<TimeConstraint> JamomaTimeConstraint::clone() const
{
  return make_shared<JamomaTimeConstraint>(this);
}

JamomaTimeConstraint::~JamomaTimeConstraint()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaTimeConstraint::state(const TimeValue& position, const TimeValue& date)
{
  // clear internal State, Message and Value
  mCurrentState->stateElements().clear();
  
  // get the state of each TimeProcess for the position and the date
  for (auto& timeProcess : timeProcesses())
  {
    mCurrentState->stateElements().push_back(timeProcess->state(position, date));
  }
  
  return mCurrentState;
}

# pragma mark -
# pragma mark Accessors

const TimeValue & JamomaTimeConstraint::getDurationMin() const
{
  return mDurationMin;
}

TimeConstraint & JamomaTimeConstraint::setDurationMin(const TimeValue& durationMin)
{
  if (durationMin > mDuration)
    throw runtime_error("duration min can't be greater than duration");
  
  mDurationMin = durationMin;
  return *this;
}

const TimeValue & JamomaTimeConstraint::getDurationMax() const
{
  return mDurationMax;
}

TimeConstraint & JamomaTimeConstraint::setDurationMax(const TimeValue& durationMax)
{
  if (durationMax < mDuration)
    throw runtime_error("duration max can't be less than duration");
  
  mDurationMax = durationMax;
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
    t->setParentTimeConstraint(shared_from_this());
  }
}

void JamomaTimeConstraint::removeTimeProcess(std::shared_ptr<TimeProcess> timeProcess)
{
  timeProcesses().erase(find(timeProcesses().begin(), timeProcesses().end(), timeProcess));
  mStartEvent->removeState(timeProcess->getStartState());
  mEndEvent->removeState(timeProcess->getEndState());
  
  JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
  t->setParentTimeConstraint(nullptr);
}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeConstraint::process()
{
  // make the time flow for external drived clock
  if (mExternal)
    tick();
  
  if (mRunning)
  {
    //! \debug
    //cout << "TimeConstraint::process() : running" << endl;
    
    // when all durations are equals
    if (mDurationMin == mDuration &&
        mDurationMax == mDuration)
    {
      // do nothing and wait for the clock end
      
      //! \debug
      cout << "TimeConstraint::process() : duration are equals => do nothing" << endl;
      
      return;
    }
    // else if the minimal duration is reached
    else if (mDate >= mDurationMin &&
             mDate < mDurationMax &&
             mEndEvent->getStatus() == TimeEvent::Status::NONE)
    {
      
      //! \debug
      cout << "TimeConstraint::process() : date > duration min => set PENDING" << endl;
      
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(mEndEvent);
      e->setStatus(TimeEvent::Status::PENDING);
    }
    // else if the maximal duration is reached
    else if (mDate >= mDurationMax &&
             mEndEvent->getStatus() == TimeEvent::Status::PENDING)
    {
      //! \debug
      cout << "TimeConstraint::process() : date > duration max => stop" << endl;
      
      stop();
    }
    else
    {
      // do nothing and wait for the clock end
      return;
    }
  }
  
  // the clock ends so now the end event is PENDING
  if (mEndEvent->getStatus() == TimeEvent::Status::NONE)
  {
    shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(mEndEvent);
    e->setStatus(TimeEvent::Status::PENDING);
  }
  
  //! \debug
  //cout << "TimeConstraint::process() : process node" << endl;
  
  // process the end event TimeNode
  shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mEndEvent->getTimeNode());
  n->process();
  
}

void JamomaTimeConstraint::ClockCallback(const TimeValue& position, const TimeValue& date, unsigned char droppedTicks)
{
  (mCallback)(position, date, state(position, date));
}