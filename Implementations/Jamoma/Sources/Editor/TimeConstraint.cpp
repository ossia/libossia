#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<TimeConstraint> TimeConstraint::create(shared_ptr<TimeEvent> startEvent,
                                                  shared_ptr<TimeEvent> endEvent,
                                                  const TimeValue& nominal,
                                                  const TimeValue& min,
                                                  const TimeValue& max)
{
    auto timeConstraint = make_shared<JamomaTimeConstraint>(startEvent, endEvent, nominal, min, max);
    
    // store the TimeConstraint into the start event as a next constraint
    if (find(startEvent->nextTimeConstraints().begin(),
             startEvent->nextTimeConstraints().end(),
             timeConstraint) == startEvent->nextTimeConstraints().end())
    {
        startEvent->nextTimeConstraints().push_back(timeConstraint);
    }
    
    // store the TimeConstraint into the end event as a previous constraint
    if (find(endEvent->previousTimeConstraints().begin(),
             endEvent->previousTimeConstraints().end(),
             timeConstraint) == endEvent->previousTimeConstraints().end())
    {
        endEvent->previousTimeConstraints().push_back(timeConstraint);
    }
    
    return timeConstraint;
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
  {
    timeProcess->getClock()->setDuration(mDuration);
    timeProcess->play();
  }
}

shared_ptr<State> JamomaTimeConstraint::state(const TimeValue& position, const TimeValue& date) const
{
  shared_ptr<State> state = State::create();
  
  for (const auto & timeProcess : timeProcesses())
  {
    state->stateElements().push_back(timeProcess->state(position, date));
  }
  
  return state;
}

# pragma mark -
# pragma mark Accessors

const TimeValue & JamomaTimeConstraint::getDuration() const
{
  return mDuration;
}

const TimeValue & JamomaTimeConstraint::getDurationMin() const
{
  return mDurationMin;
}

const TimeValue & JamomaTimeConstraint::getDurationMax() const
{
  return mDurationMax;
}

const shared_ptr<TimeEvent> & JamomaTimeConstraint::getStartEvent() const
{
  return mStartEvent;
}

const shared_ptr<TimeEvent> & JamomaTimeConstraint::getEndEvent() const
{
  return mEndEvent;
}