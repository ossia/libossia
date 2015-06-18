#include "Editor/JamomaAutomation.h"

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

template<> shared_ptr<Automation<double>> Automation<double>::create(shared_ptr<TimeConstraint> parentConstraint, shared_ptr<Clock> clock)
{
  return make_shared<JamomaAutomation>(parentConstraint);
}

JamomaAutomation::JamomaAutomation(shared_ptr<TimeConstraint> parentConstraint, shared_ptr<Clock> clock) :
mParentConstraint(parentConstraint),
mClock(clock)
{}

JamomaAutomation::JamomaAutomation(const JamomaAutomation * other)
{}

JamomaAutomation::~JamomaAutomation()
{}

shared_ptr<Automation<double>> JamomaAutomation::clone() const
{
  return make_shared<JamomaAutomation>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaAutomation::play(bool log, string name) const
{}

# pragma mark -
# pragma mark Accessors

Value JamomaAutomation::getStartValue() const
{
  return Value();
}

void JamomaAutomation::setStartValue(Value)
{}

Value JamomaAutomation::getEndValue() const
{
  return Value();
}

void JamomaAutomation::setEndValue(Value)
{}

const shared_ptr<Curve<double>> & JamomaAutomation::getCurve() const
{
  return curve;
}

void JamomaAutomation::setCurve(shared_ptr<Curve<double>>)
{}

const shared_ptr<Address> & JamomaAutomation::getInputAddress() const
{
  return addr;
}

void JamomaAutomation::setInputAddress(shared_ptr<Address>)
{}

const shared_ptr<TimeConstraint> & JamomaAutomation::getParentTimeConstraint() const
{
  return mParentConstraint;
}

const shared_ptr<State> & JamomaAutomation::getStartState() const
{
  
}

const shared_ptr<State> & JamomaAutomation::getEndState() const
{
  
}

const shared_ptr<Clock> & JamomaAutomation::getClock() const
{
  return mClock;
}
