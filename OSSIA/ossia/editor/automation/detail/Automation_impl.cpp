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
JamomaAutomation::JamomaAutomation(shared_ptr<Address> address,
                                   const Value& drive) :
JamomaTimeProcess(),
mDrivenAddress(address),
mDrive(drive)
{}

JamomaAutomation::JamomaAutomation(const JamomaAutomation& other) :
JamomaTimeProcess()
{}

shared_ptr<Automation> JamomaAutomation::clone() const
{
  return make_shared<JamomaAutomation>(*this);
}

JamomaAutomation::~JamomaAutomation()
{}

#if 0
# pragma mark -
# pragma mark Execution
#endif

StateElement JamomaAutomation::offset(TimeValue offset)
{
  auto& par = *parent;
  if (par.getRunning())
    throw runtime_error("parent time constraint is running");

  // edit a Message handling the new Value
  return Message{
          mDrivenAddress,
          computeValue(offset / par.getDurationNominal(), mDrive)};
}

StateElement JamomaAutomation::state()
{
  auto& par = *parent;
  if (par.getRunning())
  {
    // if date hasn't been processed already
    TimeValue date = par.getDate();
    if (date != mLastDate)
    {
      mLastDate = date;

      // edit a Message handling the new Value
      return mLastMessage = Message{
              mDrivenAddress,
              computeValue(par.getPosition(), mDrive)};
    }

    return mLastMessage;
  }
  else
  {
    throw runtime_error("parent time constraint is not running");
  }
}
#if 0
# pragma mark -
# pragma mark Execution - Implementation specific
#endif

void JamomaAutomation::start()
{
    if(auto b = mDrive.try_get<Behavior>())
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
const shared_ptr<Address> JamomaAutomation::getDrivenAddress() const
{
  return mDrivenAddress;
}

const Value& JamomaAutomation::getDriving() const
{
  return mDrive;
}

namespace
{
struct computeValue_visitor
{
  double position;
  const Value& drive;

  Value operator()(const OSSIA::Behavior& b) const
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
                return Float{curve->valueAt(position)};
            }
            case OSSIA::CurveSegmentType::INT:
            {
                auto curve = static_cast<JamomaCurve<double, int>*>(base_curve);
                return Int{curve->valueAt(position)};
            }
            case OSSIA::CurveSegmentType::BOOL:
            {
                auto curve = static_cast<JamomaCurve<double, bool>*>(base_curve);
                return Bool{curve->valueAt(position)};
            }
        }
    }

    throw runtime_error("none handled drive curve type");
  }

  Value operator()(const Tuple& t) const
  {
    vector<Value> t_value;
    t_value.reserve(t.value.size());

    for (const auto & e : t.value)
    {
      t_value.push_back(JamomaAutomation::computeValue(position, e));
    }

    return Tuple{std::move(t_value)};
  }

  Value error() const
  { throw runtime_error("Unhandled drive value type."); }
  Value operator()(const Int&) { return error(); }
  Value operator()(const Float&) { return error(); }
  Value operator()(const Bool&) { return error(); }
  Value operator()(const Char&) { return error(); }
  Value operator()(const String&) { return error(); }
  Value operator()(const Destination&) { return error(); }
  Value operator()(const Vec2f&) { return error(); }
  Value operator()(const Vec3f&) { return error(); }
  Value operator()(const Vec4f&) { return error(); }
  Value operator()(const Impulse&) { return error(); }

};
}

Value JamomaAutomation::computeValue(
    double position,
    const Value& drive)
{
  computeValue_visitor vis{position, drive};

  if(drive.valid())
    return eggs::variants::apply(vis, drive.v);
  throw runtime_error("none handled drive value type");
}

}
