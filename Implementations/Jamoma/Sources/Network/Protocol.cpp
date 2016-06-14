#include "Network/JamomaProtocol.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

JamomaProtocol::JamomaProtocol()
{
  TTFoundationInit("/usr/local/jamoma/extensions/", true);
  TTModularInit("/usr/local/jamoma/extensions", true);

  // if no application manager
  if (TTModularApplicationManager == NULL)
    mApplicationManager = TTObject("ApplicationManager");
  else
      mApplicationManager = TTObjectBasePtr(TTModularApplicationManager);
}

void JamomaProtocol::setLogger(std::shared_ptr<NetworkLogger> l)
{
    mLogger = l;
}

std::shared_ptr<NetworkLogger> JamomaProtocol::getLogger() const
{
    return mLogger;
}

Protocol::~Protocol()
{}

void OSSIA::CleanupProtocols()
{
    TTObject obj = TTObjectBasePtr(TTModularApplicationManager);
    TTValue protocols;
    obj.get("protocolNames", protocols);
    for(TTSymbol key : protocols)
    {
        obj.send("ProtocolRelease", key);
    }
}
