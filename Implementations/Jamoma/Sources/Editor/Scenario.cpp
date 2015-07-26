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
  // reset internal State
  mCurrentState->stateElements().clear();
  
  if (!mInitialized)
    init(position, date);
  
  // process each TimeNode to find events to make happen
  vector<shared_ptr<TimeEvent>> eventsToHappen;
  for (const auto& timeNode : mTimeNodes)
  {
    TimeValue timeNodeDate = timeNode->getDate();
    
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
          else if (date > timeNodeDate)
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
    shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint);
    
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
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint);
  clock->setExternal(true);
}

void JamomaScenario::removeTimeConstraint(const shared_ptr<TimeConstraint> timeConstraint)
{
  mTimeContraints.erase(find(mTimeContraints.begin(), mTimeContraints.end(), timeConstraint));
  
  // set the TimeConstraint's clock in none external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(timeConstraint);
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

void JamomaScenario::init(const TimeValue& position, const TimeValue& date)
{
  // reset TimeEvent's status and prepare the state at this date
  for (const auto& timeNode : mTimeNodes)
  {
    TimeEvent::Status status = timeNode->getDate() < date ?
    TimeEvent::Status::HAPPENED : TimeEvent::Status::WAITING;
    
    //! \todo what about PENDING status ?
    //! \todo what about DISPOSED status ?
    
    for (auto& timeEvent : timeNode->timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      e->setStatus(status);
      
      //! \todo we shouldn't take all events because some of them could be conccurent
      //! if they are on two different branches separated by some expressions
      if (status == TimeEvent::Status::HAPPENED)
      {
        flattenState(e->getState());
      }
    }
  }
  
  // reset TimeConstraint's status and setup the clock
  for (const auto& timeConstraint : mTimeContraints)
  {
    TimeEvent::Status startStatus = timeConstraint->getStartEvent()->getStatus();
    TimeEvent::Status endStatus = timeConstraint->getEndEvent()->getStatus();
    
    if (startStatus == TimeEvent::Status::WAITING &&
        endStatus == TimeEvent::Status::WAITING)
    {
      //! \todo set the TimeConstraint's status to WAITING ?
    }
    else if (startStatus == TimeEvent::Status::HAPPENED &&
        endStatus == TimeEvent::Status::WAITING)
    {
      TimeValue offset = date - timeConstraint->getStartEvent()->getTimeNode()->getDate();
      timeConstraint->setOffset(offset);
      
      //! \todo set the TimeConstraint's status to STARTED ?
    }
    else if (startStatus == TimeEvent::Status::HAPPENED &&
             endStatus == TimeEvent::Status::HAPPENED)
    {
      //! \todo set the TimeConstraint's status to ENDED ?
    }
    
    //! \todo what about DISPOSED status ?
  }
  
  mInitialized = true;
}

void JamomaScenario::flattenState(const shared_ptr<State> stateToFlatten)
{
  for (const auto& element : stateToFlatten->stateElements())
  {
    switch (element->getType())
    {
      case StateElement::Type::MESSAGE :
      {
        shared_ptr<Message> messageToAppend = dynamic_pointer_cast<Message>(element);
        
        // find message with the same address
        for (auto it = mCurrentState->stateElements().begin();
             it != mCurrentState->stateElements().end();
             it++)
        {
          shared_ptr<Message> messageToCheck = dynamic_pointer_cast<Message>(*it);
          
          // replace if addresses are the same
          if (messageToCheck->getAddress() == messageToAppend->getAddress())
          {
            *it = element;
            break;
          }
        }
        
        break;
      }
      case StateElement::Type::STATE :
      {
        flattenState(dynamic_pointer_cast<State>(element));
        break;
      }
    }
  }
}