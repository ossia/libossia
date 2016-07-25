#include "BasicDevice.h"
#include "BasicLocal.h"
#include "BasicMinuit.h"
#include "BasicOSC.h"

namespace impl
{
BasicDevice::BasicDevice(std::unique_ptr<OSSIA::v2::Protocol> protocol) :
    BasicNode({}, {}),
    mProtocol(std::move(protocol))
{
    mIsDevice = true;
}

BasicDevice::~BasicDevice()
{
    m_children.clear();
}

OSSIA::v2::Protocol& BasicDevice::getProtocol() const
{
    return *mProtocol;
}

//! \deprecated use Protocol::updateChildren
bool BasicDevice::updateNamespace()
{
    return mProtocol->updateChildren(*this);
}
}
