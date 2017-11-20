// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/mapper/mapper.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <iostream>
#include <ossia/editor/mapper/detail/mapper_visitor.hpp>

namespace ossia
{

/*
mapper::mapper()
{
}

mapper::mapper(
    ossia::destination driverAddress, ossia::destination drivenAddress,
    ossia::behavior drive)
    : m_driverAddress{std::move(driverAddress)}
    , m_drive{std::move(drive)}
    , m_lastMessage{ossia::message{std::move(drivenAddress), {}}}
{
}

mapper::~mapper()
{
  stop();
}

ossia::state_element mapper::offset(ossia::time_value offset, double pos)
{
  return {}; // TODO why not state ?
}

ossia::state_element mapper::state(ossia::time_value date, double pos, ossia::time_value tick_offset)
{
  // if date hasn't been processed already
  if (date != m_lastDate)
  {
    m_lastDate = date;

    {
      std::unique_lock<mutex_t> lock(m_valueToMapMutex);
      if (m_valueToMap.valid())
      {
        // edit a Message handling the mapped value
        ossia::value val = std::move(m_valueToMap);
        // forget the former value

        m_valueToMap.reset();
        lock.unlock();

        if (m_lastMessage)
          m_lastMessage->message_value = compute_value(val, m_drive);

        if (unmuted() && m_lastMessage)
          return *m_lastMessage;
        return ossia::state_element{};
      }
    }
  }

  if (unmuted() && m_lastMessage)
    return *m_lastMessage;
  return ossia::state_element{};
}

void mapper::start()
{
  // start driver address value observation
  if (m_driverAddress && !m_callback)
  {
    ossia::net::parameter_base& addr = m_driverAddress->address();
    m_callback = addr.add_callback(
        [this](const ossia::value& val) { driver_value_callback(val); });

    auto def_val = addr.value();
    driver_value_callback(def_val);
  }
}

void mapper::stop()
{
  // stop driver address value observation
  if (m_driverAddress && m_callback)
  {
    m_driverAddress->address().remove_callback(*m_callback);
    m_callback = ossia::none;
  }
}

void mapper::pause()
{
}

void mapper::resume()
{
}

void mapper::set_driver(ossia::destination d)
{
  bool active{m_callback};

  stop();

  {
    lock_t lock(m_driverAddressMutex);
    m_driverAddress = std::move(d);
  }

  if (active)
  {
    ossia::net::parameter_base& addr = m_driverAddress->address();
    m_callback = addr.add_callback(
        [this](const ossia::value& val) { driver_value_callback(val); });
  }
}

void mapper::set_driven(ossia::destination d)
{
  if (m_lastMessage)
  {
    m_lastMessage->dest = std::move(d);
  }
  else
  {
    m_lastMessage = ossia::message{d, ossia::value{}};
  }
}

void mapper::clean()
{
  // Cleans the callback
  stop();
  {
    lock_t lock(m_driverAddressMutex);
    m_driverAddress = ossia::none;
  }

  m_drive.reset();
  m_lastMessage = ossia::none;
}

void mapper::set_behavior(ossia::behavior b)
{
  m_drive = std::move(b);
}

ossia::value
mapper::compute_value(const ossia::value& driver, const ossia::behavior& drive)
{
  if (driver.valid() && drive)
  {
    return ossia::apply(detail::mapper_compute_visitor{}, driver.v, drive.v);
  }

  throw invalid_value_type_error(
      "mapper_impl::computeValue: "
      "drive.getType() is neither BEHAVIOR nor LIST");
  return {};
}

void mapper::driver_value_callback(ossia::value value)
{
  // This access is protected by a mutex because driverValueCallback can come
  // from a network thread.
  std::unique_lock<mutex_t> l1{m_driverAddressMutex};
  if (m_driverAddress)
  {
    auto driver = *m_driverAddress;
    l1.unlock();

    auto driverUnit = driver.address().get_unit();
    if (driverUnit && driver.unit && driverUnit != driver.unit)
    {
      auto v = ossia::convert(value, driverUnit, driver.unit);
      if (driver.index.size() == 1)
      {
        value = get_value_at_index(std::move(v), driver.index);
      }
      else
      {
        value = std::move(v);
      }
    }
    else if (!driver.index.empty())
    {
      auto val = std::move(value);
      value = get_value_at_index(std::move(val), driver.index);
    }

    {
      lock_t lock(m_valueToMapMutex);

      m_valueToMap = std::move(value);
    }
  }
}
*/
}
