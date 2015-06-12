#include "Editor/JamomaAutomation.h"

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

JamomaAutomation::JamomaAutomation()
{}

JamomaAutomation::JamomaAutomation(const JamomaAutomation * other)
{}

JamomaAutomation::~JamomaAutomation()
{}

shared_ptr<Automation<double>> JamomaAutomation::clone() const
{
  return nullptr; // make_shared<JamomaAutomation>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaAutomation::play(bool log, string name) const
{}

# pragma mark -
# pragma mark Accessors

TimeValue JamomaAutomation::getLength() const
{}

void JamomaAutomation::setLength(TimeValue)
{}

AddressValue JamomaAutomation::getStartValue() const
{
  return AddressValue();
}

void JamomaAutomation::setStartValue(AddressValue)
{}

AddressValue JamomaAutomation::getEndValue() const
{
  return AddressValue();
}

void JamomaAutomation::setEndValue(AddressValue)
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

template<> shared_ptr<Automation<double>> Automation<double>::create()
{
  return nullptr; // make_shared<JamomaAutomation>();
}
