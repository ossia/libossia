#include "Editor/JamomaLoop.h"

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Loop> Loop::create()
  {
    return make_shared<JamomaLoop>();
  }
}

JamomaLoop::JamomaLoop() :
JamomaTimeProcess(State::create(), State::create())
{
  mPatternStartNode = TimeNode::create();
  mPatternStartNode->emplace(mPatternStartNode->timeEvents().begin(), std::bind(&JamomaLoop::PatternStartEventCallback, this, _1));
  
  mPatternEndNode = TimeNode::create();
  mPatternEndNode->emplace(mPatternEndNode->timeEvents().begin(), std::bind(&JamomaLoop::PatternEndEventCallback, this, _1));
  
  mPatternConstraint = TimeConstraint::create(std::bind(&JamomaLoop::PatternConstraintCallback, this, _1, _2, _3),
                                              mPatternStartNode->timeEvents()[0],
                                              mPatternEndNode->timeEvents()[0]);
  
  // set pattern TimeConstraint's clock in external mode
  shared_ptr<JamomaClock> clock = dynamic_pointer_cast<JamomaClock>(mPatternConstraint);
  clock->setDriveMode(Clock::DriveMode::EXTERNAL);
  
  mCurrentState = State::create();
}

JamomaLoop::JamomaLoop(const JamomaLoop * other) :
JamomaTimeProcess(other->mStartState, other->mEndState),
mPatternStartNode(other->mPatternStartNode->clone()),
mPatternEndNode(other->mPatternEndNode->clone()),
mPatternConstraint(other->mPatternConstraint->clone())
{}

shared_ptr<Loop> JamomaLoop::clone() const
{
  return make_shared<JamomaLoop>(this);
}

JamomaLoop::~JamomaLoop()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaLoop::state(const TimeValue& position, const TimeValue& date)
{
  // reset internal State
  mCurrentState->stateElements().clear();
  
  // if the time goes backward
  if (position < mLastPosition)
  {
    TimeValue offset = std::fmod((double)date, (double)mPatternConstraint->getDuration());
    
    mPatternStartNode->setup(offset);
    mPatternEndNode->setup(offset);
    
    if (mPatternStartNode->timeEvents()[0]->getStatus() == TimeEvent::Status::HAPPENED)
      flattenAndFilter(mPatternStartNode->timeEvents()[0]->getState());
    
    mPatternConstraint->setup(offset);
  }
  
  // if position hasn't been processed already
  if (position != mLastPosition)
  {
    mLastPosition = position;
    
    if (mPatternEndNode->timeEvents()[0]->getStatus() == TimeEvent::Status::HAPPENED)
    {
      flattenAndFilter(mPatternEndNode->timeEvents()[0]->getState());
      
      mPatternStartNode->setup(Zero);
      mPatternEndNode->setup(Zero);
      mPatternConstraint->setup(Zero);
    }
    
    mPatternConstraint->tick();
  }

  //! \see JamomaLoop::PatternConstraintCallback below to understand how mCurrentState is filled
  return mCurrentState;
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeConstraint> JamomaLoop::getPatternTimeConstraint() const
{
  return mPatternConstraint;
}

const shared_ptr<TimeNode> JamomaLoop::getPatternStartTimeNode() const
{
  return mPatternStartNode;
}

const shared_ptr<TimeNode> JamomaLoop::getPatternEndTimeNode() const
{
  return mPatternEndNode;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaLoop::flattenAndFilter(const shared_ptr<StateElement> element)
{
  switch (element->getType())
  {
    case StateElement::Type::MESSAGE :
    {
      shared_ptr<Message> messageToAppend = dynamic_pointer_cast<Message>(element);
      
      // find message with the same address to replace it
      bool found = false;
      for (auto it = mCurrentState->stateElements().begin();
           it != mCurrentState->stateElements().end();
           it++)
      {
        shared_ptr<Message> messageToCheck = dynamic_pointer_cast<Message>(*it);
        
        // replace if addresses are the same
        if (messageToCheck->getAddress() == messageToAppend->getAddress())
        {
          *it = element;
          found = true;
          break;
        }
      }
      
      // if not found append it
      if (!found)
        mCurrentState->stateElements().push_back(element);
      
      break;
    }
    case StateElement::Type::STATE :
    {
      shared_ptr<State> stateToFlatAndFilter = dynamic_pointer_cast<State>(element);
      
      for (const auto& e : stateToFlatAndFilter->stateElements())
      {
        flattenAndFilter(e);
      }
      break;
    }
  }
}

void JamomaLoop::PatternConstraintCallback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> state)
{
  // add the state of the pattern TimeConstraint
  flattenAndFilter(mPatternConstraint->state(position, date));
}

void JamomaLoop::PatternStartEventCallback(TimeEvent::Status newStatus)
{
  //! \debug
  switch (newStatus)
  {
    case TimeEvent::Status::NONE:
    {
      cout << "Start Pattern Event NONE" << endl;
      break;
    }
    case TimeEvent::Status::PENDING:
    {
      cout << "Start Pattern Event PENDING" << endl;
      break;
    }
    case TimeEvent::Status::HAPPENED:
    {
      cout << "Start Pattern Event HAPPENED" << endl;
      break;
    }
    case TimeEvent::Status::DISPOSED:
    {
      cout << "Start Pattern Event DISPOSED" << endl;
      break;
    }
  }
}

void JamomaLoop::PatternEndEventCallback(TimeEvent::Status newStatus)
{
  //! \debug
  switch (newStatus)
  {
    case TimeEvent::Status::NONE:
    {
      cout << "End Pattern Event NONE" << endl;
      break;
    }
    case TimeEvent::Status::PENDING:
    {
      cout << "End Pattern Event PENDING" << endl;
      break;
    }
    case TimeEvent::Status::HAPPENED:
    {
      cout << "End Pattern Event HAPPENED" << endl;
      break;
    }
    case TimeEvent::Status::DISPOSED:
    {
      cout << "End Pattern Event DISPOSED" << endl;
      break;
    }
  }
}