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
  JamomaTimeProcess(),
  mDriverAddress(driverAddress),
  mDrivenAddress(drivenAddress),
  mDrive(drive->clone()),
  mDriverValueObserved(false)
{}

JamomaMapper::JamomaMapper(const JamomaMapper * other) :
  JamomaTimeProcess()
{}

shared_ptr<Mapper> JamomaMapper::clone() const
{
  return make_shared<JamomaMapper>(this);
}

JamomaMapper::~JamomaMapper()
{}

Mapper::~Mapper()
{}

# pragma mark -
# pragma mark Execution

shared_ptr<StateElement> JamomaMapper::offset(const TimeValue& offset)
{
  if (parent->getRunning())
    throw runtime_error("parent time constraint is running");

  //! \todo return nothing
  return mMessageToSend;
}

shared_ptr<StateElement> JamomaMapper::state()
{
  auto& par = *parent;
  if (!par.getRunning())
    throw runtime_error("parent time constraint is not running");

  // if date hasn't been processed already
  TimeValue date = par.getDate();
  if (date != mLastDate)
  {
    mLastDate = date;

    if (mValueToMap)
    {
      std::lock_guard<std::mutex> lock(mValueToMapMutex);

      // edit a Message handling the mapped value
      auto newval = computeValue(*mValueToMap, *mDrive);
      mMessageToSend = Message::create(mDrivenAddress, *newval);

      // forget the former value
      delete mValueToMap;
      mValueToMap = nullptr;
    }
  }

  return mMessageToSend;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaMapper::start()
{
  // start driver address value observation
  if (!mDriverValueObserved)
  {
    mDriverValueCallbackIndex = mDriverAddress->addCallback(
          [this] (const OSSIA::Value& val) {
      driverValueCallback(val);
    });
    mDriverValueObserved = true;
    auto def_val = mDriverAddress->cloneValue();
    driverValueCallback(*def_val);
  }
}

void JamomaMapper::stop()
{
  // stop driver address value observation
  if (mDriverValueObserved)
  {
    mDriverAddress->removeCallback(mDriverValueCallbackIndex);
    mDriverValueObserved = false;
  }
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

std::unique_ptr<OSSIA::Value> JamomaMapper::computeValue(const Value& driver, const Value& drive)
{
  switch (driver.getType())
  {
    case Value::Type::BEHAVIOR :
    {
      auto& b = static_cast<const Behavior&>(drive);


      auto base_curve = b.value.get();
      auto t = base_curve->getType();
      switch(t.first)
      {
        case OSSIA::CurveSegmentType::FLOAT:
        {
          auto& val = static_cast<const Float&>(driver);
          switch(t.second)
          {
            case OSSIA::CurveSegmentType::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<float, float>*>(base_curve);
              return std::make_unique<Float>(curve->valueAt(val.value));
            }
            case OSSIA::CurveSegmentType::INT:
            {
              auto curve = static_cast<JamomaCurve<float, int>*>(base_curve);
              return std::make_unique<Int>(curve->valueAt(val.value));
            }
            case OSSIA::CurveSegmentType::BOOL:
            {
              auto curve = static_cast<JamomaCurve<float, bool>*>(base_curve);
              return std::make_unique<Bool>(curve->valueAt(val.value));
            }
          }
        }
        case OSSIA::CurveSegmentType::INT:
        {
          auto& val = static_cast<const Int&>(driver);
          switch(t.second)
          {
            case OSSIA::CurveSegmentType::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<int, float>*>(base_curve);
              return std::make_unique<Float>(curve->valueAt(val.value));
            }
            case OSSIA::CurveSegmentType::INT:
            {
              auto curve = static_cast<JamomaCurve<int, int>*>(base_curve);
              return std::make_unique<Int>(curve->valueAt(val.value));
            }
            case OSSIA::CurveSegmentType::BOOL:
            {
              auto curve = static_cast<JamomaCurve<int, bool>*>(base_curve);
              return std::make_unique<Bool>(curve->valueAt(val.value));
            }
          }
        }
        case OSSIA::CurveSegmentType::BOOL:
        {
          auto& val = static_cast<const Bool&>(driver);
          switch(t.second)
          {
            case OSSIA::CurveSegmentType::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<bool, float>*>(base_curve);
              return std::make_unique<Float>(curve->valueAt(val.value));
            }
            case OSSIA::CurveSegmentType::INT:
            {
              auto curve = static_cast<JamomaCurve<bool, int>*>(base_curve);
              return std::make_unique<Int>(curve->valueAt(val.value));
            }
            case OSSIA::CurveSegmentType::BOOL:
            {
              auto curve = static_cast<JamomaCurve<bool, bool>*>(base_curve);
              return std::make_unique<Bool>(curve->valueAt(val.value));
            }
          }
        }
      }
      throw runtime_error("none handled driver value type");

      break;
    }

    case Value::Type::TUPLE :
    {
      auto& t_drive = static_cast<const Tuple&>(drive);

      if (driver.getType() == Value::Type::TUPLE)
      {
        auto& t_driver = static_cast<const Tuple&>(driver);

        vector<const Value*> t_value;
        vector<Value*>::const_iterator it_driver = t_driver.value.begin();

        for (const auto & e_drive : t_drive.value)
        {
          if (it_driver == t_driver.value.end())
            break;

          t_value.push_back(computeValue(**it_driver, *e_drive).release());
          it_driver++;
        }

        return std::make_unique<Tuple>(t_value);
      }
    }

    default :
    {
      throw runtime_error("none handled drive value type");
    }
  }

  return nullptr;
}

void JamomaMapper::driverValueCallback(const Value& value)
{
  std::lock_guard<std::mutex> lock(mValueToMapMutex);

  // clear the former Value
  if (mValueToMap)
  {
    delete mValueToMap;
    mValueToMap = nullptr;
  }

  // clone the new value
  mValueToMap = value.clone();
}
