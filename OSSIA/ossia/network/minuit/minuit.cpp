#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
#include <ossia/network/minuit/detail/minuit_parser.hpp>

namespace impl
{

Minuit2::Minuit2(std::string ip, uint16_t in_port, uint16_t out_port) :
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

Minuit2::~Minuit2()
{}

void Minuit2::setDevice(ossia::net::Device& dev)
{
  mDevice = dynamic_cast<BasicDevice*>(&dev);
  mLocalNameTable.set_device_name("i-score");
  mRemoteNameTable.set_device_name(dev.getName());
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
  mReceiver = osc::receiver{out_port, [=] (
      const oscpack::ReceivedMessage& m,
      const oscpack::IpEndpointName& ip) {
    this->handleReceivedMessage(m, ip);
  }};
  return *this;
}

bool Minuit2::update(ossia::net::Node& node)
{
  // Reset node
  node.clearChildren();
  node.removeAddress();

  // Send "namespace" request
  mNamespacePromise = std::promise<void>{};
  auto fut = mNamespacePromise.get_future();

  auto act = mLocalNameTable.get_action(ossia::minuit::minuit_action::NamespaceRequest);
  refresh(act, impl::getOSCAddressAsString(node));

  fut.wait_for(std::chrono::seconds(5));
  // Won't return as long as the request hasn't finished.
  return true;
}

bool Minuit2::pull(ossia::net::address& address)
{
  // Send "get" request
  mGetPromise = std::promise<void>();
  auto fut = mGetPromise.get_future();

  auto act = mLocalNameTable.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = impl::getOSCAddressAsString(address);
  this->mSender.send(act, boost::string_ref(addr));

  fut.wait_for(std::chrono::seconds(5));
  return fut.valid();
}


bool Minuit2::push(const ossia::net::address& address)
{
  auto& addr = static_cast<const BasicAddress&>(address);

  if(addr.getAccessMode() == ossia::AccessMode::GET)
    return false;

  auto val = filterValue(addr);
  if(val.valid())
  {
    mSender.send(getOSCAddress(address), val);
    return true;
  }

  return false;
}

bool Minuit2::observe(ossia::net::address& address, bool enable)
{
  std::lock_guard<std::mutex> lock(mListeningMutex);

  auto act = mLocalNameTable.get_action(ossia::minuit::minuit_action::ListenRequest);

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
      ossia::net::address& addr = *it->second;
      lock.unlock();

      updateValue(addr, m);
    }
  }
  else
  {
    ossia::minuit::minuit_message_handler h;
    h.handleMinuitMessage(*this, *mDevice, address, m);
  }
}
}
