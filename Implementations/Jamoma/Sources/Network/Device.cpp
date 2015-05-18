#include "Network/Device.h"
#include "Network/Protocol.h"
#include "Node.cpp"

#include "TTFoundation.h"
#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaDevice : public Device, public JamomaNode
{

private:
  
  // Implementation specific
  TTObject mApplicationManager;
  TTObject mApplication;
  
public:

  // Constructor, destructor
  JamomaDevice(Protocol & protocol, string name = "") : JamomaNode(name)
  {
    TTSymbol device_name(name);
    
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/extensions/");
    TTModularInit("/usr/local/jamoma/extensions");
    
    // if no application manager
    if (TTModularApplicationManager == NULL)
      mApplicationManager = TTObject("ApplicationManager");
    else
      mApplicationManager = TTObjectBasePtr(TTModularApplicationManager);
    
    // is the application already exist ?
    mApplication = mApplicationManager.send("ApplicationFind", device_name);
    if (mApplication.valid())
    {
      TTLogError("%s device created already exist\n");
      return;
    }
    
    // which protocol is it ?
    // todo: this is not a good way to do as if a new protocol appears we have to create a case for it here
    Local* local_protocol = dynamic_cast<Local*>(&protocol);
    if (local_protocol)
    {
      // create a local application
      mApplication = mApplicationManager.send("ApplicationInstantiateLocal", device_name);

      // create
      TTLogMessage("Local device created\n");
      return;
    }
    
    Minuit* minuit_protocol = dynamic_cast<Minuit*>(&protocol);
    if (minuit_protocol)
    {
      // create a distante application
      mApplication = mApplicationManager.send("ApplicationInstantiateDistant", device_name);
      
      // create a Minuit protocol unit
      TTObject protocolMinuit = mApplicationManager.send("ProtocolFind", "Minuit");
      if (!protocolMinuit.valid())
        protocolMinuit = mApplicationManager.send("ProtocolInstantiate", "Minuit");
      
      // register local application to the Minuit protocol
      TTSymbol local_device_name;
      if (!mApplicationManager.get("applicationLocalName", local_device_name))
        protocolMinuit.send("ApplicationRegister", local_device_name);
      else
        TTLogError("Local device doesn't exist\n");
      
      // register the application to the Minuit protocol and set paramaters up
      protocolMinuit.send("Stop");
      protocolMinuit.send("ApplicationRegister", device_name);
      protocolMinuit.send("ApplicationSelect", device_name);
      protocolMinuit.set("port", minuit_protocol->port);
      protocolMinuit.set("ip", TTSymbol(minuit_protocol->ip));
      protocolMinuit.send("Run");
      
      TTLogMessage("Minuit device created\n");
      return;
    }
    
    OSC* osc_protocol = dynamic_cast<OSC*>(&protocol);
    if (osc_protocol)
    {
      // create a distante application
      mApplication = mApplicationManager.send("ApplicationInstantiateDistant", device_name);
      
      // create an OSC protocol unit
      TTObject protocolOSC = mApplicationManager.send("ProtocolFind", "OSC");
      if (!protocolOSC.valid())
        protocolOSC = mApplicationManager.send("ProtocolInstantiate", "OSC");
      
      // register local application to the OSC protocol
      TTSymbol local_device_name;
      if (!mApplicationManager.get("applicationLocalName", local_device_name))
        protocolOSC.send("ApplicationRegister", local_device_name);
      else
        TTLogError("Local device doesn't exist");
      
      // register the application to the OSC protocol and set paramaters up
      protocolOSC.send("Stop");
      protocolOSC.send("ApplicationRegister", device_name);
      protocolOSC.send("ApplicationSelect", device_name);
      TTValue ports(osc_protocol->in_port, osc_protocol->out_port);
      protocolOSC.set("port", ports);
      protocolOSC.set("ip", TTSymbol(osc_protocol->ip));
      protocolOSC.send("Run");
      
      TTLogMessage("OSC device created\n");
      return;
    }
  }
  
  ~JamomaDevice()
  {
    TTSymbol device_name;
    mApplication.get("name", device_name);
    mApplicationManager.send("ApplicationRelease", device_name);
  }

  // Network
  virtual bool updateNamespace() override
  {
    TTErr err = mApplication.send("DirectoryBuild");
    return err == kTTErrNone;
  }

};

shared_ptr<Device> Device::create(Protocol & p, string name)
{
  return shared_ptr<Device>(new JamomaDevice(p, name));
}

/* old code
 
bool Device::save(std::string filepath) const
{
  // create a xml handler
  TTObject aXmlHandler(kTTSym_XmlHandler);
  
  // pass it the application
  aXmlHandler.set(kTTSym_object, pimpl->mApplication);
  
  // write
  TTValue none;
  TTErr err = aXmlHandler.send(kTTSym_Write, filepath, none);
  
  return err == kTTErrNone;
}

bool Device::load(std::string filepath)
{
  // create a xml handler
  TTObject aXmlHandler(kTTSym_XmlHandler);
  
  // pass it the application
  aXmlHandler.set(kTTSym_object, pimpl->mApplication);
  
  // read
  TTValue none;
  TTErr err = aXmlHandler.send(kTTSym_Read, filepath, none);
  
  return err == kTTErrNone;
}
 
*/