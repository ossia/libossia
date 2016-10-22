#include <ossia/editor/mapper/mapper.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <iostream>

namespace ossia
{

mapper::mapper(
      ossia::Destination driverAddress,
      ossia::Destination drivenAddress,
      ossia::behavior drive)
    : mDriverAddress{std::move(driverAddress)}
    , mDrivenAddress{std::move(drivenAddress)}
    , mDrive{std::move(drive)}
    , mLastMessage{mDrivenAddress, {}}
    , mDriverValueObserved(false)
{
}

mapper::~mapper()
{
}

ossia::state_element mapper::offset(ossia::time_value offset)
{
  if (parent()->getRunning())
    throw execution_error("mapper_impl::offset: "
                           "parent time constraint is running");

  return {}; // TODO why not state ?
}

ossia::state_element mapper::state()
{
  auto& par = *parent();
  if (!par.getRunning())
  {
    throw execution_error("mapper_impl::state: "
                          "parent time constraint is not running");
    return {};
  }

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

      mLastMessage.message_value = std::move(newval);

      return mLastMessage;
    }
  }

  return mLastMessage;
}

void mapper::start()
{
  // start driver address value observation
  if (!mDriverValueObserved)
  {
    mDriverValueCallbackIndex = mDriverAddress.get().add_callback(
        [this](const ossia::value& val) { driverValueCallback(val); });
    mDriverValueObserved = true;
    auto def_val = mDriverAddress.get().cloneValue();
    driverValueCallback(def_val);
  }
}

void mapper::stop()
{
  // stop driver address value observation
  if (mDriverValueObserved)
  {
    mDriverAddress.get().remove_callback(mDriverValueCallbackIndex);
    mDriverValueObserved = false;
  }
}

void mapper::pause()
{
}

void mapper::resume()
{
}

const ossia::Destination& mapper::getDriverAddress() const
{
  return mDriverAddress;
}

const ossia::Destination& mapper::getDrivenAddress() const
{
  return mDrivenAddress;
}

const ossia::behavior& mapper::getDriving() const
{
  return mDrive;
}

struct mapper_compute_visitor
{
  const ossia::value& driver;

  ossia::value operator()(const ossia::curve_ptr& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->getType();
    switch (t.first)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto& val = driver.get<ossia::Float>();
        switch (t.second)
        {
          case ossia::curve_segment_type::FLOAT:
          {
            auto c = static_cast<curve<float, float>*>(base_curve);
            return ossia::Float{c->valueAt(val)};
          }
          case ossia::curve_segment_type::INT:
          {
            auto c = static_cast<curve<float, int>*>(base_curve);
            return ossia::Int{c->valueAt(val)};
          }
          case ossia::curve_segment_type::BOOL:
          {
            auto c = static_cast<curve<float, bool>*>(base_curve);
            return ossia::Bool{c->valueAt(val)};
          }
          case ossia::curve_segment_type::DOUBLE:
          case ossia::curve_segment_type::ANY:
            break;
        }
      }
      case ossia::curve_segment_type::INT:
      {
        auto& val = driver.get<ossia::Int>();
        switch (t.second)
        {
          case ossia::curve_segment_type::FLOAT:
          {
            auto c = static_cast<curve<int, float>*>(base_curve);
            return ossia::Float{c->valueAt(val)};
          }
          case ossia::curve_segment_type::INT:
          {
            auto c = static_cast<curve<int, int>*>(base_curve);
            return ossia::Int{c->valueAt(val)};
          }
          case ossia::curve_segment_type::BOOL:
          {
            auto c = static_cast<curve<int, bool>*>(base_curve);
            return ossia::Bool{c->valueAt(val)};
          }
          case ossia::curve_segment_type::DOUBLE:
          case ossia::curve_segment_type::ANY:
            break;
        }
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto& val = driver.get<ossia::Bool>();
        switch (t.second)
        {
          case ossia::curve_segment_type::FLOAT:
          {
            auto c = static_cast<curve<bool, float>*>(base_curve);
            return ossia::Float{c->valueAt(val)};
          }
          case ossia::curve_segment_type::INT:
          {
            auto c = static_cast<curve<bool, int>*>(base_curve);
            return ossia::Int{c->valueAt(val)};
          }
          case ossia::curve_segment_type::BOOL:
          {
            auto c = static_cast<curve<bool, bool>*>(base_curve);
            return ossia::Bool{c->valueAt(val)};
          }
          case ossia::curve_segment_type::DOUBLE:
          case ossia::curve_segment_type::ANY:
            break;
        }
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
        break;
    }

    throw invalid_value_type_error("mapper_impl::computeValue: "
                                   "base_curve->getType() is incorrect");
  }

  ossia::value operator()(const std::vector<ossia::behavior>& t_drive)
  {
    if (auto t_driver = driver.target<ossia::Tuple>())
    {
      std::vector<ossia::value> t_value;
      t_value.reserve(t_drive.size());
      auto it_driver = t_driver->begin();

      for (const auto& e_drive : t_drive)
      {
        if (it_driver == t_driver->end())
          break;

        t_value.push_back(mapper::computeValue(*it_driver, e_drive));
        it_driver++;
      }

      return ossia::Tuple{std::move(t_value)};
    }

    throw invalid_value_type_error("mapper_impl::computeValue: "
                                   "t_driver is incorrect");
    return {};
  }
};

ossia::value mapper::computeValue(
    const ossia::value& driver, const ossia::behavior& drive)
{
  if(drive)
  {
    return eggs::variants::apply(mapper_compute_visitor{driver}, drive);
  }

  throw invalid_value_type_error("mapper_impl::computeValue: "
                                 "drive.getType() is neither BEHAVIOR nor TUPLE");
  return {};
}

void mapper::driverValueCallback(const ossia::value& value)
{
  std::lock_guard<std::mutex> lock(mValueToMapMutex);

  mValueToMap = value;
}
}
