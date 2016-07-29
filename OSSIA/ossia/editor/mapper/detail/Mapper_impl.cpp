#include "Mapper_impl.hpp"
#include <ossia/editor/curve/detail/Curve_impl.hpp>
#include <iostream>
namespace impl
{

JamomaMapper::JamomaMapper(
    ossia::net::address& driverAddress,
    ossia::net::address& drivenAddress,
    const ossia::value& drive) :
  JamomaTimeProcess(),
  mDriverAddress{driverAddress},
  mDrivenAddress{drivenAddress},
  mDrive{drive},
  mLastMessage{mDrivenAddress, {}},
  mDriverValueObserved(false)
{}

JamomaMapper::~JamomaMapper()
{}

# pragma mark -
# pragma mark Execution

ossia::StateElement JamomaMapper::offset(ossia::time_value offset)
{
  if (parent->getRunning())
    throw std::runtime_error("parent time constraint is running");

  return {}; // TODO why not state ?
}

ossia::StateElement JamomaMapper::state()
{
  auto& par = *parent;
  if (!par.getRunning())
    throw std::runtime_error("parent time constraint is not running");

  // if date hasn't been processed already
  ossia::time_value date = par.getDate();
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
                                  [this] (const ossia::value& val) {
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

const ossia::net::address& JamomaMapper::getDriverAddress() const
{
  return mDriverAddress;
}

const ossia::net::address& JamomaMapper::getDrivenAddress() const
{
  return mDrivenAddress;
}

const ossia::value& JamomaMapper::getDriving() const
{
  return mDrive;
}

ossia::value JamomaMapper::computeValue(
    const ossia::value& driver,
    const ossia::value& drive)
{
  switch (drive.getType())
  {
    case ossia::Type::BEHAVIOR :
    {
      auto& b = drive.get<ossia::Behavior>();

      auto base_curve = b.value.get();
      auto t = base_curve->getType();
      switch(t.first)
      {
        case ossia::curve_segment_type::FLOAT:
        {
          auto& val = driver.get<ossia::Float>();
          switch(t.second)
          {
            case ossia::curve_segment_type::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<float, float>*>(base_curve);
              return ossia::Float{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::INT:
            {
              auto curve = static_cast<JamomaCurve<float, int>*>(base_curve);
              return ossia::Int{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::BOOL:
            {
              auto curve = static_cast<JamomaCurve<float, bool>*>(base_curve);
              return ossia::Bool{curve->valueAt(val.value)};
            }
          }
        }
        case ossia::curve_segment_type::INT:
        {
          auto& val = driver.get<ossia::Int>();
          switch(t.second)
          {
            case ossia::curve_segment_type::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<int, float>*>(base_curve);
              return ossia::Float{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::INT:
            {
              auto curve = static_cast<JamomaCurve<int, int>*>(base_curve);
              return ossia::Int{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::BOOL:
            {
              auto curve = static_cast<JamomaCurve<int, bool>*>(base_curve);
              return ossia::Bool{curve->valueAt(val.value)};
            }
          }
        }
        case ossia::curve_segment_type::BOOL:
        {
          auto& val = driver.get<ossia::Bool>();
          switch(t.second)
          {
            case ossia::curve_segment_type::FLOAT:
            {
              auto curve = static_cast<JamomaCurve<bool, float>*>(base_curve);
              return ossia::Float{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::INT:
            {
              auto curve = static_cast<JamomaCurve<bool, int>*>(base_curve);
              return ossia::Int{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::BOOL:
            {
              auto curve = static_cast<JamomaCurve<bool, bool>*>(base_curve);
              return ossia::Bool{curve->valueAt(val.value)};
            }
          }
        }
      }
      throw std::runtime_error("none handled driver value type");

      break;
    }

    case ossia::Type::TUPLE :
    {
      auto& t_drive = drive.get<ossia::Tuple>();

      if (driver.getType() == ossia::Type::TUPLE)
      {
        auto& t_driver = driver.get<ossia::Tuple>();

        std::vector<ossia::value> t_value;
        t_value.reserve(t_drive.value.size());
        auto it_driver = t_driver.value.begin();

        for (const auto & e_drive : t_drive.value)
        {
          if (it_driver == t_driver.value.end())
            break;

          t_value.push_back(computeValue(*it_driver, e_drive));
          it_driver++;
        }

        return ossia::Tuple{std::move(t_value)};
      }
    }

    default :
    {
      throw std::runtime_error("none handled drive value type");
    }
  }

  throw std::runtime_error("none handled drive value type");
}

void JamomaMapper::driverValueCallback(const ossia::value& value)
{
  std::lock_guard<std::mutex> lock(mValueToMapMutex);

  mValueToMap = value;
}
}
