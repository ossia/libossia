#include "Editor/JamomaAutomation.h"

#include <iostream> //! \todo to remove. only here for debug purpose

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

Automation::~Automation()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaAutomation::state(const TimeValue& position, const TimeValue& date)
{
  if (date != mLastDate)
  {
    // clear the former Value
    if (mValueToSend) delete mValueToSend;

    // compute a new value from the Curves
    mValueToSend = computeValue(position, mDrive);

    // edit a Message handling the new Value
    mMessageToSend = Message::create(mDrivenAddress, mValueToSend);

    mLastDate = date;
  }

  return mMessageToSend;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaAutomation::start()
{}

void JamomaAutomation::stop()
{}

void JamomaAutomation::pause()
{}

void JamomaAutomation::resume()
{}

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

Value* JamomaAutomation::computeValue(double position, const Value* drive)
{
  switch (drive->getType())
  {
    case Value::Type::BEHAVIOR :
    {
      auto b = static_cast<const Behavior*>(drive);

      try
      {
        Curve<double, bool>* curve = dynamic_cast<Curve<double, bool>*>(b->value.get());
        if (curve)
          return new Bool(curve->valueAt(position));
      }
      catch (std::bad_cast e) {};

      try
      {
        Curve<double, int>* curve = dynamic_cast<Curve<double, int>*>(b->value.get());
        if (curve)
          return new Int(curve->valueAt(position));
      }
      catch (std::bad_cast e) {};

      try
      {
        Curve<double, float>* curve = dynamic_cast<Curve<double, float>*>(b->value.get());
        if (curve)
          return new Float(curve->valueAt(position));
      }
      catch (std::bad_cast e) {};
      
      throw runtime_error("none handled drive curve type");
    }

    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(drive);
      vector<const Value*> t_value;

      for (const auto & e : t->value)
      {
        t_value.push_back(computeValue(position, e));
      }

      return new Tuple(t_value);
    }

    default :
    {
      throw runtime_error("none handled drive value type");
    }
  }

  return nullptr;
}
