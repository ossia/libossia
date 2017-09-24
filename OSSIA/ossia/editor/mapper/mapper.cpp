// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/mapper/mapper.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/misc_visitors.hpp>
#include <iostream>

namespace ossia
{

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

void mapper::start(ossia::state& st)
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

struct mapper_compute_visitor
{
  ossia::value
  operator()(float driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<float, float>*>(base_curve);
        return float{c->value_at(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<float, int>*>(base_curve);
        return int32_t{c->value_at(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<float, bool>*>(base_curve);
        return bool{c->value_at(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value
  operator()(int32_t driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<int, float>*>(base_curve);
        return float{c->value_at(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<int, int>*>(base_curve);
        return int32_t{c->value_at(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<int, bool>*>(base_curve);
        return bool{c->value_at(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value
  operator()(bool driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();

    switch (t.second)
    {
      case ossia::curve_segment_type::FLOAT:
      {
        auto c = static_cast<curve<bool, float>*>(base_curve);
        return float{c->value_at(driver)};
      }
      case ossia::curve_segment_type::INT:
      {
        auto c = static_cast<curve<bool, int>*>(base_curve);
        return int32_t{c->value_at(driver)};
      }
      case ossia::curve_segment_type::BOOL:
      {
        auto c = static_cast<curve<bool, bool>*>(base_curve);
        return bool{c->value_at(driver)};
      }
      case ossia::curve_segment_type::DOUBLE:
      case ossia::curve_segment_type::ANY:
      default:
        return {};
    }
  }

  ossia::value operator()(
      const std::vector<ossia::value>& t_driver,
      const std::shared_ptr<curve_abstract>& c)
  {
    std::vector<ossia::value> t_value = t_driver;
    for (auto& v : t_value)
    {
      if (v.valid())
      {
        v = ossia::apply_nonnull(
            [&](const auto& e) { return this->operator()(e, c); },
            std::move(v.v));
      }
    }

    return t_value;
  }

  template <std::size_t N>
  ossia::value operator()(
      std::array<float, N> driver, const std::shared_ptr<curve_abstract>& c)
  {
    auto base_curve = c.get();
    auto t = base_curve->get_type();
    if (t.first == ossia::curve_segment_type::FLOAT
        && t.second == ossia::curve_segment_type::FLOAT)
    {
      auto c = static_cast<curve<float, float>*>(base_curve);
      for (std::size_t i = 0; i < N; i++)
      {
        driver[i] = c->value_at(driver[i]);
      }
      return driver;
    }
    else
    {
      return {};
    }
  }

  template <std::size_t N>
  ossia::value operator()(
      std::array<float, N> driver, const std::vector<ossia::behavior>& t_drive)
  {
    if (t_drive.size() != N)
      return {};

    for (std::size_t i = 0; i < N; i++)
    {
      auto curve_p = t_drive[i].target<std::shared_ptr<curve_abstract>>();
      if (!curve_p)
        return {};

      auto c = curve_p->get();
      if (!c)
        return {};

      auto t = c->get_type();
      if (t.first == ossia::curve_segment_type::FLOAT
          && t.second == ossia::curve_segment_type::FLOAT)
        driver[i] = static_cast<curve<float, float>*>(c)->value_at(driver[i]);
      else
        return {};
    }

    return driver;
  }

  ossia::value operator()(
      const std::vector<ossia::value>& t_driver,
      const std::vector<ossia::behavior>& t_drive)
  {
    std::vector<ossia::value> t_value;
    t_value.reserve(t_drive.size());
    auto it_driver = t_driver.begin();

    for (const auto& e_drive : t_drive)
    {
      if (it_driver == t_driver.end())
        break;

      t_value.push_back(mapper::compute_value(*it_driver, e_drive));
      it_driver++;
    }

    return t_value;
  }

  template <typename T, typename U>
  ossia::value operator()(const T& driver, const U& t_drive)
  {
    throw invalid_value_type_error(
        "mapper_compute_visitor_2: "
        "invalid case");
    return {};
  }
};

ossia::value
mapper::compute_value(const ossia::value& driver, const ossia::behavior& drive)
{
  if (driver.valid() && drive)
  {
    return ossia::apply(mapper_compute_visitor{}, driver.v, drive.v);
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
}
