#include "BasicOSC.h"
#include "Address.hpp"
#include "Domain.hpp"
#include "BasicAddress.h"
#include <Editor/Value/Value.h>

#include <BasicDevice.h>
#include <OSC/Osc.hpp>
namespace impl
{

OSC2::OSC2(std::string ip, uint16_t in_port, uint16_t out_port) :
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

uint16_t OSC2::getInPort() const
{
    return mInPort;
}

OSC2& OSC2::setInPort(uint16_t in_port)
{
    mInPort = in_port;
    return *this;
}

uint16_t OSC2::getOutPort() const
{
    return mOutPort;
}

OSC2& OSC2::setOutPort(uint16_t out_port)
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

static OSSIA::Value filterValue(
        const OSSIA::v2::Domain& dom,
        const OSSIA::Value& base_val,
        OSSIA::BoundingMode mode)
{
    if(dom)
    {
        auto res = OSSIA::v2::clamp(dom, mode, base_val);
        if(res.valid())
            return res;
        else
            return {};
    }
    else
    {
        return base_val;
    }
}

static OSSIA::Value filterValue(const BasicAddress& addr)
{
    if(addr.getRepetitionFilter() &&
       addr.getValue() == addr.PreviousValue)
        return {};

    return filterValue(addr.getDomain(), addr.cloneValue(), addr.getBoundingMode());
}

bool OSC2::pushAddressValue(const OSSIA::v2::Address& address) const
{
    auto& addr = static_cast<const BasicAddress&>(address);

    if(addr.getAccessMode() == OSSIA::AccessMode::GET)
        return false;

    auto val = filterValue(addr);
    if(val.valid())
    {
        mSender.send(address.getTextualAddress(), val);
    }
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
    std::unique_lock<std::mutex> lock(mListeningMutex);
    auto it = mListening.find(m.AddressPattern());
    if(it != mListening.end())
    {
        OSSIA::v2::Address& addr = *it->second;
        lock.unlock();
        if(addr.getAccessMode() == OSSIA::AccessMode::SET)
            return;

        auto res = filterValue(
                    addr.getDomain(),
                    toValue(m, addr.cloneValue()),
                    addr.getBoundingMode());

        if(res.valid())
        {
            addr.setValue(std::move(res));
            try {
                addr.send(addr.cloneValue());
            } catch(...) { }
        }
    }
}

}
