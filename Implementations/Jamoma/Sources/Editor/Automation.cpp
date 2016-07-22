#include "Editor/JamomaAutomation.h"
#include <Editor/JamomaCurve.h>
#include <Editor/Value/Behavior.h>

#include <iostream> //! \todo to remove. only here for debug purpose
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

namespace OSSIA
{
  shared_ptr<Automation> Automation::create(shared_ptr<Address> address,
                                            const SafeValue& drive)
  {
    return make_shared<JamomaAutomation>(address, drive);
  }

  Automation::~Automation()
  {
  }
}

JamomaAutomation::JamomaAutomation(shared_ptr<Address> address,
                                   const SafeValue& drive) :
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

shared_ptr<StateElement> JamomaAutomation::offset(TimeValue offset)
{
  auto& par = *parent;
  if (par.getRunning())
    throw runtime_error("parent time constraint is running");

  // compute a new value from the Curves
  mValueToSend = computeValue(offset / par.getDurationNominal(), mDrive);

  // edit a Message handling the new Value
  mMessageToSend = Message::create(mDrivenAddress, mValueToSend);

  return mMessageToSend;
}

shared_ptr<StateElement> JamomaAutomation::state()
{
  auto& par = *parent;
  if (par.getRunning())
  {
    // if date hasn't been processed already
    TimeValue date = par.getDate();
    if (date != mLastDate)
    {
      mLastDate = date;

      // compute a new value from the Curves
      mValueToSend = computeValue(par.getPosition(), mDrive);

      // edit a Message handling the new Value
      mMessageToSend = Message::create(mDrivenAddress, mValueToSend);
    }

    return mMessageToSend;
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
{}

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

const SafeValue& JamomaAutomation::getDriving() const
{
  return mDrive;
}

namespace
{
struct computeValue_visitor
{
  double position;
  const SafeValue& drive;

  SafeValue operator()(const OSSIA::Behavior& b) const
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

  SafeValue operator()(const Tuple& t) const
  {
    vector<SafeValue> t_value;
    t_value.reserve(t.value.size());

    for (const auto & e : t.value)
    {
      t_value.push_back(JamomaAutomation::computeValue(position, e));
    }

    return Tuple{std::move(t_value)};
  }

  SafeValue error() const
  { throw runtime_error("Unhandled drive value type."); }
  SafeValue operator()(const Int&) { return error(); }
  SafeValue operator()(const Float&) { return error(); }
  SafeValue operator()(const Bool&) { return error(); }
  SafeValue operator()(const Char&) { return error(); }
  SafeValue operator()(const String&) { return error(); }
  SafeValue operator()(const Destination&) { return error(); }
  SafeValue operator()(const Vec2f&) { return error(); }
  SafeValue operator()(const Vec3f&) { return error(); }
  SafeValue operator()(const Vec4f&) { return error(); }
  SafeValue operator()(const Impulse&) { return error(); }

};
}

SafeValue JamomaAutomation::computeValue(
    double position,
    const SafeValue& drive)
{
  computeValue_visitor vis{position, drive};

  if(drive.valid())
    return eggs::variants::apply(vis, drive.v);
  throw runtime_error("none handled drive value type");
}
