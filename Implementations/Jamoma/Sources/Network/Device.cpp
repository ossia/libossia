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
  
  // Implemenatation specific
  TTObject mApplicationManager;
  TTObject mApplication;
  
public:

  // Constructor, destructor
  JamomaDevice(Protocol & protocol, string name = "") : JamomaNode(name)
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    
    // if no application manager
    if (TTModularApplicationManager == NULL)
      mApplicationManager = TTObject("ApplicationManager");
    else
      mApplicationManager = TTObjectBasePtr(TTModularApplicationManager);
    
    // which protocol is it ?
    // todo: this is not a good way to do as if a new protocol appears we have to create a case for it here
    Local* local_protocol = dynamic_cast<Local*>(&protocol);
    if (local_protocol)
    {
      TTLogMessage("Local device created");
      return;
    }
    
    Minuit* minuit_protocol = dynamic_cast<Minuit*>(&protocol);
    if (minuit_protocol)
    {
      TTLogMessage("Minuit device created");
      return;
    }
    
    OSC* osc_protocol = dynamic_cast<OSC*>(&protocol);
    if (osc_protocol)
    {
      TTLogMessage("OSC device created");
      return;
    }
  }
  
  ~JamomaDevice()
  {}

  // Network
  virtual bool updateNamespace() override
  {
    return false;
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