#include "Automation_impl.hpp"
#include <ossia/editor/curve/detail/Curve_impl.hpp>
#include <ossia/editor/value/behavior.hpp>

#include <iostream>
#if 0
# pragma mark -
# pragma mark Life cycle
#endif
namespace impl
{
JamomaAutomation::JamomaAutomation(
    ossia::net::address& address,
    const ossia::value& drive) :
  JamomaTimeProcess(),
  mDrivenAddress(address),
  mDrive(drive),
  mLastMessage{address, ossia::value{}}
{}

JamomaAutomation::~JamomaAutomation() = default;


#if 0
# pragma mark -
# pragma mark Execution
#endif

ossia::StateElement JamomaAutomation::offset(
    ossia::time_value offset)
{
  auto& par = *parent;
  if (par.getRunning())
    throw std::runtime_error("parent time constraint is running");

  // edit a Message handling the new Value
  return ossia::Message{
    mDrivenAddress,
        computeValue(offset / par.getDurationNominal(), mDrive)};
}

ossia::StateElement JamomaAutomation::state()
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
      return mLastMessage;
    }

    return mLastMessage;
  }
  else
  {
    throw std::runtime_error("parent time constraint is not running");
  }
}
#if 0
# pragma mark -
# pragma mark Execution - Implementation specific
#endif

void JamomaAutomation::start()
{
  if(auto b = mDrive.try_get<ossia::Behavior>())
  {
    if(auto& curve = b->value)
    {
      curve->reset();
    }
  }
}

void JamomaAutomation::stop()
{}

void JamomaAutomation::pause()
{}

void JamomaAutomation::resume()
{}
#if 0
# pragma mark -
# pragma mark Accessors
#endif
const ossia::net::address& JamomaAutomation::getDrivenAddress() const
{
  return mDrivenAddress;
}

const ossia::value& JamomaAutomation::getDriving() const
{
  return mDrive;
}

namespace
{
struct computeValue_visitor
{
  double position;
  const ossia::value& drive;

  ossia::value operator()(const ossia::Behavior& b) const
  {
    auto base_curve = b.value.get();
    auto t = base_curve->getType();
    if(t.first == ossia::curve_segment_type::DOUBLE)
    {
      switch(t.second)
      {
        case ossia::curve_segment_type::FLOAT:
        {
          auto curve = static_cast<JamomaCurve<double, float>*>(base_curve);
          return ossia::Float{curve->valueAt(position)};
        }
        case ossia::curve_segment_type::INT:
        {
          auto curve = static_cast<JamomaCurve<double, int>*>(base_curve);
          return ossia::Int{curve->valueAt(position)};
        }
        case ossia::curve_segment_type::BOOL:
        {
          auto curve = static_cast<JamomaCurve<double, bool>*>(base_curve);
          return ossia::Bool{curve->valueAt(position)};
        }
      }
    }

    throw std::runtime_error("none handled drive curve type");
  }

  ossia::value operator()(const ossia::Tuple& t) const
  {
    std::vector<ossia::value> t_value;
    t_value.reserve(t.value.size());

    for (const auto & e : t.value)
    {
      t_value.push_back(JamomaAutomation::computeValue(position, e));
    }

    return ossia::Tuple{std::move(t_value)};
  }

  ossia::value error() const
  { throw std::runtime_error("Unhandled drive value type."); }
  ossia::value operator()(const ossia::Int&) { return error(); }
  ossia::value operator()(const ossia::Float&) { return error(); }
  ossia::value operator()(const ossia::Bool&) { return error(); }
  ossia::value operator()(const ossia::Char&) { return error(); }
  ossia::value operator()(const ossia::String&) { return error(); }
  ossia::value operator()(const ossia::Destination&) { return error(); }
  ossia::value operator()(const ossia::Vec2f&) { return error(); }
  ossia::value operator()(const ossia::Vec3f&) { return error(); }
  ossia::value operator()(const ossia::Vec4f&) { return error(); }
  ossia::value operator()(const ossia::Impulse&) { return error(); }

};
}

ossia::value JamomaAutomation::computeValue(
    double position,
    const ossia::value& drive)
{
  computeValue_visitor vis{position, drive};

  if(drive.valid())
    return eggs::variants::apply(vis, drive.v);
  throw std::runtime_error("none handled drive value type");
}

}
