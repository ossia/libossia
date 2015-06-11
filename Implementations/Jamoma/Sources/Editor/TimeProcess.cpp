#include "Editor/JamomaTimeProcess.h"

# pragma mark -
# pragma mark Navigation

const shared_ptr<TimeEvent> & JamomaTimeProcess::getStartEvent() const
{
  return m_startEvent;
}

const shared_ptr<TimeEvent> & JamomaTimeProcess::getEndEvent() const
{
  return m_endEvent;
}

# pragma mark -
# pragma mark Accessors

TimeValue JamomaTimeProcess::getLength() const
{
  return TimeValue();
}

void JamomaTimeProcess::setLength(TimeValue)
{}

const shared_ptr<State> & JamomaTimeProcess::getStartState() const
{
  return state;
}

void JamomaTimeProcess::setStartState(shared_ptr<State>)
{}

const shared_ptr<State> & JamomaTimeProcess::getEndState() const
{
  return state;
}

void JamomaTimeProcess::setEndState(shared_ptr<State>)
{}

const shared_ptr<Clock> & JamomaTimeProcess::getClock() const
{
  return clock;
}

void JamomaTimeProcess::setClock(shared_ptr<Clock>)
{}
