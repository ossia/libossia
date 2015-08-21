#include "Network/JamomaDevice.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Device> Device::create(Protocol & protocol, string name)
  {
    TTSymbol device_name(name);
    TTObject applicationManager;
    TTObject application;
    
    //! \todo we shouldn't init each time we create an object ...
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
    //! \todo this is not a good way to do as if a new protocol appears we have to create a case for it here
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
      
      // setup attribute to be cached : all attributes handled by Address class should be declared here
      TTValue args;
      args.append(kTTSym_service);
      args.append(kTTSym_type);
      args.append(kTTSym_rangeBounds);
      args.append(kTTSym_rangeClipmode);
      args.append(kTTSym_repetitionsFilter);
      application.set("cachedAttributes", args);
      
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
      
      //! \todo change Minuit mechanism to setup one out_port per distant device
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

JamomaDevice::JamomaDevice(Protocol & protocol, TTObject applicationManager, TTObject application, TTNodeDirectoryPtr aDirectory) :
JamomaNode(aDirectory, aDirectory->getRoot()),
mApplicationManager(applicationManager),
mApplication(application)
{
  return ;
}

JamomaDevice::~JamomaDevice()
{
  TTSymbol device_name;
  mApplication.get("name", device_name);
  mApplicationManager.send("ApplicationRelease", device_name);
}

# pragma mark -
# pragma mark Network

bool JamomaDevice::updateNamespace()
{
  TTErr err = mApplication.send("DirectoryBuild");
  
  // update root node
  this->mNode = this->mDirectory->getRoot();
  
  // erase all former nodes
  m_children.clear();
  
  // build tree from the root
  buildChildren();
  
  // is there children below ?
  if (children().size() == 0)
    throw runtime_error("namespace empty after the update");
  
  return err == kTTErrNone;
}