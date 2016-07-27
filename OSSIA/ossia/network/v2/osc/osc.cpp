#include <ossia/network/v2/osc/osc.hpp>
#include <ossia/network/v2/base/address.hpp>
#include <ossia/network/v2/domain.hpp>
#include <ossia/network/v2/generic/generic_device.hpp>
#include <ossia/network/v2/generic/generic_address.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/network/v2/osc/detail/osc.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
namespace impl
{
OSC2::OSC2(std::string ip, uint16_t in_port, uint16_t out_port) :
    mIp{ip},
    mInPort{in_port},
    mOutPort{out_port},
    mSender{ip, in_port},
    mReceiver{out_port, [=] (
              const oscpack::ReceivedMessage& m,
              const oscpack::IpEndpointName& ip) {
            this->handleReceivedMessage(m, ip);
          }}
{
    mReceiver.run();
}

OSC2::~OSC2()
{}

const std::string& OSC2::getIp() const
{
    return mIp;
}

OSC2& OSC2::setIp(std::string ip)
{
    mIp = ip;
    mSender = osc::sender{mIp, mInPort};

    return *this;
}

uint16_t OSC2::getInPort() const
{
    return mInPort;
}

OSC2& OSC2::setInPort(uint16_t in_port)
{
    mInPort = in_port;
    mSender = osc::sender{mIp, mInPort};

    return *this;
}

uint16_t OSC2::getOutPort() const
{
    return mOutPort;
}

OSC2& OSC2::setOutPort(uint16_t out_port)
{
    mOutPort = out_port;
    mReceiver = osc::receiver{out_port, [=] (
        const oscpack::ReceivedMessage& m,
        const oscpack::IpEndpointName& ip) {
      this->handleReceivedMessage(m, ip);
    }};
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

bool OSC2::updateChildren(OSSIA::v2::Node& node)
{
    return false;
}

bool OSC2::pullAddressValue(OSSIA::v2::Address& address)
{
    return false;
}

bool OSC2::pushAddressValue(const OSSIA::v2::Address& address)
{
    auto& addr = static_cast<const BasicAddress&>(address);

    if(addr.getAccessMode() == OSSIA::AccessMode::GET)
        return false;

    auto val = filterValue(addr);
    if(val.valid())
    {
        mSender.send(getOSCAddress(address), val);
        return true;
    }
    return false;
}

bool OSC2::observeAddressValue(OSSIA::v2::Address& address, bool enable)
{
    std::lock_guard<std::mutex> lock(mListeningMutex);

    if(enable)
        mListening.insert(std::make_pair(getOSCAddressAsString(address), &address));
    else
        mListening.erase(getOSCAddressAsString(address));

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
        updateValue(addr, m);
    }
}

}
