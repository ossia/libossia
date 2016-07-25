#include "BasicProtocol.h"

namespace impl
{
BasicProtocol::BasicProtocol()
{
}

void BasicProtocol::setLogger(std::shared_ptr<OSSIA::NetworkLogger> l)
{
    mLogger = l;
}

std::shared_ptr<OSSIA::NetworkLogger> BasicProtocol::getLogger() const
{
    return mLogger;
}

bool BasicProtocol::pullAddressValue(OSSIA::Address2& address) const
{
    return false;
}

bool BasicProtocol::pushAddressValue(const OSSIA::Address2& address) const
{
    return false;
}

bool BasicProtocol::observeAddressValue(OSSIA::Address2& address, bool enable) const
{
    return false;
}
}
