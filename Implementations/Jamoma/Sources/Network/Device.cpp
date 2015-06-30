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

# pragma mark -
# pragma mark Implementation specific
  
  TTObject mApplicationManager;
  TTObject mApplication;

public:

# pragma mark -
# pragma mark Life cycle
  
  JamomaDevice(Protocol & protocol, TTObject applicationManager = TTObject(), TTObject application = TTObject(), TTNodeDirectoryPtr aDirectory = nullptr) :
  JamomaNode(aDirectory, aDirectory->getRoot()),
  mApplicationManager(applicationManager),
  mApplication(application)
  {
    return ;
  }

  ~JamomaDevice()
  {
    TTSymbol device_name;
    mApplication.get("name", device_name);
    mApplicationManager.send("ApplicationRelease", device_name);
  }

# pragma mark -
# pragma mark Network
  
  virtual bool updateNamespace() override
  {
    TTErr err = mApplication.send("DirectoryBuild");
    
    // update root node
    this->mNode = this->mDirectory->getRoot();
    
    // todo : erase all former nodes

    // build tree from the root
    buildChildren();
    
    // is there children below ?
    if (children().size() == 0)
      throw runtime_error("namespace empty after the update");

    return err == kTTErrNone;
  }
};

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Device> Device::create(Protocol & protocol, string name)
  {
    TTSymbol device_name(name);
    TTObject applicationManager;
    TTObject application;
    
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/extensions/", true);
    TTModularInit("/usr/local/jamoma/extensions", true);
    
    // if no application manager
    if (TTModularApplicationManager == NULL)
      applicationManager = TTObject("ApplicationManager");
    else
      applicationManager = TTObjectBasePtr(TTModularApplicationManager);
    
    // is the application already exist ?
    application = applicationManager.send("ApplicationFind", device_name);
    if (application.valid())
    {
      throw runtime_error("the device already exist");
      return nullptr;
    }
    
    // which protocol is it ?
    // todo: this is not a good way to do as if a new protocol appears we have to create a case for it here
    Local* local_protocol = dynamic_cast<Local*>(&protocol);
    if (local_protocol)
    {
      // create a local application
      application = applicationManager.send("ApplicationInstantiateLocal", device_name);
      
      // create
      TTLogMessage("Local device created\n");
      
      TTValue v;
      application.get("directory", v);
      return make_shared<JamomaDevice>(protocol, applicationManager, application, TTNodeDirectoryPtr(TTPtr(v[0])));
    }
    
    Minuit* minuit_protocol = dynamic_cast<Minuit*>(&protocol);
    if (minuit_protocol)
    {
      // create a distant application
      application = applicationManager.send("ApplicationInstantiateDistant", device_name);
      
      // create a Minuit protocol unit
      TTObject protocolMinuit = applicationManager.send("ProtocolFind", "Minuit");
      if (!protocolMinuit.valid())
        protocolMinuit = applicationManager.send("ProtocolInstantiate", "Minuit");
      
      // register local application to the Minuit protocol
      TTSymbol local_device_name;
      if (!applicationManager.get("applicationLocalName", local_device_name))
        protocolMinuit.send("ApplicationRegister", local_device_name);
      else
        throw runtime_error("Local device doesn't exist");
      
      // register the application to the Minuit protocol and set parameters up
      protocolMinuit.send("Stop");
      protocolMinuit.send("ApplicationRegister", device_name);
      protocolMinuit.send("ApplicationSelect", device_name);
      protocolMinuit.set("port", minuit_protocol->in_port);
      protocolMinuit.set("ip", TTSymbol(minuit_protocol->ip));
      
      // todo : change Minuit mechanism to setup one out_port per distant device
      protocolMinuit.send("ApplicationSelect", local_device_name);
      protocolMinuit.set("port", minuit_protocol->out_port);
      
      protocolMinuit.send("Run");
      
      TTLogMessage("Minuit device created\n");
      
      TTValue v;
      application.get("directory", v);
      return make_shared<JamomaDevice>(protocol, applicationManager, application, TTNodeDirectoryPtr(TTPtr(v[0])));
    }
    
    OSC* osc_protocol = dynamic_cast<OSC*>(&protocol);
    if (osc_protocol)
    {
      // create a distante application
      application = applicationManager.send("ApplicationInstantiateDistant", device_name);
      
      // create an OSC protocol unit
      TTObject protocolOSC = applicationManager.send("ProtocolFind", "OSC");
      if (!protocolOSC.valid())
        protocolOSC = applicationManager.send("ProtocolInstantiate", "OSC");
      
      // register local application to the OSC protocol
      TTSymbol local_device_name;
      if (!applicationManager.get("applicationLocalName", local_device_name))
        protocolOSC.send("ApplicationRegister", local_device_name);
      else
        throw runtime_error("Local device doesn't exist");
      
      // register the application to the OSC protocol and set paramaters up
      protocolOSC.send("Stop");
      protocolOSC.send("ApplicationRegister", device_name);
      protocolOSC.send("ApplicationSelect", device_name);
      TTValue ports(osc_protocol->in_port, osc_protocol->out_port);
      protocolOSC.set("port", ports);
      protocolOSC.set("ip", TTSymbol(osc_protocol->ip));
      protocolOSC.send("Run");
      
      TTLogMessage("OSC device created\n");
      
      TTValue v;
      application.get("directory", v);
      return make_shared<JamomaDevice>(protocol, applicationManager, application, TTNodeDirectoryPtr(TTPtr(v[0])));
    }
    
    return nullptr;
  }
}

/* old code

bool Device::save(string filepath) const
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

bool Device::load(string filepath)
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
