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
  TTObject mProtocol = mApplicationManager.send("ProtocolFind", "OSC");
  if (!mProtocol.valid())
    mProtocol = mApplicationManager.send("ProtocolInstantiate", "OSC");
}

JamomaOSC::~JamomaOSC()
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

bool JamomaOSC::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaOSC::pushAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaOSC::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
