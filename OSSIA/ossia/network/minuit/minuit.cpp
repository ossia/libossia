#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
#include <ossia/network/minuit/detail/minuit_parser.hpp>

namespace ossia
{
namespace net
{
minuit_protocol::minuit_protocol(std::string ip, uint16_t in_port, uint16_t out_port) :
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

minuit_protocol::~minuit_protocol()
{}

void minuit_protocol::setDevice(ossia::net::device_base& dev)
{
  mDevice = dynamic_cast<generic_device*>(&dev);
  mLocalNameTable.set_device_name("i-score");
  mRemoteNameTable.set_device_name(dev.getName());
}

const std::string& minuit_protocol::getIp() const
{
  return mIp;
}

minuit_protocol& minuit_protocol::setIp(std::string ip)
{
  mIp = ip;
  mSender = osc::sender{mIp, mInPort};

  return *this;
}

uint16_t minuit_protocol::getInPort() const
{
  return mInPort;
}

minuit_protocol& minuit_protocol::setInPort(uint16_t in_port)
{
  mInPort = in_port;
  mSender = osc::sender{mIp, mInPort};

  return *this;
}

uint16_t minuit_protocol::getOutPort() const
{
  return mOutPort;
}

minuit_protocol& minuit_protocol::setOutPort(uint16_t out_port)
{
  mOutPort = out_port;
  mReceiver = osc::receiver{out_port, [=] (
      const oscpack::ReceivedMessage& m,
      const oscpack::IpEndpointName& ip) {
    this->handleReceivedMessage(m, ip);
  }};
  return *this;
}

bool minuit_protocol::update(ossia::net::node_base& node)
{
  // Reset node
  node.clearChildren();
  node.removeAddress();

  // Send "namespace" request
  mNamespacePromise = std::promise<void>{};
  auto fut = mNamespacePromise.get_future();

  auto act = mLocalNameTable.get_action(ossia::minuit::minuit_action::NamespaceRequest);
  refresh(act, ossia::net::getOSCAddressAsString(node));

  fut.wait_for(std::chrono::seconds(5));
  // Won't return as long as the request hasn't finished.
  return true;
}

bool minuit_protocol::pull(ossia::net::address_base& address)
{
  // Send "get" request
  mGetPromise = std::promise<void>();
  auto fut = mGetPromise.get_future();

  auto act = mLocalNameTable.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = ossia::net::getOSCAddressAsString(address);
  this->mSender.send(act, boost::string_ref(addr));

  fut.wait_for(std::chrono::seconds(5));
  return fut.valid();
}


bool minuit_protocol::push(const ossia::net::address_base& address)
{
  auto& addr = static_cast<const generic_address&>(address);

  if(addr.getAccessMode() == ossia::access_mode::GET)
    return false;

  auto val = filterValue(addr);
  if(val.valid())
  {
    mSender.send(getOSCAddress(address), val);
    return true;
  }

  return false;
}

bool minuit_protocol::observe(ossia::net::address_base& address, bool enable)
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

void minuit_protocol::handleReceivedMessage(
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
      ossia::net::address_base& addr = *it->second;
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
}
