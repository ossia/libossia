#include "Network/Protocol/JamomaOSC.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<OSC> OSC::create(std::string ip, int in_port, int out_port)
{
  return make_shared<JamomaOSC>(ip, in_port, out_port);
}

JamomaOSC::JamomaOSC(std::string ip, int in_port, int out_port) :
mIp(ip),
mInPort(in_port),
mOutPort(out_port)
{}

JamomaOSC::~JamomaOSC()
{}

# pragma mark -
# pragma mark Accessors

std::string JamomaOSC::getIp()
{
  return mIp;
}

int JamomaOSC::getInPort()
{
  return mInPort;
}

int JamomaOSC::getOutPort()
{
  return mOutPort;
}

# pragma mark -
# pragma mark Operation

bool JamomaOSC::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaOSC::pushAddressValue(std::shared_ptr<Address>) const
{
  /*
  TTValue v;
  address->convertValueIntoTTValue(address->mValue, v);
  
  // because TTApplication with OSC protocol use proxy Data
  return !address->mObject.send("Command", v);
   */
}

bool JamomaOSC::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
