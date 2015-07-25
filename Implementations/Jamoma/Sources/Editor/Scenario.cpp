#include "Editor/JamomaClock.h"
#include "Editor/JamomaScenario.h"
#include "Editor/JamomaTimeConstraint.h"
#include "Editor/JamomaTimeEvent.h"

#include <iostream> //! \todo to remove. only here for debug purpose
#include <algorithm>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Scenario> Scenario::create(shared_ptr<State> startState,
                                        shared_ptr<State> endState)
  {
    return make_shared<JamomaScenario>(startState, endState);
  }
}

JamomaScenario::JamomaScenario(shared_ptr<State> startState,
                               shared_ptr<State> endState) :
JamomaTimeProcess(startState, endState),
mKiller(false),
mInitialized(false)
{
  // create the start and the end TimeNodes
  mTimeNodes.push_back(TimeNode::create());
  mTimeNodes.push_back(TimeNode::create());
}

JamomaScenario::JamomaScenario(const JamomaScenario * other) :
JamomaTimeProcess(other->mStartState, other->mEndState)
{}

shared_ptr<Scenario> JamomaScenario::clone() const
{
  return make_shared<JamomaScenario>(this);
}

JamomaScenario::~JamomaScenario()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<State> JamomaScenario::state(const TimeValue& position, const TimeValue& date)
{
  if (!mInitialized)
    init();
  
  // reset internal State
  mCurrentState->stateElements().clear();
  
  // process each TimeNode to find events to make happen
  vector<shared_ptr<TimeEvent>> eventsToHappen;
  for (const auto& timeNode : mTimeNodes)
  {
    TimeValue d = timeNode->getDate();
    
    for (auto& timeEvent : timeNode->timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      TimeEvent::Status status = timeEvent->getStatus();
      
      // HAPPEND or DISPOSED TimeEvents are ignored
      if (status == TimeEvent::Status::HAPPENED || status == TimeEvent::Status::DISPOSED)
        continue;
      
      // if the TimeEvent has no previous TimeConstraints
      if (timeEvent->previousTimeConstraints().size() == 0)
      {
        // for WAITING TimeEvent
        if (status == TimeEvent::Status::WAITING)
        {
          // evaluate expression if there is one
          if (timeEvent->getExpression() != nullptr)
          {
            if (timeEvent->getExpression()->evaluate())
            {
              eventsToHappen.push_back(timeEvent);
            }
          }
          // or if it is in the past
          else if (date > d)
          {
            eventsToHappen.push_back(timeEvent);
          }
        }
      }
      // if the TimeEvent has previous TimeConstraints
      else
      {
        // process each TimeConstraint to if it is ...?
        ;
      }
    }
  }
  
  // for each TimeEvent to make happen
  for (auto& timeEvent : eventsToHappen)
  {
    shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
    
    mCurrentState->stateElements().push_back(e->getState());
    e->setStatus(TimeEvent::Status::HAPPENED);
  }
  
  // process each running TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint->getClock());
    
    if (clock->getRunning())
    {
      if (clock->getExternal())
      {
        if (clock->tick())
          mCurrentState->stateElements().push_back(timeConstraint->state(clock->getPosition(), clock->getDate()));
      }
      else
        mCurrentState->stateElements().push_back(timeConstraint->state(position, date));
    }
  }
  
  return mCurrentState;
}

# pragma mark -
# pragma mark Edition

void JamomaScenario::addTimeConstraint(const shared_ptr<TimeConstraint> timeConstraint)
{
  // store the TimeConstraint if it is not already stored
  if (std::find(mTimeContraints.begin(),
           mTimeContraints.end(),
           timeConstraint) == mTimeContraints.end())
  {
    mTimeContraints.push_back(timeConstraint);
  }

  // store TimeConstraint's start node if it is not already stored
  addTimeNode(timeConstraint->getStartEvent()->getTimeNode());

  // store TimeConstraint's end node if it is not already stored
  addTimeNode(timeConstraint->getEndEvent()->getTimeNode());
  
  // set the TimeConstraint's clock in external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint->getClock());
  clock->setExternal(true);
}

void JamomaScenario::removeTimeConstraint(const shared_ptr<TimeConstraint> timeConstraint)
{
  mTimeContraints.erase(find(mTimeContraints.begin(), mTimeContraints.end(), timeConstraint));
  
  // set the TimeConstraint's clock in none external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint->getClock());
  clock->setExternal(false);
}

void JamomaScenario::addTimeNode(const shared_ptr<TimeNode> timeNode)
{
  // store a TimeNode if it is not already stored
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
  return mKiller;
}

void JamomaScenario::setKiller(bool isKiller)
{
  mKiller = isKiller;
}

const shared_ptr<TimeNode> & JamomaScenario::getStartNode() const
{
  return mTimeNodes[0];
}

const shared_ptr<TimeNode> & JamomaScenario::getEndNode() const
{
  return mTimeNodes[1];
}

const shared_ptr<State> & JamomaScenario::getStartState() const
{
  return mStartState;
}

const shared_ptr<State> & JamomaScenario::getEndState() const
{
  return mEndState;
}

const shared_ptr<TimeConstraint> & JamomaScenario::getParentTimeConstraint() const
{
  return mParent;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaScenario::init()
{
  // reset internal State
  mCurrentState->stateElements().clear();
  
  // reset TimeEvent's status and build state
  for (const auto& timeNode : mTimeNodes)
  {
    TimeEvent::Status status = timeNode->getDate() < mParent->getClock()->getOffset() ?
    TimeEvent::Status::HAPPENED : TimeEvent::Status::WAITING;
    
    for (auto& timeEvent : timeNode->timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      e->setStatus(status);
      
      if (status == TimeEvent::Status::HAPPENED)
        mCurrentState->stateElements().push_back(e->getState());
    }
  }
  
  // activate TimeConstraint's clock
  for (const auto& timeConstraint : mTimeContraints)
  {
    TimeValue offset = Zero;
    TimeEvent::Status startStatus = timeConstraint->getStartEvent()->getStatus();
    TimeEvent::Status endStatus = timeConstraint->getEndEvent()->getStatus();
    
    if (startStatus == TimeEvent::Status::HAPPENED &&
        endStatus == TimeEvent::Status::WAITING)
    {
      offset = mParent->getClock()->getOffset() - timeConstraint->getStartEvent()->getTimeNode()->getDate();
    }
    
    timeConstraint->getClock()->setOffset(offset);
    
    // activate start node
    timeConstraint->getStartEvent()->getTimeNode()->play();
  }
  
  mInitialized = true;
}
