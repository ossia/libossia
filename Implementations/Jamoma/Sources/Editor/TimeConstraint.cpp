#include "Editor/JamomaTimeConstraint.h"
#include <algorithm>
# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<TimeConstraint> TimeConstraint::create(shared_ptr<TimeEvent> startEvent,
                                                    shared_ptr<TimeEvent> endEvent,
                                                    const TimeValue& nominal,
                                                    const TimeValue& min,
                                                    const TimeValue& max)
  {
    auto timeConstraint = make_shared<JamomaTimeConstraint>(startEvent, endEvent, nominal, min, max);
    
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

JamomaTimeConstraint::JamomaTimeConstraint(shared_ptr<TimeEvent> startEvent,
                                           shared_ptr<TimeEvent> endEvent,
                                           const TimeValue& nominal,
                                           const TimeValue& min,
                                           const TimeValue& max) :
mStartEvent(startEvent),
mEndEvent(endEvent),
mDuration(nominal),
mDurationMin(min),
mDurationMax(max)
{}

JamomaTimeConstraint::JamomaTimeConstraint(const JamomaTimeConstraint * other)
{}

JamomaTimeConstraint::~JamomaTimeConstraint()
{}

shared_ptr<TimeConstraint> JamomaTimeConstraint::clone() const
{
  return make_shared<JamomaTimeConstraint>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaTimeConstraint::play(bool log, string name) const
{
  for (const auto & timeProcess : timeProcesses())
    timeProcess->play();
}

void JamomaTimeConstraint::stop() const
{
  for (const auto & timeProcess : timeProcesses())
    timeProcess->stop();
}

void JamomaTimeConstraint::pause() const
{
  for (const auto & timeProcess : timeProcesses())
    timeProcess->pause();
}

void JamomaTimeConstraint::resume() const
{
  for (const auto & timeProcess : timeProcesses())
    timeProcess->resume();
}

# pragma mark -
# pragma mark Accessors

const TimeValue & JamomaTimeConstraint::getDuration() const
{
  return mDuration;
}

TimeConstraint & JamomaTimeConstraint::setDuration(const TimeValue& duration)
{
  mDuration = duration;
  return *this;
}

const TimeValue & JamomaTimeConstraint::getDurationMin() const
{
  return mDurationMin;
}

TimeConstraint & JamomaTimeConstraint::setDurationMin(const TimeValue& durationMin)
{
  mDurationMin = durationMin;
  return *this;
}

const TimeValue & JamomaTimeConstraint::getDurationMax() const
{
  return mDurationMax;
}

TimeConstraint & JamomaTimeConstraint::setDurationMax(const TimeValue& durationMax)
{
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
    t->mParent = shared_from_this();
  }
}

void JamomaTimeConstraint::removeTimeProcess(std::shared_ptr<TimeProcess> timeProcess)
{
  timeProcesses().erase(find(timeProcesses().begin(), timeProcesses().end(), timeProcess));
  mStartEvent->removeState(timeProcess->getStartState());
  mEndEvent->removeState(timeProcess->getEndState());
  
  JamomaTimeProcess* t = dynamic_cast<JamomaTimeProcess*>(timeProcess.get());
  t->mParent = nullptr;
}