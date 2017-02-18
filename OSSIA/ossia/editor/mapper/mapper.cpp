#include <ossia/editor/mapper/mapper.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/detail/logger.hpp>
#include <iostream>

namespace ossia
{

mapper::mapper()
{

}

mapper::mapper(
      ossia::Destination driverAddress,
      ossia::Destination drivenAddress,
      ossia::behavior drive)
    : mDriverAddress{std::move(driverAddress)}
    , mDrive{std::move(drive)}
    , mLastMessage{ossia::message{std::move(drivenAddress), {}}}
{
}

mapper::~mapper()
{
  stop();
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

    {
      std::unique_lock<mutex_t> lock(mValueToMapMutex);
      if (mValueToMap.valid())
      {
        // edit a Message handling the mapped value
        auto val = mValueToMap;
        // forget the former value

        mValueToMap.reset();
        lock.unlock();

        if(mLastMessage)
          mLastMessage->message_value = computeValue(val, mDrive);

        if(unmuted() && mLastMessage)
          return *mLastMessage;
        return ossia::state_element{};
      }
    }
  }

  if(unmuted() && mLastMessage)
    return *mLastMessage;
  return ossia::state_element{};
}

void mapper::start()
{
  // start driver address value observation
  if (mDriverAddress && !mDriverValueCallbackIndex)
  {
    ossia::net::address_base& addr = mDriverAddress->address();
    mDriverValueCallbackIndex = addr.add_callback(
        [this](const ossia::value& val) { driverValueCallback(val); });

    auto def_val = addr.cloneValue();
    driverValueCallback(def_val);
  }
}

void mapper::stop()
{
  // stop driver address value observation
  if (mDriverAddress && mDriverValueCallbackIndex)
  {
    mDriverAddress->address().remove_callback(*mDriverValueCallbackIndex);
    mDriverValueCallbackIndex = ossia::none;
  }
}

void mapper::pause()
{
}

void mapper::resume()
{
}

void mapper::setDriverAddress(ossia::Destination d)
{
  bool active{mDriverValueCallbackIndex};

  stop();

  {
    lock_t lock(mDriverAddressMutex);
    mDriverAddress = std::move(d);
  }

  if(active)
  {
    ossia::net::address_base& addr = mDriverAddress->address();
    mDriverValueCallbackIndex = addr.add_callback(
        [this](const ossia::value& val) { driverValueCallback(val); });
  }
}

void mapper::setDrivenAddress(ossia::Destination d)
{
  if(mLastMessage)
  {
    mLastMessage->destination = std::move(d);
  }
  else
  {
    mLastMessage = ossia::message{d, ossia::value{}};
  }
}

void mapper::clean()
{
  // Cleans the callback
  stop();
  {
    lock_t lock(mDriverAddressMutex);
    mDriverAddress = ossia::none;
  }

  mDrive.reset();
  mLastMessage = ossia::none;
}

void mapper::setBehavior(ossia::behavior b)
{
  mDrive = std::move(b);
}

struct mapper_compute_visitor
{
  ossia::value operator()(float driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->getType();
    if(t.first != ossia::curve_segment_type::FLOAT)
      return {};

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<float, float>*>(base_curve);
        return float{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<float, int>*>(base_curve);
        return int32_t{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<float, bool>*>(base_curve);
        return bool{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value operator()(int32_t driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->getType();
    if(t.first != ossia::curve_segment_type::INT)
      return {};

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<int, float>*>(base_curve);
        return float{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<int, int>*>(base_curve);
        return int32_t{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<int, bool>*>(base_curve);
        return bool{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value operator()(bool driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->getType();
    if(t.first != ossia::curve_segment_type::BOOL)
      return {};

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<bool, float>*>(base_curve);
        return float{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<bool, int>*>(base_curve);
        return int32_t{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<bool, bool>*>(base_curve);
        return bool{c->valueAt(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value operator()(const std::vector<ossia::value>& t_driver, const std::shared_ptr<curve_abstract>& c)
  {
    std::vector<ossia::value> t_value = t_driver;
    for (auto& v : t_value)
    {
      if(v.valid())
      {
        v = eggs::variants::apply([&] (const auto& e) {
          return this->operator ()(e, c);
        }, std::move(v.v));
      }
    }

    return t_value;
  }

  template<std::size_t N>
  ossia::value operator()(std::array<float, N> driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->getType();
    if(t.first == ossia::curve_segment_type::FLOAT && t.second == ossia::curve_segment_type::FLOAT)
    {
      auto c = static_cast<curve<float, float>*>(base_curve);
      for(std::size_t i = 0; i < N; i++)
      {
        driver[i] = c->valueAt(driver[i]);
      }
      return driver;
    }
    else
    {
      return {};
    }
  }

  template<std::size_t N>
  ossia::value operator()(std::array<float, N> driver, const std::vector<ossia::behavior>& t_drive)
  {
    if(t_drive.size() != N)
      return {};

    for(std::size_t i = 0; i < N; i++)
    {
      auto curve_p = t_drive[i].target<std::shared_ptr<curve_abstract>>();
      if(!curve_p)
        return {};

      auto c = curve_p->get();
      if(!c)
        return {};

      auto t = c->getType();
      if(t.first == ossia::curve_segment_type::FLOAT && t.second == ossia::curve_segment_type::FLOAT)
        driver[i] = static_cast<curve<float, float>*>(c)->valueAt(driver[i]);
      else
        return {};
    }

    return driver;
  }

  ossia::value operator()(const std::vector<ossia::value>& t_driver, const std::vector<ossia::behavior>& t_drive)
  {
    std::vector<ossia::value> t_value;
    t_value.reserve(t_drive.size());
    auto it_driver = t_driver.begin();

    for (const auto& e_drive : t_drive)
    {
      if (it_driver == t_driver.end())
        break;

      t_value.push_back(mapper::computeValue(*it_driver, e_drive));
      it_driver++;
    }

    return t_value;
  }

  template<typename T, typename U>
  ossia::value operator()(const T& driver, const U& t_drive)
  {
    throw invalid_value_type_error("mapper_compute_visitor_2: "
                                   "invalid case");
    return {};
  }
};

ossia::value mapper::computeValue(
    const ossia::value& driver, const ossia::behavior& drive)
{
  if(driver.valid() && drive)
  {
    return eggs::variants::apply(mapper_compute_visitor{}, driver.v, drive);
  }

  throw invalid_value_type_error("mapper_impl::computeValue: "
                                 "drive.getType() is neither BEHAVIOR nor TUPLE");
  return {};
}

void mapper::driverValueCallback(ossia::value value)
{
  // This access is protected by a mutex because driverValueCallback can come from a network thread.
  std::unique_lock<mutex_t> l1{mDriverAddressMutex};
  if(mDriverAddress)
  {
    auto driver = *mDriverAddress;
    l1.unlock();

    auto driverUnit = driver.address().getUnit();
    if(driverUnit && driver.unit && driverUnit != driver.unit)
    {
      auto v = ossia::convert(value, driverUnit, driver.unit);
      if(driver.index.size() == 1)
      {
        value = get_value_at_index(std::move(v), driver.index);
      }
      else
      {
        value = std::move(v);
      }
    }

    {
      lock_t lock(mValueToMapMutex);

      mValueToMap = value;
    }
  }
}
}
