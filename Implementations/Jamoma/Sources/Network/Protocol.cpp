#include "Network/JamomaProtocol.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

JamomaProtocol::JamomaProtocol()
{
  TTFoundationInit("/usr/local/jamoma/extensions/", true);
  TTModularInit("/usr/local/jamoma/extensions", true);
  
  // if no application manager
  if (TTModularApplicationManager == NULL)
    mApplicationManager = TTObject("ApplicationManager");
  else
    mApplicationManager = TTObjectBasePtr(TTModularApplicationManager);
}

# pragma mark -
# pragma mark Operation

bool JamomaProtocol::pullAddressValue(Address& address) const
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

bool JamomaProtocol::pushAddressValue(const Address& address) const
{
  const JamomaAddress& adrs = dynamic_cast<const JamomaAddress&>(address);
  
  TTValue value;
  
  adrs.getValue(value);

  return adrs.pushValue(value);
}

bool JamomaProtocol::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
  shared_ptr<JamomaAddress> adrs = dynamic_pointer_cast<JamomaAddress>(address);
  
  adrs->observeValue(enable);
  
  return true;
}
