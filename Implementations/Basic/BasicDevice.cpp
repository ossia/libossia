#include "BasicDevice.h"
#include "BasicLocal.h"
#include "BasicMinuit.h"
#include "BasicOSC.h"

namespace impl
{
BasicDevice::BasicDevice(
        std::string name,
        std::unique_ptr<OSSIA::v2::Protocol> protocol) :
    BasicNode(std::move(name), *this),
    mProtocol{std::move(protocol)}
{
}

BasicDevice::~BasicDevice()
{
    mChildren.clear();
}

OSSIA::v2::Protocol& BasicDevice::getProtocol() const
{
    return *mProtocol;
}

}
