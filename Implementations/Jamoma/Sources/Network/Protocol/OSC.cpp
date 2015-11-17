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
{
  // create a OSC protocol unit
  TTObject oscObject = mApplicationManager.send("ProtocolFind", "OSC");
  if (!oscObject.valid())
    mApplicationManager.send("ProtocolInstantiate", "OSC");
}

JamomaOSC::~JamomaOSC()
{}

OSC::~OSC()
{}

# pragma mark -
# pragma mark Accessors

std::string JamomaOSC::getIp()
{
  return mIp;
}

Protocol & JamomaOSC::setIp(std::string ip)
{
  TTObject oscObject = mApplicationManager.send("ProtocolFind", "OSC");
  
  mIp = ip;
  
  oscObject.set("ip", TTSymbol(mIp));
  
  return *this;
}

int JamomaOSC::getInPort()
{
  return mInPort;
}

Protocol & JamomaOSC::setInPort(int in_port)
{
  TTObject oscObject = mApplicationManager.send("ProtocolFind", "OSC");
  
  mInPort = in_port;
  
  TTValue v(mInPort, mOutPort);
  oscObject.set("port", v);
  
  return *this;
}

int JamomaOSC::getOutPort()
{
  return mOutPort;
}

Protocol & JamomaOSC::setOutPort(int out_port)
{
  TTObject oscObject = mApplicationManager.send("ProtocolFind", "OSC");
  
  mOutPort = out_port;
  
  TTValue v(mInPort, mOutPort);
  oscObject.set("port", v);
  
  return *this;
}

# pragma mark -
# pragma mark Operation

bool JamomaOSC::pullAddressValue(Address& address) const
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

bool JamomaOSC::pushAddressValue(const Address& address) const
{
  const JamomaAddress& adrs = dynamic_cast<const JamomaAddress&>(address);
  
  TTValue value;
  
  adrs.getValue(value);
  
  return adrs.pushValue(value);
}

bool JamomaOSC::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
  shared_ptr<JamomaAddress> adrs = dynamic_pointer_cast<JamomaAddress>(address);
  
  adrs->observeValue(enable);
  
  return true;
}

bool JamomaOSC::updateChildren(Node& node) const
{
  return false;
}