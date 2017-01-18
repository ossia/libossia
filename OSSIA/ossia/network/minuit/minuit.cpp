#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/minuit/minuit.hpp>

#include <ossia/network/minuit/detail/minuit_common.hpp>
#include <ossia/network/minuit/detail/minuit_parser.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/detail/string_view.hpp>

namespace ossia
{
namespace net
{

static auto get_time()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

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
    , mLastSentMessage{get_time()}
    , mLastReceivedMessage{get_time()}
{
  if(mReceiver->port() != local_port)
  {
    throw ossia::connection_error{"minuit_protocol::minuit_protocol: "
                                  "Could not connect to port: " + boost::lexical_cast<std::string>(local_port)};
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
  mDevice = &dev;
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
      mSender->send(req, ossia::string_view(addr));
    }
    mLastSentMessage = get_time();
  }
  // While messages are being received regularly, we wait.
  mLastReceivedMessage = get_time();
  auto prev_t = mLastReceivedMessage.load();
  bool ok = false;
  while(!ok)
  {
    status = fut.wait_for(std::chrono::milliseconds(100));
    if(mLastReceivedMessage == prev_t)
      ok = true;
    else
      prev_t = mLastReceivedMessage;
  }

  auto check_unfinished = [&] {
    lock_type lock(mNamespaceRequestsMutex);
    return mNamespaceRequests.size() > 0 || mPendingGetRequests > 0  || status != std::future_status::ready;
  };

  if(check_unfinished())
  {
    std::vector<std::string> nreq, greq;
    for(int i = 0; i < 100; i++)
    {
      status = fut.wait_for(std::chrono::milliseconds(250));
      if(!check_unfinished())
      {
          break;
      }
      else
      {
        {
          lock_type lock(mNamespaceRequestsMutex);
          nreq.clear();
          ossia::copy(mNamespaceRequests, nreq);
        }
        for(const auto& req : nreq)
        {
          mSender->send(name_table.get_action(ossia::minuit::minuit_action::NamespaceRequest), ossia::string_view(req));
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        {
          lock_type lock(mGetRequestsMutex);
          greq.clear();
          ossia::copy(mGetRequests, greq);
        }
        for(const auto& req : greq)
        {
          mSender->send(name_table.get_action(ossia::minuit::minuit_action::GetRequest), ossia::string_view(req));
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

      }
    }

  }

  mPendingGetRequests = 0;
  {
    lock_type lock(mNamespaceRequestsMutex);
    for(const auto& node : mNamespaceRequests)
    {
      logger().error("Namespace request unmatched: {0}", node);
    }
    mNamespaceRequests.clear();
  }
  {
    lock_type lock(mGetRequestsMutex);
    for(const auto& node : mGetRequests)
    {
      logger().error("Namespace request unmatched: {0}", node);
    }
    mGetRequests.clear();
  }

  return status == std::future_status::ready || node.children().size() != 0;
}

void minuit_protocol::request(ossia::net::address_base& address)
{
  auto act = name_table.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = ossia::net::get_osc_address_as_string(address);
  addr += ":value";
  this->mSender->send(act, ossia::string_view(addr));
  mLastSentMessage = get_time();
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

bool minuit_protocol::push(const ossia::net::address_base& addr)
{
  if (addr.getAccessMode() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr);
  if (val.valid())
  {
    mSender->send(addr, val);
    mLastSentMessage = get_time();
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

  mLastSentMessage = get_time();

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

void minuit_protocol::namespace_refresh(ossia::string_view req, const std::string& addr)
{
  lock_type lock(mNamespaceRequestsMutex);
  auto it = mNamespaceRequests.find(addr);
  if (it == mNamespaceRequests.end())
  {
    mNamespaceRequests.insert(addr);
    mSender->send(req, ossia::string_view(addr));
    mLastSentMessage = get_time();
  }
}

void minuit_protocol::namespace_refreshed(ossia::string_view addr)
{
  lock_type lock(mNamespaceRequestsMutex);
#if defined(__ANDROID_API__)
  auto it = mNamespaceRequests.find(addr.to_string());
#else
  auto it = mNamespaceRequests.find(addr);
#endif
  if (it != mNamespaceRequests.end())
  {
    mNamespaceRequests.erase(it);
  }

  if (mNamespaceRequests.empty())
  {
    mNamespaceFinishedPromise.set_value();
  }
}

void minuit_protocol::get_refresh(ossia::string_view req, const std::string& addr)
{
  lock_type lock(mGetRequestsMutex);
  auto it = ossia::find(mGetRequests, addr);
  if (it == mGetRequests.end())
  {
    mGetRequests.push_back(addr);
    mPendingGetRequests++;
    mSender->send(req, ossia::string_view(addr));

    mLastSentMessage = get_time();
  }
}

void minuit_protocol::get_refreshed(ossia::string_view addr)
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
    ossia::string_view address{m.AddressPattern()};

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
            update_value_quiet(*base_addr, m);
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

    mLastReceivedMessage = get_time();
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
