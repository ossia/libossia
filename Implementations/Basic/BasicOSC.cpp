#include "BasicOSC.h"

#include <BasicDevice.h>

namespace impl
{

OSC2::OSC2(std::string ip, int in_port, int out_port) :
    mIp(ip),
    mInPort(in_port),
    mOutPort(out_port)
{
}

OSC2::~OSC2()
{}


std::string OSC2::getIp() const
{
    return mIp;
}

OSC2& OSC2::setIp(std::string ip)
{
    mIp = ip;

    return *this;
}

int OSC2::getInPort() const
{
    return mInPort;
}

OSC2& OSC2::setInPort(int in_port)
{
    mInPort = in_port;
    return *this;
}

int OSC2::getOutPort() const
{
    return mOutPort;
}

OSC2& OSC2::setOutPort(int out_port)
{
    mOutPort = out_port;
    return *this;
}

bool OSC2::getLearningStatus() const
{
    return mLearning;
}


OSC2& OSC2::setLearningStatus(OSSIA::v2::Device& ossiaDevice, bool newLearn)
{
    return *this;
}

bool OSC2::updateChildren(OSSIA::v2::Node& node) const
{
    return false;
}

bool OSC2::pullAddressValue(OSSIA::v2::Address& address) const
{
    return false;
}

bool OSC2::pushAddressValue(const OSSIA::v2::Address& address) const
{
    return false;
}

bool OSC2::observeAddressValue(OSSIA::v2::Address& address, bool enable) const
{
    return false;
}

}
