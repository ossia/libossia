#include "BasicOSC.h"
#include "Address.hpp"
#include <Editor/Value/Value.h>

#include <BasicDevice.h>
#include <OSC/Osc.hpp>
namespace impl
{

OSC2::OSC2(std::string ip, int in_port, int out_port) :
    mIp{std::move(ip)},
    mInPort{in_port},
    mOutPort{out_port},
    mSender{ip, out_port},
    mReceiver{in_port, [=] (
              const oscpack::ReceivedMessage& m,
              const oscpack::IpEndpointName& ip) {
    handleReceivedMessage(m, ip);
}}
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
    mSender.send(address.getTextualAddress(), address.cloneValue());
    return false;
}

bool OSC2::observeAddressValue(OSSIA::v2::Address& address, bool enable) const
{
    std::lock_guard<std::mutex> lock(mListeningMutex);

    if(enable)
        mListening.insert(std::make_pair(address.getTextualAddress(), &address));
    else
        mListening.erase(address.getTextualAddress());

    return true;
}

void OSC2::handleReceivedMessage(
        const oscpack::ReceivedMessage& m,
        const oscpack::IpEndpointName& ip)
{
    std::lock_guard<std::mutex> lock(mListeningMutex);
    auto it = mListening.find(m.AddressPattern());
    if(it != mListening.end())
    {
        OSSIA::v2::Address& addr = *it->second;
        addr.setValue(toValue(m, addr.cloneValue()));
        try {
            addr.send(addr.cloneValue());
        } catch(...) { }
    }
}

}
