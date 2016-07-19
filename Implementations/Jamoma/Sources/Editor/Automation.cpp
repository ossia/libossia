#include "Editor/JamomaAutomation.h"
#include <Editor/JamomaCurve.h>

#include <iostream> //! \todo to remove. only here for debug purpose
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

namespace OSSIA
{
  shared_ptr<Automation> Automation::create(shared_ptr<Address> address,
                                            const Value* drive)
  {
    return make_shared<JamomaAutomation>(address, drive);
  }

  Automation::~Automation()
  {
  }
}

JamomaAutomation::JamomaAutomation(shared_ptr<Address> address,
                                   const Value* drive) :
JamomaTimeProcess(),
mDrivenAddress(address),
mDrive(drive->clone())
{}

JamomaAutomation::JamomaAutomation(const JamomaAutomation * other) :
JamomaTimeProcess()
{}

shared_ptr<Automation> JamomaAutomation::clone() const
{
  return make_shared<JamomaAutomation>(this);
}

JamomaAutomation::~JamomaAutomation()
{}

#if 0
# pragma mark -
# pragma mark Execution
#endif

shared_ptr<StateElement> JamomaAutomation::offset(const TimeValue& offset)
{
  auto& par = *parent;
  if (par.getRunning())
    throw runtime_error("parent time constraint is running");

  // compute a new value from the Curves
  mValueToSend = computeValue(offset / par.getDurationNominal(), *mDrive);

  // edit a Message handling the new Value
  mMessageToSend = Message::create(mDrivenAddress, *mValueToSend);

  return mMessageToSend;
}

shared_ptr<StateElement> JamomaAutomation::state()
{
  auto& par = *parent;
  if (par.getRunning())
  {
    // if date hasn't been processed already
    TimeValue date = par.getDate();
    if (date != mLastDate)
    {
      mLastDate = date;

      // compute a new value from the Curves
      mValueToSend = computeValue(par.getPosition(), *mDrive);

      // edit a Message handling the new Value
      mMessageToSend = Message::create(mDrivenAddress, *mValueToSend);
    }

    return mMessageToSend;
  }
  else
  {
    throw runtime_error("parent time constraint is not running");
  }
}
#if 0
# pragma mark -
# pragma mark Execution - Implementation specific
#endif

void JamomaAutomation::start()
{}

void JamomaAutomation::stop()
{}

void JamomaAutomation::pause()
{}

void JamomaAutomation::resume()
{}
#if 0
# pragma mark -
# pragma mark Accessors
#endif
const shared_ptr<Address> JamomaAutomation::getDrivenAddress() const
{
  return mDrivenAddress;
}

const Value * JamomaAutomation::getDriving() const
{
  return mDrive.get();
}

std::unique_ptr<OSSIA::Value> JamomaAutomation::computeValue(double position, const Value& drive)
{
  switch (drive.getType())
  {
    case Type::BEHAVIOR :
    {
      auto& b = static_cast<const Behavior&>(drive);

      auto base_curve = b.value.get();
      auto t = base_curve->getType();
      if(t.first == OSSIA::CurveSegmentType::DOUBLE)
      {
          switch(t.second)
          {
              case OSSIA::CurveSegmentType::FLOAT:
              {
                  auto curve = static_cast<JamomaCurve<double, float>*>(base_curve);
                  return std::make_unique<Float>(curve->valueAt(position));
              }
              case OSSIA::CurveSegmentType::INT:
              {
                  auto curve = static_cast<JamomaCurve<double, int>*>(base_curve);
                  return std::make_unique<Int>(curve->valueAt(position));
              }
              case OSSIA::CurveSegmentType::BOOL:
              {
                  auto curve = static_cast<JamomaCurve<double, bool>*>(base_curve);
                  return std::make_unique<Bool>(curve->valueAt(position));
              }
          }
      }

      throw runtime_error("none handled drive curve type");
    }

    case Type::TUPLE :
    {
      auto& t = static_cast<const Tuple&>(drive);
      vector<const Value*> t_value;

      for (const auto & e : t.value)
      {
        t_value.push_back(computeValue(position, *e).release());
      }

      return std::make_unique<Tuple>(t_value);
    }

    default :
    {
      throw runtime_error("none handled drive value type");
    }
  }

  return nullptr;
}
