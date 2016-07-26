#include "BasicDevice.h"
#include "BasicLocal.h"
#include "BasicMinuit.h"
#include "BasicOSC.h"

namespace impl
{
BasicDevice::BasicDevice(
        std::unique_ptr<OSSIA::v2::Protocol> protocol,
        std::string name) :
    Device(std::move(protocol)),
    BasicNode(std::move(name), *this)
{
}

BasicDevice::~BasicDevice()
{
    mChildren.clear();
}
}
