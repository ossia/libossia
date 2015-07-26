#include "Editor/JamomaScenario.h"

#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

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
  
  // process the scenario from the beginning
  shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mTimeNodes[0]);
  n->process();
  
  // get the state of newly HAPPENED TimeEvents
  for (const auto& timeNode : mTimeNodes)
  {
    for (const auto& timeEvent : timeNode->timeEvents())
    {
      ;//! \todo how to know that an event is newly HAPPENED ?
    }
  }
  
  // get the state of each running TimeConstraint
  for (const auto& timeConstraint : mTimeContraints)
  {
    if (timeConstraint->getRunning())
    {
      mCurrentState->stateElements().push_back(timeConstraint->state(timeConstraint->getPosition(), timeConstraint->getDate()));
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

# pragma mark -
# pragma mark Implementation specific

void JamomaScenario::init(const TimeValue& position, const TimeValue& date)
{
  // reset TimeEvent's status to NONE status
  // but set the first TimeNode TimeEvent's status to WAITING status
  for (const auto& timeNode : mTimeNodes)
  {
    for (auto& timeEvent : timeNode->timeEvents())
    {
      shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(timeEvent);
      e->setStatus(timeNode == mTimeNodes[0] ? TimeEvent::Status::PENDING : TimeEvent::Status::NONE);
    }
  }
  
/*
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
*/
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