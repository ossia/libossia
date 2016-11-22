#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/minuit/minuit.hpp>

#include <ossia/network/minuit/detail/minuit_common.hpp>
#include <ossia/network/minuit/detail/minuit_parser.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>

namespace ossia
{
namespace net
{
minuit_protocol::minuit_protocol(
    const std::string& local_name, const std::string& remote_ip,
    uint16_t remote_port, uint16_t local_port)
    : mLocalName{local_name}
    , mIp{remote_ip}
    , mRemotePort{remote_port}
    , mLocalPort{local_port}
    , mSender{std::make_unique<osc::sender>(mLogger, remote_ip, remote_port)}
    , mReceiver{std::make_unique<osc::receiver>(local_port, [=](const oscpack::ReceivedMessage& m,
                                const oscpack::IpEndpointName& ip) {
                  this->handleReceivedMessage(m, ip);
                })}
{
  if(mReceiver->port() != local_port)
  {
    throw ossia::connection_error{"minuit_protocol::minuit_protocol: "
                                  "Could not connect to port: " + std::to_string(local_port)};
  }

  name_table.set_device_name(mLocalName);
  mReceiver->run();

  update_zeroconf();
}

minuit_protocol::~minuit_protocol()
{
  mReceiver->stop();
}

void minuit_protocol::setDevice(ossia::net::device_base& dev)
{
  mDevice = dynamic_cast<generic_device*>(&dev);
}

const std::string& minuit_protocol::getIp() const
{
  return mIp;
}

minuit_protocol& minuit_protocol::setIp(std::string ip)
{
  mIp = ip;
  mSender = std::make_unique<osc::sender>(mLogger, mIp, mRemotePort);

  update_zeroconf();

  return *this;
}

uint16_t minuit_protocol::getRemotePort() const
{
  return mRemotePort;
}

minuit_protocol& minuit_protocol::setRemotePort(uint16_t in_port)
{
  mRemotePort = in_port;
  mSender = std::make_unique<osc::sender>(mLogger, mIp, mRemotePort);

  update_zeroconf();

  return *this;
}

uint16_t minuit_protocol::getLocalPort() const
{
  return mLocalPort;
}

minuit_protocol& minuit_protocol::setLocalPort(uint16_t out_port)
{
  mLocalPort = out_port;
  mReceiver = std::make_unique<osc::receiver>(out_port, [=](const oscpack::ReceivedMessage& m,
                                          const oscpack::IpEndpointName& ip) {
                              this->handleReceivedMessage(m, ip);
                            });

  update_zeroconf();

  return *this;
}

bool minuit_protocol::update(ossia::net::node_base& node)
{
  // Reset node
  node.clearChildren();
  node.removeAddress();

  // Send "namespace" request
  mNamespaceFinishedPromise = std::promise<void>{};
  auto fut = mNamespaceFinishedPromise.get_future();

  auto act
      = name_table.get_action(ossia::minuit::minuit_action::NamespaceRequest);
  namespace_refresh(act, ossia::net::get_osc_address_as_string(node));

  auto status = fut.wait_for(std::chrono::seconds(5));
  // Won't return as long as the tree exploration request haven't finished.

  // If there are still un-explored nodes, we go for a second round
  if(status != std::future_status::ready)
  {
    auto req = name_table.get_action(minuit::minuit_action::NamespaceRequest);
    lock_type lock(mNamespaceRequestsMutex);
    for(const auto& addr : mNamespaceRequests)
    {
      mSender->send(req, boost::string_view(addr));
    }
  }
  status = fut.wait_for(std::chrono::seconds(5));

  using namespace std::chrono;

  // Then we wait for the "get" requests
  if(mPendingGetRequests > 0 || status != std::future_status::ready)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto t1 = high_resolution_clock::now();
    while(true)
    {
      auto t2 = std::chrono::high_resolution_clock::now();
      if(duration_cast<milliseconds>(t2 - t1).count() > 4000) {
        break;
      }
      if(mPendingGetRequests == 0) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }

  mPendingGetRequests = 0;
  { lock_type lock(mNamespaceRequestsMutex); mNamespaceRequests.clear(); }
  { lock_type lock(mGetRequestsMutex); mGetRequests.clear(); }

  return status == std::future_status::ready || node.children().size() != 0;
}

void minuit_protocol::request(ossia::net::address_base& address)
{
  auto act = name_table.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = ossia::net::get_osc_address_as_string(address);
  addr += ":value";
  this->mSender->send(act, boost::string_view(addr));
}

std::future<void> minuit_protocol::pullAsync(address_base& address)
{
  // Send "get" request
  mGetFinishedPromise = std::promise<void>();
  auto fut = mGetFinishedPromise.get_future();

  auto act = name_table.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = ossia::net::get_osc_address_as_string(address);
  addr += ":value";

  get_refresh(act, addr);

  return fut;
}

bool minuit_protocol::pull(ossia::net::address_base& address)
{
  auto fut = pullAsync(address);

  fut.wait_for(std::chrono::milliseconds(25));

  return fut.valid();
}

bool minuit_protocol::push(const ossia::net::address_base& address)
{
  auto& addr = static_cast<const generic_address&>(address);

  if (addr.getAccessMode() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr);
  if (val.valid())
  {
    mSender->send(address, val);
    return true;
  }

  return false;
}

bool minuit_protocol::observe(ossia::net::address_base& address, bool enable)
{
  lock_type lock(mListeningMutex);

  auto act
      = name_table.get_action(ossia::minuit::minuit_action::ListenRequest);

  if (enable)
  {
    this->mSender->send(act, address, "enable");
    mListening.insert(
        std::make_pair(get_osc_address_as_string(address), &address));
  }
  else
  {
    this->mSender->send(act, address, "disable");
    mListening.erase(get_osc_address_as_string(address));
  }

  return true;
}

bool minuit_protocol::observe_quietly(
    ossia::net::address_base& address, bool enable)
{
  lock_type lock(mListeningMutex);

  if(enable)
    mListening.insert(
          std::make_pair(get_osc_address_as_string(address), &address));
  else
    mListening.erase(get_osc_address_as_string(address));

  return true;
}

void minuit_protocol::namespace_refresh(boost::string_view req, const std::string& addr)
{
  lock_type lock(mNamespaceRequestsMutex);
  auto it = mNamespaceRequests.find(addr);
  if (it == mNamespaceRequests.end())
  {
    mNamespaceRequests.insert(addr);
    mSender->send(req, boost::string_view(addr));
  }
}

void minuit_protocol::namespace_refreshed(boost::string_view addr)
{
  lock_type lock(mNamespaceRequestsMutex);
  auto it = mNamespaceRequests.find(addr);
  if (it != mNamespaceRequests.end())
  {
    mNamespaceRequests.erase(it);
  }

  if (mNamespaceRequests.empty())
  {
    mNamespaceFinishedPromise.set_value();
  }
}

void minuit_protocol::get_refresh(boost::string_view req, const std::string& addr)
{
  lock_type lock(mGetRequestsMutex);
  auto it = ossia::find(mGetRequests, addr);
  if (it == mGetRequests.end())
  {
    mGetRequests.push_back(addr);
    mPendingGetRequests++;
    mSender->send(req, boost::string_view(addr));
  }
}

void minuit_protocol::get_refreshed(boost::string_view addr)
{
  lock_type lock(mGetRequestsMutex);
  auto it = ossia::find(mGetRequests, addr);
  if (it != mGetRequests.end())
  {
    mGetRequests.erase(it);
    mPendingGetRequests--;
  }

  if (mGetRequests.empty())
  {
    try { mGetFinishedPromise.set_value(); } catch(...) { }
  }
}

osc::sender& minuit_protocol::sender() const
{
    return *mSender;
}

struct minuit_task
{
  std::function<void()> handle;
};

void minuit_protocol::handleReceivedMessage(
        const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
    boost::string_view address{m.AddressPattern()};

    if (address.size() > 0 && address[0] == '/')
    {
      // Handle the OSC-like case where we receive a plain value.
      lock_type lock(mListeningMutex);
      auto it = mListening.find(address.to_string());
      if (it != mListening.end())
      {
        ossia::net::address_base& addr = *it->second;

        update_value(addr, m);
      }
      else
      {
        // We still want to save the value even if it is not listened to.
        if(auto n = find_node(mDevice->getRootNode(), address))
        {
          if(auto base_addr = n->getAddress())
          {
            auto addr = static_cast<ossia::net::generic_address*>(base_addr);
            update_value_quiet(*addr, m);
          }
        }
      }
    }
    else
    {
      if (mDevice)
        ossia::minuit::minuit_message_handler::handleMinuitMessage(
              *this, *mDevice, address, m);
    }

    if(mLogger.inbound_logger)
      mLogger.inbound_logger->info("In: {0}", m);
}

void minuit_protocol::update_zeroconf()
{
  mZeroconfServer = make_zeroconf_server(
                      mLocalName + " Minuit server",
                      "_minuit._tcp",
                      mLocalName, mLocalPort, mRemotePort);
}
}
}
