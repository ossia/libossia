#include "Network/Protocol/JamomaMIDI.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<MIDI> MIDI::create()
{
  return make_shared<JamomaMIDI>();
}

JamomaMIDI::JamomaMIDI()
{}

JamomaMIDI::~JamomaMIDI()
{}

# pragma mark -
# pragma mark Operation

bool JamomaMIDI::pullAddressValue(Address& address) const
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

bool JamomaMIDI::pushAddressValue(const Address& address) const
{
  const JamomaAddress& adrs = dynamic_cast<const JamomaAddress&>(address);
  
  TTValue value;
  
  adrs.getValue(value);
  
  return adrs.pushValue(value);
}

bool JamomaMIDI::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
  shared_ptr<JamomaAddress> adrs = dynamic_pointer_cast<JamomaAddress>(address);
  
  adrs->observeValue(enable);
  
  return true;
}

bool JamomaMIDI::updateChildren(Node& node) const
{
  return false;
}

vector<std::shared_ptr<MIDI>> JamomaMIDI::scan()
{
  return {};
}
