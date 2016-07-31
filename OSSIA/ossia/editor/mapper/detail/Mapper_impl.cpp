#include "Mapper_impl.hpp"
#include <ossia/editor/curve/detail/Curve_impl.hpp>
#include <iostream>
namespace detail
{

mapper_impl::mapper_impl(
    ossia::net::address_base& driverAddress,
    ossia::net::address_base& drivenAddress,
    const ossia::value& drive) :
  time_process_impl(),
  mDriverAddress{driverAddress},
  mDrivenAddress{drivenAddress},
  mDrive{drive},
  mLastMessage{mDrivenAddress, {}},
  mDriverValueObserved(false)
{}

mapper_impl::~mapper_impl()
{}

ossia::state_element mapper_impl::offset(ossia::time_value offset)
{
  if (parent->getRunning())
    throw std::runtime_error("parent time constraint is running");

  return {}; // TODO why not state ?
}

ossia::state_element mapper_impl::state()
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

void mapper_impl::start()
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

void mapper_impl::stop()
{
  // stop driver address value observation
  if (mDriverValueObserved)
  {
    mDriverAddress.removeCallback(mDriverValueCallbackIndex);
    mDriverValueObserved = false;
  }
}

void mapper_impl::pause()
{}

void mapper_impl::resume()
{}

const ossia::net::address_base& mapper_impl::getDriverAddress() const
{
  return mDriverAddress;
}

const ossia::net::address_base& mapper_impl::getDrivenAddress() const
{
  return mDrivenAddress;
}

const ossia::value& mapper_impl::getDriving() const
{
  return mDrive;
}

ossia::value mapper_impl::computeValue(
    const ossia::value& driver,
    const ossia::value& drive)
{
  switch (drive.getType())
  {
    case ossia::val_type::BEHAVIOR :
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
              auto curve = static_cast<curve_impl<float, float>*>(base_curve);
              return ossia::Float{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::INT:
            {
              auto curve = static_cast<curve_impl<float, int>*>(base_curve);
              return ossia::Int{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::BOOL:
            {
              auto curve = static_cast<curve_impl<float, bool>*>(base_curve);
              return ossia::Bool{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::DOUBLE:
              break;
          }
        }
        case ossia::curve_segment_type::INT:
        {
          auto& val = driver.get<ossia::Int>();
          switch(t.second)
          {
            case ossia::curve_segment_type::FLOAT:
            {
              auto curve = static_cast<curve_impl<int, float>*>(base_curve);
              return ossia::Float{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::INT:
            {
              auto curve = static_cast<curve_impl<int, int>*>(base_curve);
              return ossia::Int{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::BOOL:
            {
              auto curve = static_cast<curve_impl<int, bool>*>(base_curve);
              return ossia::Bool{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::DOUBLE:
              break;
          }
        }
        case ossia::curve_segment_type::BOOL:
        {
          auto& val = driver.get<ossia::Bool>();
          switch(t.second)
          {
            case ossia::curve_segment_type::FLOAT:
            {
              auto curve = static_cast<curve_impl<bool, float>*>(base_curve);
              return ossia::Float{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::INT:
            {
              auto curve = static_cast<curve_impl<bool, int>*>(base_curve);
              return ossia::Int{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::BOOL:
            {
              auto curve = static_cast<curve_impl<bool, bool>*>(base_curve);
              return ossia::Bool{curve->valueAt(val.value)};
            }
            case ossia::curve_segment_type::DOUBLE:
              break;
          }
        }
        case ossia::curve_segment_type::DOUBLE:
          break;
      }
      throw std::runtime_error("none handled driver value type");

      break;
    }

    case ossia::val_type::TUPLE :
    {
      auto& t_drive = drive.get<ossia::Tuple>();

      if (driver.getType() == ossia::val_type::TUPLE)
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

void mapper_impl::driverValueCallback(const ossia::value& value)
{
  std::lock_guard<std::mutex> lock(mValueToMapMutex);

  mValueToMap = value;
}
}
