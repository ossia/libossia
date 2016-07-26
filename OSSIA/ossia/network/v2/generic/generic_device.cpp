#include "BasicDevice.hpp"
#include "BasicLocal.hpp"
#include "BasicMinuit.hpp"
#include "BasicOSC.hpp"

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
