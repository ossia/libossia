#include "Network/Protocol/JamomaOSC.h"

#include <Network/JamomaDevice.h>

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

# pragma mark -
# pragma mark Accessors

std::string JamomaOSC::getIp() const
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

int JamomaOSC::getInPort() const
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

int JamomaOSC::getOutPort() const
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

bool JamomaOSC::getLearningStatus() const
{
    return mLearning;
}
void NamespaceCallback(const TTValue& baton, const TTValue& value)
{
    OSSIA::Device* dev = (OSSIA::Device*)((TTPtr)baton[0]);
    TTSymbol applicationName = baton[1];
    TTUInt8 flag = value[2];

    if (flag == kAddressCreated)
    {
        TTSymbol addr = value[0];
        TTNodePtr node =  TTNodePtr((TTPtr)value[1]);
        std::cerr << "addr" << addr.c_str() << std::endl;
    }
}

Protocol& JamomaOSC::setLearningStatus(OSSIA::Device& ossiaDevice, bool newLearn)
{
    TTSymbol    applicationName(ossiaDevice.getName());
    TTObject    anApplication = accessApplication(applicationName);

    anApplication.set("learn", newLearn);

    // enable namespace observation
    if (newLearn && !mNamespaceObserver.valid()) {

        TTValue baton(TTPtr(&ossiaDevice), applicationName);

        // create a TTCallback to observe when a node is created (using NamespaceCallback)
        mNamespaceObserver = TTObject("callback");

        mNamespaceObserver.set("baton", baton);
        mNamespaceObserver.set("function",
                               TTPtr(&NamespaceCallback));

        accessApplicationDirectory(applicationName)->addObserverForNotifications(kTTAdrsRoot, mNamespaceObserver);
    }
    // disable namespace observation
    else if (!newLearn && mNamespaceObserver.valid()) {

        accessApplicationDirectory(applicationName)->removeObserverForNotifications(kTTAdrsRoot, mNamespaceObserver);

        mNamespaceObserver = TTObject();
    }

    return *this;
}

# pragma mark -
# pragma mark Operation

bool JamomaOSC::updateChildren(Node& node) const
{
  return false;
}
