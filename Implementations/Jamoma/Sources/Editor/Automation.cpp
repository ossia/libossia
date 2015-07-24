#include "Editor/JamomaAutomation.h"

#include <assert.h>
#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Automation> Automation::create(TimeProcess::ExecutionCallback callback,
                                            shared_ptr<Address> address,
                                            const Value* drive)
  {
    return make_shared<JamomaAutomation>(callback, address, drive);
  }
}

JamomaAutomation::JamomaAutomation(TimeProcess::ExecutionCallback callback,
                                   shared_ptr<Address> address,
                                   const Value* drive) :
JamomaTimeProcess(callback, State::create(), State::create()),
mDrivenAddress(address),
mDrive(drive->clone())
{
  // pass callback to the Clock
  Clock::ExecutionCallback clockCallback = std::bind(&JamomaAutomation::ClockCallback, this, _1, _2, _3);
  mClock->setExecutionCallback(clockCallback);
}

JamomaAutomation::JamomaAutomation(const JamomaAutomation * other) :
JamomaTimeProcess(other->mCallback, other->mStartState, other->mEndState)
{}

shared_ptr<Automation> JamomaAutomation::clone() const
{
  return make_shared<JamomaAutomation>(this);
}

JamomaAutomation::~JamomaAutomation()
{}

# pragma mark -
# pragma mark Execution

void JamomaAutomation::play(bool log, string name) const
{
  assert(mParent != nullptr);

  // setup clock duration with parent constraint duration
  mClock->setDuration(mParent->getDuration());
  
  mClock->go();
}

void JamomaAutomation::stop() const
{
  mClock->stop();
}

void JamomaAutomation::pause() const
{
  mClock->pause();
}

void JamomaAutomation::resume() const
{
  mClock->resume();
}

shared_ptr<State> JamomaAutomation::state() const
{
  return mCurrentState;
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<Address> JamomaAutomation::getDrivenAddress() const
{
  return mDrivenAddress;
}

const Value * JamomaAutomation::getDriving() const
{
  return mDrive;
}

const shared_ptr<State> & JamomaAutomation::getStartState() const
{
  return mStartState;
}

const shared_ptr<State> & JamomaAutomation::getEndState() const
{
  return mEndState;
}

const shared_ptr<Clock> & JamomaAutomation::getClock() const
{
  return mClock;
}

const shared_ptr<TimeConstraint> & JamomaAutomation::getParentTimeConstraint() const
{
  return mParent;
}

void JamomaAutomation::ClockCallback(const TimeValue& position, const TimeValue& date, unsigned char droppedTicks)
{
  // clear internal State, Message and Value
  mCurrentState->stateElements().clear();
  if (mValueToSend) delete mValueToSend;
  
  // compute a new value from the Curves
  mValueToSend = computeValueAtPosition(mDrive, position);
  
  // fill internal State with a Message handling the Value
  mMessageToSend = Message::create(mDrivenAddress, mValueToSend);
  mCurrentState->stateElements().push_back(mMessageToSend);
  
  (mCallback)(position, date, mCurrentState);
}

Value* JamomaAutomation::computeValueAtPosition(const Value* drive, const TimeValue& position)
{
  switch (drive->getType())
  {
    case Value::Type::IMPULSE :
    {
      return drive->clone();
    }
      
    case Value::Type::BOOL :
    {
      return drive->clone();
    }
      
    case Value::Type::INT :
    {
      return drive->clone();
    }
      
    case Value::Type::FLOAT :
    {
      return drive->clone();
    }
      
    case Value::Type::CHAR :
    {
      return drive->clone();
    }
      
    case Value::Type::STRING :
    {
      return drive->clone();
    }
      
    case Value::Type::DESTINATION :
    {
      return drive->clone();
    }
      
    case Value::Type::BEHAVIOR :
    {
      Behavior * b = (Behavior*)drive;
      
      try
      {
        Curve<bool>* curve = dynamic_cast<Curve<bool>*>(b->value.get());
        if (curve)
          return new Bool(curve->valueAt(position));
      }
      catch (std::bad_cast e) {};
      
      try
      {
        Curve<int>* curve = dynamic_cast<Curve<int>*>(b->value.get());
        if (curve)
          return new Int(curve->valueAt(position));
      }
      catch (std::bad_cast e) {};

      try
      {
        Curve<float>* curve = dynamic_cast<Curve<float>*>(b->value.get());
        if (curve)
          return new Float(curve->valueAt(position));
      }
      catch (std::bad_cast e) {};
      
      break;
    }
      
    case Value::Type::TUPLE :
    {
      Tuple * t = (Tuple*)drive;
      vector<const Value*> t_value;
      
      for (const auto & e : t->value)
      {
        t_value.push_back(computeValueAtPosition(e, position));
      }
      
      return new Tuple(t_value);
    }
      
    case Value::Type::GENERIC :
    {
      //! \todo
      break;
    }
  }
  
  return nullptr;
}
