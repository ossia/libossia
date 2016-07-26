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
    std::shared_ptr<OSSIA::Address> address,
    const OSSIA::Value& drive) :
  JamomaTimeProcess(),
  mDrivenAddress(address),
  mDrive(drive)
{}

JamomaAutomation::JamomaAutomation(const JamomaAutomation& other) = default;
JamomaAutomation::~JamomaAutomation() = default;

std::shared_ptr<OSSIA::Automation> JamomaAutomation::clone() const
{
  return std::make_shared<JamomaAutomation>(*this);
}


#if 0
# pragma mark -
# pragma mark Execution
#endif

OSSIA::StateElement JamomaAutomation::offset(
    OSSIA::TimeValue offset)
{
  auto& par = *parent;
  if (par.getRunning())
    throw std::runtime_error("parent time constraint is running");

  // edit a Message handling the new Value
  return OSSIA::Message{
    mDrivenAddress,
        computeValue(offset / par.getDurationNominal(), mDrive)};
}

OSSIA::StateElement JamomaAutomation::state()
{
  auto& par = *parent;
  if (par.getRunning())
  {
    // if date hasn't been processed already
    OSSIA::TimeValue date = par.getDate();
    if (date != mLastDate)
    {
      mLastDate = date;

      // edit a Message handling the new Value
      return mLastMessage = OSSIA::Message{
          mDrivenAddress,
          computeValue(par.getPosition(), mDrive)};
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
  if(auto b = mDrive.try_get<OSSIA::Behavior>())
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
const std::shared_ptr<OSSIA::Address> JamomaAutomation::getDrivenAddress() const
{
  return mDrivenAddress;
}

const OSSIA::Value& JamomaAutomation::getDriving() const
{
  return mDrive;
}

namespace
{
struct computeValue_visitor
{
  double position;
  const OSSIA::Value& drive;

  OSSIA::Value operator()(const OSSIA::Behavior& b) const
  {
    auto base_curve = b.value.get();
    auto t = base_curve->getType();
    if(t.first == OSSIA::CurveSegmentType::DOUBLE)
    {
      switch(t.second)
      {
        case OSSIA::CurveSegmentType::FLOAT:
        {
          auto curve = static_cast<JamomaCurve<double, float>*>(base_curve);
          return OSSIA::Float{curve->valueAt(position)};
        }
        case OSSIA::CurveSegmentType::INT:
        {
          auto curve = static_cast<JamomaCurve<double, int>*>(base_curve);
          return OSSIA::Int{curve->valueAt(position)};
        }
        case OSSIA::CurveSegmentType::BOOL:
        {
          auto curve = static_cast<JamomaCurve<double, bool>*>(base_curve);
          return OSSIA::Bool{curve->valueAt(position)};
        }
      }
    }

    throw std::runtime_error("none handled drive curve type");
  }

  OSSIA::Value operator()(const OSSIA::Tuple& t) const
  {
    std::vector<OSSIA::Value> t_value;
    t_value.reserve(t.value.size());

    for (const auto & e : t.value)
    {
      t_value.push_back(JamomaAutomation::computeValue(position, e));
    }

    return OSSIA::Tuple{std::move(t_value)};
  }

  OSSIA::Value error() const
  { throw std::runtime_error("Unhandled drive value type."); }
  OSSIA::Value operator()(const OSSIA::Int&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Float&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Bool&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Char&) { return error(); }
  OSSIA::Value operator()(const OSSIA::String&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Destination&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Vec2f&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Vec3f&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Vec4f&) { return error(); }
  OSSIA::Value operator()(const OSSIA::Impulse&) { return error(); }

};
}

OSSIA::Value JamomaAutomation::computeValue(
    double position,
    const OSSIA::Value& drive)
{
  computeValue_visitor vis{position, drive};

  if(drive.valid())
    return eggs::variants::apply(vis, drive.v);
  throw std::runtime_error("none handled drive value type");
}

}
