#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<TimeConstraint> TimeConstraint::create(TimeValue nominal, shared_ptr<TimeEvent> startEvent, shared_ptr<TimeEvent> endEvent, TimeValue min, TimeValue max)
{
  return make_shared<JamomaTimeConstraint>(nominal, startEvent, endEvent, min, max);
}

JamomaTimeConstraint::JamomaTimeConstraint(TimeValue nominal, shared_ptr<TimeEvent> startEvent, shared_ptr<TimeEvent> endEvent, TimeValue min, TimeValue max) :
mDuration(nominal),
mDurationMin(min),
mDurationMax(max),
mStartEvent(startEvent),
mEndEvent(endEvent)
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
{}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeEvent> & JamomaTimeConstraint::getStartEvent() const
{
  return mStartEvent;
}

const shared_ptr<TimeEvent> & JamomaTimeConstraint::getEndEvent() const
{
  return mEndEvent;
}