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

bool JamomaLocal::pullAddressValue(std::shared_ptr<Address> address) const
{
  return false;
}

bool JamomaLocal::pushAddressValue(std::shared_ptr<Address> address) const
{
  return false;
}

bool JamomaLocal::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
