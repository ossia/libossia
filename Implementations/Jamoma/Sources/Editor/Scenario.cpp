#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<Scenario> Scenario::create(shared_ptr<TimeConstraint> parentConstraint, shared_ptr<Clock> clock)
{
  return make_shared<JamomaScenario>(parentConstraint, clock);
}

JamomaScenario::JamomaScenario(shared_ptr<TimeConstraint> parentConstraint, shared_ptr<Clock> clock) :
mParentConstraint(parentConstraint),
mClock(clock)
{
  // create the start and the end time nodes
  mTimeNodes.push_back(TimeNode::create());
  mTimeNodes.push_back(TimeNode::create());
}

JamomaScenario::JamomaScenario(const JamomaScenario * other)
{}

JamomaScenario::~JamomaScenario()
{}

shared_ptr<Scenario> JamomaScenario::clone() const
{
  return make_shared<JamomaScenario>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaScenario::play(bool log, string name) const
{
  /* PROCESS START
  
  // reset all events to waiting status
  for (mTimeEvents.begin(); mTimeEvents.end(); mTimeEvents.next())
  {
    TTObject aTimeEvent = mTimeEvents.current()[0];
    aTimeEvent.set("status", kTTSym_eventWaiting);
  }
  
  // sort events in 2 lists depending of their time process position relative to the time offset
  TTValue v;
  mClock.get(kTTSym_offset, v);
  TTUInt32 timeOffset = v[0];
  
  TTList eventsToSetHappened;
  TTList eventsToRequestHappen;
  
  for (mTimeProcesses.begin(); mTimeProcesses.end(); mTimeProcesses.next())
  {
    TTObject aTimeProcess = mTimeProcesses.current()[0];
    
    TTObject startEvent = getTimeProcessStartEvent(aTimeProcess);
    TTObject endEvent = getTimeProcessEndEvent(aTimeProcess);
    
    TTUInt32 startEventDate;
    TTUInt32 endEventDate;
    
    startEvent.get("date", startEventDate);
    endEvent.get("date", endEventDate);
    
    if (startEventDate < timeOffset &&
        endEventDate < timeOffset)
    {
      // if the start event is not already into the other list
      if (eventsToRequestHappen.findEquals(startEvent, v))
        eventsToSetHappened.appendUnique(startEvent);
    }
    else if (startEventDate < timeOffset &&
             endEventDate > timeOffset)
    {
      eventsToRequestHappen.appendUnique(startEvent);
      eventsToSetHappened.remove(startEvent);
    }
  }
  
  for (eventsToSetHappened.begin(); eventsToSetHappened.end(); eventsToSetHappened.next())
  {
    TTObject event = eventsToSetHappened.current()[0];
    event.set("status", kTTSym_eventHappened);
  }
  
  for (eventsToRequestHappen.begin(); eventsToRequestHappen.end(); eventsToRequestHappen.next())
  {
    TTObject event = eventsToRequestHappen.current()[0];
    event.send(kTTSym_Happen);
  }
  
  */
}

# pragma mark -
# pragma mark Edition

void JamomaScenario::addConstraint(const shared_ptr<TimeConstraint> constraint)
{
  // store the constraint if it is not already stored
  if (find(mTimeContraints.begin(),
           mTimeContraints.end(),
           constraint) == mTimeContraints.end())
  {
    mTimeContraints.push_back(constraint);
  }
  
  // store constraint's start node if it is not already stored
  addTimeNode(constraint->getStartEvent()->getTimeNode());
  
  // store constraint's end node if it is not already stored
  addTimeNode(constraint->getEndEvent()->getTimeNode());
}

void JamomaScenario::removeConstraint(const shared_ptr<TimeConstraint> constraint)
{
  mTimeContraints.erase(find(mTimeContraints.begin(), mTimeContraints.end(), constraint));
}

void JamomaScenario::addTimeNode(const shared_ptr<TimeNode> timeNode)
{
  // store a time node if it is not already stored
  if (find(mTimeNodes.begin(),
           mTimeNodes.end(),
           timeNode) == mTimeNodes.end())
  {
    mTimeNodes.push_back(timeNode);
  }
}

void JamomaScenario::removeTimeNode(const shared_ptr<TimeNode> timeNode)
{
  mTimeNodes.erase(find(mTimeNodes.begin(), mTimeNodes.end(), timeNode));
}

# pragma mark -
# pragma mark Accessors

bool JamomaScenario::isKiller() const
{
  return mIsKiller;
}

void JamomaScenario::setKiller(bool isKiller)
{
  mIsKiller = isKiller;
}

const shared_ptr<TimeNode> & JamomaScenario::getStartNode() const
{
  return mTimeNodes[0];
}

const shared_ptr<TimeNode> & JamomaScenario::getEndNode() const
{
  return mTimeNodes[1];
}

const shared_ptr<TimeConstraint> & JamomaScenario::getParentTimeConstraint() const
{
  return mParentConstraint;
}

const shared_ptr<State> & JamomaScenario::getStartState() const
{
  
}

const shared_ptr<State> & JamomaScenario::getEndState() const
{
  
}

const shared_ptr<Clock> & JamomaScenario::getClock() const
{
    return mClock;
}
