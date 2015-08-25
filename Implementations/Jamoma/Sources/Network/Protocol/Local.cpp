#include "Network/Protocol/JamomaLocal.h"

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

bool JamomaLocal::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaLocal::pushAddressValue(std::shared_ptr<Address> address) const
{
  /*
  TTValue v;
  address->convertValueIntoTTValue(address->mValue, v);
  
  // because local TTApplication use Data
  return !address->mObject.send("Command", v);
  */
}

bool JamomaLocal::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
