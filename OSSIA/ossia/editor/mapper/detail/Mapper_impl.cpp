#include "Mapper_impl.hpp"
#include <ossia/editor/curve/detail/Curve_impl.hpp>
#include <iostream>
namespace impl
{

JamomaMapper::JamomaMapper(
    OSSIA::net::Address& driverAddress,
    OSSIA::net::Address& drivenAddress,
    const OSSIA::Value& drive) :
  JamomaTimeProcess(),
  mDriverAddress{driverAddress},
  mDrivenAddress{drivenAddress},
  mDrive{drive},
  mLastMessage{mDrivenAddress, {}},
  mDriverValueObserved(false)
{}

JamomaMapper::JamomaMapper(const JamomaMapper& other) :
  JamomaTimeProcess(),
  mDriverAddress{other.mDriverAddress},
  mDrivenAddress{other.mDrivenAddress},
  mDrive{other.mDrive},
  mLastMessage{other.mLastMessage},
  mDriverValueObserved{other.mDriverValueObserved}
{}

std::shared_ptr<OSSIA::Mapper> JamomaMapper::clone() const
{
  return std::make_shared<JamomaMapper>(*this);
}

JamomaMapper::~JamomaMapper()
{}

# pragma mark -
# pragma mark Execution

OSSIA::StateElement JamomaMapper::offset(OSSIA::TimeValue offset)
{
  if (parent->getRunning())
    throw std::runtime_error("parent time constraint is running");

  return {}; // TODO why not state ?
}

OSSIA::StateElement JamomaMapper::state()
{
  auto& par = *parent;
  if (!par.getRunning())
    throw std::runtime_error("parent time constraint is not running");

  // if date hasn't been processed already
  OSSIA::TimeValue date = par.getDate();
  if (date != mLastDate)
  {
    mLastDate = date;

    std::lock_guard<std::mutex> lock(mValueToMapMutex);
    if (mValueToMap.valid())
    {
      // edit a Message handling the mapped value
      auto newval = computeValue(mValueToMap, mDrive);

      // forget the former value
      mValueToMap.reset();

      mLastMessage.value = std::move(newval);
      return mLastMessage;
    }
  }

  return mLastMessage;
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaMapper::start()
{
  // start driver address value observation
  if (!mDriverValueObserved)
  {
    mDriverValueCallbackIndex = mDriverAddress.addCallback(
                                  [this] (const OSSIA::Value& val) {
      driverValueCallback(val);
    });
    mDriverValueObserved = true;
    auto def_val = mDriverAddress.cloneValue();
    driverValueCallback(def_val);
  }
}

void JamomaMapper::stop()
{
  // stop driver address value observation
  if (mDriverValueObserved)
  {
    mDriverAddress.removeCallback(mDriverValueCallbackIndex);
    mDriverValueObserved = false;
  }
}

void JamomaMapper::pause()
{}

void JamomaMapper::resume()
{}

# pragma mark -
# pragma mark Accessors

const OSSIA::net::Address& JamomaMapper::getDriverAddress() const
{
  return mDriverAddress;
}

const OSSIA::net::Address& JamomaMapper::getDrivenAddress() const
{
  return mDrivenAddress;
}

const OSSIA::Value& JamomaMapper::getDriving() const
{
  return mDrive;
}

OSSIA::Value JamomaMapper::computeValue(
    const OSSIA::Value& driver,
    const OSSIA::Value& drive)
{
  switch (drive.getType())
  {
    case OSSIA::Type::BEHAVIOR :
    {
      auto& b = drive.get<OSSIA::Behavior>();

      auto base_curve = b.value.get();
      auto t = base_curve->getType();
      switch(t.first)
      {
        case OSSIA::CurveSegmentType::FLOAT:
        {
          auto& val = driver.get<OSSIA::Float>();
          switch(t.second)
          {
            case OSSIA::CurveSegmentType::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<float, float>*>(base_curve);
              return OSSIA::Float{curve->valueAt(val.value)};
            }
            case OSSIA::CurveSegmentType::INT:
            {
              auto curve = static_cast<JamomaCurve<float, int>*>(base_curve);
              return OSSIA::Int{curve->valueAt(val.value)};
            }
            case OSSIA::CurveSegmentType::BOOL:
            {
              auto curve = static_cast<JamomaCurve<float, bool>*>(base_curve);
              return OSSIA::Bool{curve->valueAt(val.value)};
            }
          }
        }
        case OSSIA::CurveSegmentType::INT:
        {
          auto& val = driver.get<OSSIA::Int>();
          switch(t.second)
          {
            case OSSIA::CurveSegmentType::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<int, float>*>(base_curve);
              return OSSIA::Float{curve->valueAt(val.value)};
            }
            case OSSIA::CurveSegmentType::INT:
            {
              auto curve = static_cast<JamomaCurve<int, int>*>(base_curve);
              return OSSIA::Int{curve->valueAt(val.value)};
            }
            case OSSIA::CurveSegmentType::BOOL:
            {
              auto curve = static_cast<JamomaCurve<int, bool>*>(base_curve);
              return OSSIA::Bool{curve->valueAt(val.value)};
            }
          }
        }
        case OSSIA::CurveSegmentType::BOOL:
        {
          auto& val = driver.get<OSSIA::Bool>();
          switch(t.second)
          {
            case OSSIA::CurveSegmentType::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<bool, float>*>(base_curve);
              return OSSIA::Float{curve->valueAt(val.value)};
            }
            case OSSIA::CurveSegmentType::INT:
            {
              auto curve = static_cast<JamomaCurve<bool, int>*>(base_curve);
              return OSSIA::Int{curve->valueAt(val.value)};
            }
            case OSSIA::CurveSegmentType::BOOL:
            {
              auto curve = static_cast<JamomaCurve<bool, bool>*>(base_curve);
              return OSSIA::Bool{curve->valueAt(val.value)};
            }
          }
        }
      }
      throw std::runtime_error("none handled driver value type");

      break;
    }

    case OSSIA::Type::TUPLE :
    {
      auto& t_drive = drive.get<OSSIA::Tuple>();

      if (driver.getType() == OSSIA::Type::TUPLE)
      {
        auto& t_driver = driver.get<OSSIA::Tuple>();

        std::vector<OSSIA::Value> t_value;
        t_value.reserve(t_drive.value.size());
        auto it_driver = t_driver.value.begin();

        for (const auto & e_drive : t_drive.value)
        {
          if (it_driver == t_driver.value.end())
            break;

          t_value.push_back(computeValue(*it_driver, e_drive));
          it_driver++;
        }

        return OSSIA::Tuple{std::move(t_value)};
      }
    }

    default :
    {
      throw std::runtime_error("none handled drive value type");
    }
  }

  throw std::runtime_error("none handled drive value type");
}

void JamomaMapper::driverValueCallback(const OSSIA::Value& value)
{
  std::lock_guard<std::mutex> lock(mValueToMapMutex);

  mValueToMap = value;
}
}
