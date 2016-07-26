#include <ossia/network/v1/detail/JamomaDevice.hpp>
#include <ossia/network/v1/Protocol/detail/JamomaLocal.hpp>
#include <ossia/network/v1/Protocol/detail/JamomaMinuit.hpp>
#include <ossia/network/v1/Protocol/detail/JamomaOSC.hpp>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<Device> Device::create(std::shared_ptr<Protocol> protocol, string name)
  {
    TTSymbol device_name(name);
    TTObject applicationManager;
    TTObject application;

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
      throw std::runtime_error("the device already exist");
      return nullptr;
    }

    // which protocol is it ?
    //! \todo this is not a good way to do as if a new protocol appears we have to create a case for it here
    std::shared_ptr<JamomaLocal> local_protocol = std::dynamic_pointer_cast<JamomaLocal>(protocol);
    if (local_protocol)
    {
      // create a local application
      application = applicationManager.send("ApplicationInstantiateLocal", device_name);

      // create
      TTLogMessage("Local device created\n");

      TTValue v;
      application.get("directory", v);

      std::shared_ptr<JamomaDevice> device = std::make_shared<JamomaDevice>(protocol, applicationManager, application, TTNodeDirectoryPtr(TTPtr(v[0])));

       // as it is not possible to call shared_from_this() into the constructor
      device->setDevice(device);

      return device;
    }

    std::shared_ptr<JamomaMinuit> minuit_protocol = std::dynamic_pointer_cast<JamomaMinuit>(protocol);
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
        throw std::runtime_error("Local device doesn't exist");

      // register the application to the Minuit protocol and set parameters up
      protocolMinuit.send("Stop");
      protocolMinuit.send("ApplicationRegister", device_name);
      protocolMinuit.send("ApplicationSelect", device_name);
      protocolMinuit.set("port", minuit_protocol->getInPort());
      protocolMinuit.set("ip", TTSymbol(minuit_protocol->getIp()));

      //! \todo change Minuit mechanism to setup one out_port per distant device
      protocolMinuit.send("ApplicationSelect", local_device_name);
      protocolMinuit.set("port", minuit_protocol->getOutPort());

      protocolMinuit.send("Run");

      TTLogMessage("Minuit device created\n");

      TTValue v;
      application.get("directory", v);

      std::shared_ptr<JamomaDevice> device = std::make_shared<JamomaDevice>(protocol, applicationManager, application, TTNodeDirectoryPtr(TTPtr(v[0])));

      // as it is not possible to call shared_from_this() into the constructor
      device->setDevice(device);

      return device;
    }

    std::shared_ptr<JamomaOSC> osc_protocol = std::dynamic_pointer_cast<JamomaOSC>(protocol);
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
        throw std::runtime_error("Local device doesn't exist");

      // register the application to the OSC protocol and set paramaters up
      protocolOSC.send("Stop");
      protocolOSC.send("ApplicationRegister", device_name);
      protocolOSC.send("ApplicationSelect", device_name);
      TTValue ports(osc_protocol->getInPort(), osc_protocol->getOutPort());
      protocolOSC.set("port", ports);
      protocolOSC.set("ip", TTSymbol(osc_protocol->getIp()));
      protocolOSC.send("Run");

      TTLogMessage("OSC device created\n");

      TTValue v;
      application.get("directory", v);

      std::shared_ptr<JamomaDevice> device = std::make_shared<JamomaDevice>(protocol, applicationManager, application, TTNodeDirectoryPtr(TTPtr(v[0])));

      // as it is not possible to call shared_from_this() into the constructor
      device->setDevice(device);

      return device;
    }

    return nullptr;
  }
}

JamomaDevice::JamomaDevice(std::shared_ptr<Protocol> protocol, TTObject applicationManager, TTObject application, TTNodeDirectoryPtr aDirectory) :
JamomaNode(aDirectory, aDirectory->getRoot()),
mProtocol(protocol),
mApplicationManager(applicationManager),
mApplication(application)
{
  mIsDevice = true;
  if (mNode->getObject() == nullptr)
    mNode->setObject(application);
}

JamomaDevice::~JamomaDevice()
{
  m_children.clear();
  TTSymbol device_name;
  mApplication.get("name", device_name);
  mApplicationManager.send("ApplicationRelease", device_name);
}

# pragma mark -
# pragma mark Accessors

string JamomaDevice::getName() const
{
  TTSymbol device_name;
  mApplication.get("name", device_name);
  return device_name.c_str();
}

Node & JamomaDevice::setName(std::string name)
{
  mApplication.set("name", TTSymbol(name));

  return *this;
}

shared_ptr<Protocol> JamomaDevice::getProtocol() const
{
  return mProtocol;
}

//! \deprecated use Protocol::updateChildren
bool JamomaDevice::updateNamespace()
{
  return mProtocol->updateChildren(*this);
}

# pragma mark -
# pragma mark Implementation specific

void JamomaDevice::setDevice(std::shared_ptr<Device> device)
{
  mDevice = device;
}
