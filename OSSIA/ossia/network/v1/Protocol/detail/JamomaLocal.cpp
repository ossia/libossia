#include "Network/Protocol/JamomaLocal.hpp"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<Local> Local::create()
{
  return make_shared<JamomaLocal>();
}

JamomaLocal::JamomaLocal()
{}

JamomaLocal::~JamomaLocal()
{}

# pragma mark -
# pragma mark Operation

bool JamomaLocal::pullAddressValue(Address& address) const
{
  JamomaAddress& adrs = dynamic_cast<JamomaAddress&>(address);

  TTValue value;

  if (adrs.pullValue(value))
  {
    adrs.setValue(value);
    return true;
  }

  return false;
}

bool JamomaLocal::pushAddressValue(const Address& address) const
{
  const JamomaAddress& adrs = dynamic_cast<const JamomaAddress&>(address);

  TTValue value;

  adrs.getValue(value);

  return adrs.pushValue(value);
}

bool JamomaLocal::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
  shared_ptr<JamomaAddress> adrs = dynamic_pointer_cast<JamomaAddress>(address);

  adrs->observeValue(enable);

  return true;
}

bool JamomaLocal::updateChildren(Node& node) const
{
  return false;
}
