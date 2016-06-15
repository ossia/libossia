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


bool JamomaProtocol::pullAddressValue(Address& address) const
{
    JamomaAddress& adrs = dynamic_cast<JamomaAddress&>(address);

    TTValue value;

    if (adrs.pullValue(value))
    {
        adrs.setValue(value);

        if(mLogger)
        {
            auto& cb = mLogger->getInboundLogCallback();
            if(cb)
                cb(adrs.getTextualAddress() + " <<= " + getValueAsString(*adrs.getValue()));
        }
        return true;
    }

    return false;
}

bool JamomaProtocol::pushAddressValue(const Address& address) const
{
    const JamomaAddress& adrs = dynamic_cast<const JamomaAddress&>(address);

    TTValue value;

    adrs.getValue(value);

    bool res = adrs.pushValue(value);

    if(mLogger)
    {
        auto& cb = mLogger->getOutboundLogCallback();
        if(cb)
            cb(adrs.getTextualAddress() + " => " + getValueAsString(*adrs.getValue()));
    }

    return res;
}

bool JamomaProtocol::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
    shared_ptr<JamomaAddress> adrs = dynamic_pointer_cast<JamomaAddress>(address);

    adrs->observeValue(enable);

    return true;
}
