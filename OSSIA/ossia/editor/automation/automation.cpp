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
  : mDrive(drive)
  , mLastMessage{ossia::message{address, ossia::value{}}}
  , mDrivenType{address.value.get().cloneValue(address.index).getType()}
{
}

automation::automation(
    Destination address, ossia::behavior&& drive)
  : mDrive(std::move(drive))
  , mLastMessage{ossia::message{address, ossia::value{}}}
  , mDrivenType{address.value.get().cloneValue(address.index).getType()}
{
}

automation::~automation() = default;

void automation::updateMessage(double t)
{
  if(mLastMessage)
    mLastMessage->message_value = computeValue(t, mDrivenType, mDrive);
}

ossia::state_element automation::offset(ossia::time_value offset)
{
  auto& par = *parent();
  if (par.getRunning())
  {
    throw execution_error("automation_impl::offset: "
                          "parent time constraint is running");
    return {};
  }
  // edit a Message handling the new Value
  updateMessage(offset / par.getDurationNominal());

  if(unmuted() && mLastMessage)
    return *mLastMessage;
  return ossia::state_element{};
}

ossia::state_element automation::state()
{
  auto& par = *parent();
  if (par.getRunning())
  {
    // if date hasn't been processed already
    ossia::time_value date = par.getDate();
    if (date != mLastDate)
    {
      mLastDate = date;

      // edit a Message handling the new Value
      updateMessage(par.getDate() / par.getDurationNominal());
    }

    if(unmuted() && mLastMessage)
      return *mLastMessage;
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
  mDrive.reset();
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

void automation::setDestination(Destination d)
{
  mDrivenType = d.value.get().cloneValue(d.index).getType();
  if(mLastMessage)
  {
    mLastMessage->destination = std::move(d);
  }
  else
  {
    mLastMessage = ossia::message{d, ossia::value{}};
  }
}

void automation::setBehavior(behavior b)
{
  mDrive = std::move(b);
}

void automation::clean()
{
  mDrive.reset();
  mLastMessage = ossia::none;
}

ossia::value
automation::computeValue(
    double position,
    ossia::val_type drivenType,
    const ossia::behavior& drive)
{
  if(drive)
    return ossia::apply(detail::compute_value_visitor{position, drivenType}, drive);
  return {};
}

}
