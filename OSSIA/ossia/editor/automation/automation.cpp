#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/curve/curve.hpp>

namespace ossia
{

automation::automation(
    Destination address, const ossia::value& drive)
    : mDrivenAddress(address)
    , mDrive(drive)
    , mLastMessage{address, ossia::value{}}
{
}

automation::automation(
    Destination address, ossia::value&& drive)
    : mDrivenAddress(address)
    , mDrive(std::move(drive))
    , mLastMessage{address, ossia::value{}}
{
}

automation::~automation() = default;

ossia::state_element automation::offset(ossia::time_value offset)
{
  auto& par = *parent;
  if (par.getRunning())
  {
    throw execution_error("automation_impl::offset: "
                          "parent time constraint is running");
    return {};
  }
  // edit a Message handling the new Value
  return ossia::message{
      mDrivenAddress, computeValue(offset / par.getDurationNominal(), mDrive)};
}

ossia::state_element automation::state()
{
  auto& par = *parent;
  if (par.getRunning())
  {
    // if date hasn't been processed already
    ossia::time_value date = par.getDate();
    if (date != mLastDate)
    {
      mLastDate = date;

      // edit a Message handling the new Value
      mLastMessage.value = computeValue(par.getPosition(), mDrive);
    }

    return mLastMessage;
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
  if (auto b = mDrive.try_get<ossia::Behavior>())
  {
    if (auto& curve = b->value)
    {
      curve->reset();
    }
  }
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

Destination automation::getDrivenAddress() const
{
  return mDrivenAddress;
}

const ossia::value& automation::getDriving() const
{
  return mDrive;
}

ossia::value
automation::computeValue(double position, const ossia::value& drive)
{
  struct computeValue_visitor
  {
    double position;
    const ossia::value& drive;

    ossia::value operator()(const ossia::Behavior& b) const
    {
      auto base_curve = b.value.get();
      auto t = base_curve->getType();
      if (t.first == ossia::curve_segment_type::DOUBLE)
      {
        switch (t.second)
        {
          case ossia::curve_segment_type::FLOAT:
          {
            auto c = static_cast<curve<double, float>*>(base_curve);
            return ossia::Float{c->valueAt(position)};
          }
          case ossia::curve_segment_type::INT:
          {
            auto c = static_cast<curve<double, int>*>(base_curve);
            return ossia::Int{c->valueAt(position)};
          }
          case ossia::curve_segment_type::BOOL:
          {
            auto c = static_cast<curve<double, bool>*>(base_curve);
            return ossia::Bool{c->valueAt(position)};
          }
          case ossia::curve_segment_type::DOUBLE:
            break;
          case ossia::curve_segment_type::ANY:
          {
            auto c = static_cast<constant_curve*>(base_curve);
            // TODO we need a specific handling for destination.
            return c->value();
          }
        }
      }


      throw invalid_value_type_error("computeValue_visitor: drive curve type is not DOUBLE");
      return {};
    }

    ossia::value operator()(const ossia::Tuple& t) const
    {
      std::vector<ossia::value> t_value;
      t_value.reserve(t.value.size());

      for (const auto& e : t.value)
      {
        t_value.push_back(automation::computeValue(position, e));
      }

      return ossia::Tuple{std::move(t_value)};
    }

    ossia::value error() const
    {
      throw invalid_value_type_error("computeValue_visitor: Unhandled drive value type.");
      return {};
    }
    ossia::value operator()(const ossia::Int&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Float&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Bool&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Char&)
    {
      return error();
    }
    ossia::value operator()(const ossia::String&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Destination&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Vec2f&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Vec3f&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Vec4f&)
    {
      return error();
    }
    ossia::value operator()(const ossia::Impulse&)
    {
      return error();
    }
    ossia::value operator()()
    {
      return error();
    }
  };


  return drive.apply(computeValue_visitor{position, drive});
}
}
