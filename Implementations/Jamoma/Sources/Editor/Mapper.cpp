#include "Editor/JamomaMapper.h"
#include "Editor/JamomaCurve.h"

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
    mValueToSend = computeValue(mDriverAddress->pullValue(), mDrive);
    
    // edit a Message handling the new Value
    mMessageToSend = Message::create(mDrivenAddress, mValueToSend);
    
    mLastPosition = position;
  }
  
  return mMessageToSend;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaMapper::start()
{
  //! \todo start driver address value observation
}

void JamomaMapper::stop()
{
  //! \todo stop driver address value observation
}

void JamomaMapper::pause()
{}

void JamomaMapper::resume()
{}

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

Value* JamomaMapper::computeValue(const Value* driver, const Value* drive)
{
  switch (drive->getType())
  {
    case Value::Type::BEHAVIOR :
    {
      auto behavior = static_cast<const Behavior*>(drive);
      
      switch (driver->getType())
      {
        case Value::Type::BOOL :
        {
          auto b = static_cast<const Bool*>(driver);
          
          try
          {
            Curve<bool, bool>* curve = dynamic_cast<Curve<bool, bool>*>(behavior->value.get());
            if (curve)
              return new Bool(curve->valueAt(b->value));
          }
          catch (std::bad_cast e) {};
          
          try
          {
            Curve<bool, int>* curve = dynamic_cast<Curve<bool, int>*>(behavior->value.get());
            if (curve)
              return new Int(curve->valueAt(b->value));
          }
          catch (std::bad_cast e) {};
          
          try
          {
            Curve<bool, float>* curve = dynamic_cast<Curve<bool, float>*>(behavior->value.get());
            if (curve)
              return new Float(curve->valueAt(b->value));
          }
          catch (std::bad_cast e) {};
        }
          
        case Value::Type::INT :
        {
          auto i = static_cast<const Int*>(driver);
          
          try
          {
            Curve<int, bool>* curve = dynamic_cast<Curve<int, bool>*>(behavior->value.get());
            if (curve)
              return new Bool(curve->valueAt(i->value));
          }
          catch (std::bad_cast e) {};
          
          try
          {
            Curve<int, int>* curve = dynamic_cast<Curve<int, int>*>(behavior->value.get());
            if (curve)
              return new Int(curve->valueAt(i->value));
          }
          catch (std::bad_cast e) {};
          
          try
          {
            Curve<int, float>* curve = dynamic_cast<Curve<int, float>*>(behavior->value.get());
            if (curve)
              return new Float(curve->valueAt(i->value));
          }
          catch (std::bad_cast e) {};
        }
          
        case Value::Type::FLOAT :
        {
          auto f = static_cast<const Float*>(driver);
          
          try
          {
            Curve<float, bool>* curve = dynamic_cast<Curve<float, bool>*>(behavior->value.get());
            if (curve)
              return new Bool(curve->valueAt(f->value));
          }
          catch (std::bad_cast e) {};
          
          try
          {
            Curve<float, int>* curve = dynamic_cast<Curve<float, int>*>(behavior->value.get());
            if (curve)
              return new Int(curve->valueAt(f->value));
          }
          catch (std::bad_cast e) {};
          
          try
          {
            Curve<float, float>* curve = dynamic_cast<Curve<float, float>*>(behavior->value.get());
            if (curve)
              return new Float(curve->valueAt(f->value));
          }
          catch (std::bad_cast e) {};
        }
          
        default :
        {
          throw runtime_error("none handled driver value type");
        }
      }
      
      break;
    }
      
    case Value::Type::TUPLE :
    {
      auto t_drive = static_cast<const Tuple*>(drive);
      
      if (driver->getType() == Value::Type::TUPLE)
      {
        auto t_driver = static_cast<const Tuple*>(driver);
        
        vector<const Value*> t_value;
        vector<Value*>::const_iterator it_driver = t_driver->value.begin();
        
        for (const auto & e_drive : t_drive->value)
        {
          if (it_driver == t_driver->value.end())
            break;
          
          t_value.push_back(computeValue(*it_driver, e_drive));
          it_driver++;
        }
        
        return new Tuple(t_value);
      }
    }
      
    default :
    {
      throw runtime_error("none handled drive value type");
    }
  }
  
  return nullptr;
}