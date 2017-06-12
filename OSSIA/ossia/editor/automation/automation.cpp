#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/automation/curve_value_visitor.hpp>
#include <ossia/network/base/address.hpp>
#include <iostream>

namespace ossia
{

automation::automation()
{

}

automation::automation(
    Destination address, const ossia::behavior& drive)
  : m_drive(drive)
  , m_lastMessage{ossia::message{address, ossia::value{}}}
  , m_drivenType{address.address().value(address.index).getType()}
{
}

automation::automation(
    Destination address, ossia::behavior&& drive)
  : m_drive(std::move(drive))
  , m_lastMessage{ossia::message{address, ossia::value{}}}
  , m_drivenType{address.address().value(address.index).getType()}
{
}

automation::~automation() = default;

void automation::update_message(double t)
{
  if(m_lastMessage)
  {
    m_lastMessage->message_value = compute_value(t, m_drivenType, m_drive);
  }
}

ossia::state_element automation::offset(ossia::time_value offset, double pos)
{
  auto& par = *parent();
  if (par.running())
  {
    throw execution_error("automation_impl::offset: "
                          "parent time constraint is running");
    return {};
  }
  // edit a Message handling the new Value
  update_message(pos);

  if(unmuted() && m_lastMessage)
    return *m_lastMessage;
  return ossia::state_element{};
}

ossia::state_element automation::state(ossia::time_value date, double pos)
{
  auto& par = *parent();
  if (par.running())
  {
    // if date hasn't been processed already
    if (date != m_lastDate)
    {
      m_lastDate = date;

      // edit a Message handling the new Value
      update_message(pos);
    }

    if(unmuted() && m_lastMessage)
      return *m_lastMessage;
    return ossia::state_element{};
  }
  else
  {
    throw execution_error("automation_impl::state: "
                          "parent time constraint is not running");
    return {};
  }
}

void automation::start()
{
  m_drive.reset();
}

void automation::stop()
{
}

void automation::pause()
{
}

void automation::resume()
{
}

void automation::set_destination(Destination d)
{
  m_drivenType = d.address().value(d.index).getType();
  if(m_lastMessage)
  {
    m_lastMessage->destination = std::move(d);
  }
  else
  {
    m_lastMessage = ossia::message{d, ossia::value{}};
  }
}

void automation::set_behavior(behavior b)
{
  m_drive = std::move(b);
}

void automation::clean()
{
  m_drive.reset();
  m_lastMessage = ossia::none;
}

ossia::value
automation::compute_value(
    double position,
    ossia::val_type drivenType,
    const ossia::behavior& drive)
{
  if(drive)
    return ossia::apply(detail::compute_value_visitor{position, drivenType}, drive);
  return {};
}

}
