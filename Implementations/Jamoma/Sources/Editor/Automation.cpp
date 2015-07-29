#include "Editor/JamomaAutomation.h"

#include <assert.h>
#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Automation> Automation::create(shared_ptr<Address> address,
                                            const Value* drive)
  {
    return make_shared<JamomaAutomation>(address, drive);
  }
}

JamomaAutomation::JamomaAutomation(shared_ptr<Address> address,
                                   const Value* drive) :
JamomaTimeProcess(State::create(), State::create()),
mDrivenAddress(address),
mDrive(drive->clone())
{}

JamomaAutomation::JamomaAutomation(const JamomaAutomation * other) :
JamomaTimeProcess(other->mStartState, other->mEndState)
{}

shared_ptr<Automation> JamomaAutomation::clone() const
{
  return make_shared<JamomaAutomation>(this);
}

JamomaAutomation::~JamomaAutomation()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaAutomation::state(const TimeValue& position, const TimeValue& date)
{
  if (position != mLastPosition)
  {
    // clear the former Value
    if (mValueToSend) delete mValueToSend;
    
    // compute a new value from the Curves
    mValueToSend = computeValueAtPosition(mDrive, position);
    
    // edit a Message handling the new Value
    mMessageToSend = Message::create(mDrivenAddress, mValueToSend);

    mLastPosition = position;
  }
  
  return mMessageToSend;
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
      //! \todo GENERIC case
      break;
    }
  }
  
  return nullptr;
}
