#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<TimeConstraint> TimeConstraint::create(shared_ptr<TimeEvent> startEvent,
                                                  shared_ptr<TimeEvent> endEvent,
                                                  const TimeValue& nominal,
                                                  const TimeValue& min,
                                                  const TimeValue& max)
{
    auto constraint = make_shared<JamomaTimeConstraint>(startEvent, endEvent, nominal, min, max);
    
    // store the constraint into the start event as a next constraint
    if (find(startEvent->nextTimeConstraints().begin(),
             startEvent->nextTimeConstraints().end(),
             constraint) == startEvent->nextTimeConstraints().end())
    {
        startEvent->nextTimeConstraints().push_back(constraint);
    }
    
    // store the constraint into the end event as a previous constraint
    if (find(endEvent->previousTimeConstraints().begin(),
             endEvent->previousTimeConstraints().end(),
             constraint) == endEvent->previousTimeConstraints().end())
    {
        endEvent->previousTimeConstraints().push_back(constraint);
    }
    
    return constraint;
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
  for (const auto & timeprocess : timeProcesses())
  {
    timeprocess->getClock()->setDuration(mDuration);
    timeprocess->play();
  }
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