#include "Editor/JamomaMapper.h"


#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Mapper> Mapper::create(shared_ptr<Address> driverAddress,
                                    shared_ptr<Address> drivenAddress,
                                    const Value* drive)
  {
    return make_shared<JamomaMapper>(driverAddress, drivenAddress, drive);
  }
}

JamomaMapper::JamomaMapper(shared_ptr<Address> driverAddress,
                           shared_ptr<Address> drivenAddress,
                           const Value* drive) :
JamomaTimeProcess(State::create(), State::create()),
mDriverAddress(driverAddress),
mDrivenAddress(drivenAddress),
mDrive(drive->clone())
{}

JamomaMapper::JamomaMapper(const JamomaMapper * other) :
JamomaTimeProcess(other->mStartState, other->mEndState)
{}

shared_ptr<Mapper> JamomaMapper::clone() const
{
  return make_shared<JamomaMapper>(this);
}

JamomaMapper::~JamomaMapper()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaMapper::state(const TimeValue& position, const TimeValue& date)
{
  if (position != mLastPosition)
  {
    // clear the former Value
    if (mValueToSend) delete mValueToSend;
    
    // compute a new value
    //! \todo observe the driver address value
    //mValueToSend = computeValueAtPositions(mDrive, computePositions(mDriverAddress->pullValue()));

    // edit a Message handling the new Value
    mMessageToSend = Message::create(mDrivenAddress, mValueToSend);

    mLastPosition = position;
  }

  return mMessageToSend;
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<Address> JamomaMapper::getDriverAddress() const
{
  return mDriverAddress;
}

const shared_ptr<Address> JamomaMapper::getDrivenAddress() const
{
  return mDrivenAddress;
}

const Value * JamomaMapper::getDriving() const
{
  return mDrive;
}
/*
vector<const TimeValue> computePositions(const Value* driver)
{
  switch (driver->getType())
  {
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(driver);
      return {b->value};
    }
      
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(driver);
      return {(double)i->value};
    }
      
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(driver);
      return {f->value};
    }
      
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(driver);
      vector<const TimeValue> positions;
      
      for (const auto & e : t->value)
      {
        positions.push_back(computePositions(e)[0]);
      }
      
      return positions;
    }
      
    default :
    {
      throw runtime_error("none numerical value cannot be used as driver mapper value");
    }
  }
}

Value* JamomaMapper::computeValueAtPositions(const Value* drive, vector<const TimeValue&> positions)
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
      auto b = static_cast<const Behavior*>(drive);
      
      try
      {
        Curve<double, bool>* curve = dynamic_cast<Curve<double, bool>*>(b->value.get());
        if (curve)
          return new Bool(curve->valueAt(positions[0]));
      }
      catch (std::bad_cast e) {};
      
      try
      {
        Curve<double, int>* curve = dynamic_cast<Curve<double, int>*>(b->value.get());
        if (curve)
          return new Int(curve->valueAt(positions[0]));
      }
      catch (std::bad_cast e) {};
      
      try
      {
        Curve<double, float>* curve = dynamic_cast<Curve<double, float>*>(b->value.get());
        if (curve)
          return new Float(curve->valueAt(positions[0]));
      }
      catch (std::bad_cast e) {};
      
      break;
    }
      
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(drive);
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
*/