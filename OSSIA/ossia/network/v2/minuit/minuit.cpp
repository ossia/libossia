#include <ossia/network/v2/minuit/minuit.hpp>
#include <ossia/network/v2/generic/generic_address.hpp>

#include <ossia/network/v2/osc/detail/osc.hpp>
#include <ossia/network/v2/minuit/detail/minuit_parser.hpp>

namespace impl
{

Minuit2::Minuit2(std::string ip, uint16_t in_port, uint16_t out_port) :
    mIp{ip},
    mInPort{in_port},
    mOutPort{out_port},
    mSender{ip, in_port},
    mReceiver{out_port, [=] (auto&&... var) {
        this->handleReceivedMessage(std::forward<decltype(var)>(var)...);
    }}
{
    mReceiver.run();
}

Minuit2::~Minuit2()
{}

void Minuit2::setDevice(OSSIA::v2::Device& dev)
{
  mDevice = dynamic_cast<BasicDevice*>(&dev);
  mNameTable.set_device_name(dev.getName());
}

const std::string& Minuit2::getIp() const
{
    return mIp;
}

Minuit2& Minuit2::setIp(std::string ip)
{
    mIp = ip;
    mSender = osc::sender{mIp, mInPort};

    return *this;
}

uint16_t Minuit2::getInPort() const
{
    return mInPort;
}

Minuit2& Minuit2::setInPort(uint16_t in_port)
{
    mInPort = in_port;
    mSender = osc::sender{mIp, mInPort};

    return *this;
}

uint16_t Minuit2::getOutPort() const
{
    return mOutPort;
}

Minuit2& Minuit2::setOutPort(uint16_t out_port)
{
    mOutPort = out_port;
    mReceiver = osc::receiver{out_port, [=] (auto&&... var) {
        this->handleReceivedMessage(std::forward<decltype(var)>(var)...);
    }};
    return *this;
}

bool Minuit2::updateChildren(OSSIA::v2::Node& node)
{
    // Reset node
    node.clearChildren();
    node.removeAddress();

    // Send "namespace" request
    mNamespacePromise = std::promise<void>{};
    auto fut = mNamespacePromise.get_future();

    auto act = mNameTable.get_action(OSSIA::minuit::minuit_action::NamespaceRequest);
    mSender.send(act, "/"); // TODO sub-nodes

    fut.wait();
    // Won't return as long as the request hasn't finished.
    return false;
}

bool Minuit2::pullAddressValue(OSSIA::v2::Address& address)
{
    // Send "get" request
    return false;
}


bool Minuit2::pushAddressValue(const OSSIA::v2::Address& address)
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

bool Minuit2::observeAddressValue(OSSIA::v2::Address& address, bool enable)
{
    std::lock_guard<std::mutex> lock(mListeningMutex);

    auto act = mNameTable.get_action(OSSIA::minuit::minuit_action::ListenRequest);

    if(enable)
    {
        this->mSender.send(act, getOSCAddress(address), "enable");
        mListening.insert(std::make_pair(getOSCAddressAsString(address), &address));
    }
    else
    {
        this->mSender.send(act, getOSCAddress(address), "disable");
        mListening.erase(getOSCAddressAsString(address));
    }

    return true;
}

void Minuit2::handleReceivedMessage(
        const oscpack::ReceivedMessage& m,
        const oscpack::IpEndpointName& ip)
{
    boost::string_ref address{m.AddressPattern()};

    if(address.size() > 0 && address[0] == '/')
    {
        // Handle the OSC-like case where we receive a plain value.
        std::unique_lock<std::mutex> lock(mListeningMutex);
        auto it = mListening.find(m.AddressPattern());
        if(it != mListening.end())
        {
            OSSIA::v2::Address& addr = *it->second;
            lock.unlock();

            updateValue(addr, m);
        }
    }
    else
    {
        OSSIA::minuit::minuit_message_handler h;
        h.handleMinuitMessage(*this, *mDevice, address, m);
    }
}
}
