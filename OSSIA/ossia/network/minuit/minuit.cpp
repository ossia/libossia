#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/minuit/minuit.hpp>

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
    , mSender{std::make_unique<osc::sender>(remote_ip, remote_port)}
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
  mSender = std::make_unique<osc::sender>(mIp, mRemotePort);

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
  mSender = std::make_unique<osc::sender>(mIp, mRemotePort);

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
  mNamespacePromise = std::promise<void>{};
  auto fut = mNamespacePromise.get_future();

  auto act
      = name_table.get_action(ossia::minuit::minuit_action::NamespaceRequest);
  refresh(act, ossia::net::get_osc_address_as_string(node));

  auto status = fut.wait_for(std::chrono::seconds(5));
  // Won't return as long as the tree exploration request haven't finished.
  using namespace std::chrono;

  // Then we wait for the "get" requests
  auto t1 = high_resolution_clock::now();
  while(true)
  {
    auto t2 = std::chrono::high_resolution_clock::now();
    if(duration_cast<milliseconds>(t2 - t1).count() > 5000) {
      break;
    }
    if(pending_get_requests == 0) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return status == std::future_status::ready || node.children().size() != 0;
}

struct scope_timer
{
    std::chrono::high_resolution_clock::time_point t1, t2;
    scope_timer()
    {
      t1 = std::chrono::high_resolution_clock::now();
    }
    ~scope_timer()
    {
      using namespace std::chrono;
      t2 = high_resolution_clock::now();
      std::cerr <<  duration_cast<milliseconds>(t2 - t1).count() << "\n";
    }
};

bool minuit_protocol::pull(ossia::net::address_base& address)
{
  // Send "get" request
  // TODO put them in a set to check that we get the correct reply
  // and not some other random get answer
  get_promise = std::promise<void>();
  auto fut = get_promise.get_future();
  pending_get_requests++;

  auto act = name_table.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = ossia::net::get_osc_address_as_string(address);
  this->mSender->send(act, boost::string_ref(addr));

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

    if(mLogger.outbound_logger)
        mLogger.outbound_logger->info("Out: {0} {1}",
                                      ossia::net::address_string_from_node(address),
                                      val);

    return true;
  }

  return false;
}

bool minuit_protocol::observe(ossia::net::address_base& address, bool enable)
{
  std::lock_guard<std::mutex> lock(mListeningMutex);

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
  std::lock_guard<std::mutex> lock(mListeningMutex);

  if(enable)
    mListening.insert(
          std::make_pair(get_osc_address_as_string(address), &address));
  else
    mListening.erase(get_osc_address_as_string(address));

  return true;
}

void minuit_protocol::refresh(boost::string_ref req, const std::string& addr)
{
    auto it = mNamespaceRequests.find(addr);
    if (it == mNamespaceRequests.end())
    {
        mNamespaceRequests.insert(addr);
        mSender->send(req, boost::string_ref{addr});
    }
}

void minuit_protocol::refreshed(boost::string_ref addr)
{
    auto it = mNamespaceRequests.find(addr);
    if (it != mNamespaceRequests.end())
    {
        mNamespaceRequests.erase(it);
    }

    if (mNamespaceRequests.empty())
    {
        mNamespacePromise.set_value();
    }
}

osc::sender& minuit_protocol::sender() const
{
    return *mSender;
}

void minuit_protocol::handleReceivedMessage(
        const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
    boost::string_ref address{m.AddressPattern()};

    if (address.size() > 0 && address[0] == '/')
    {
        // Handle the OSC-like case where we receive a plain value.
    std::unique_lock<std::mutex> lock(mListeningMutex);
    auto it = mListening.find(m.AddressPattern());
    if (it != mListening.end())
    {
      ossia::net::address_base& addr = *it->second;
      lock.unlock();

      bool res = update_value(addr, m);
      if(res && mLogger.inbound_logger)
          mLogger.inbound_logger->info("In: {0} {1}", ossia::net::address_string_from_node(addr), addr.cloneValue());
    }
  }
  else
  {
    if (mDevice)
      ossia::minuit::minuit_message_handler::handleMinuitMessage(
          *this, *mDevice, address, m);
    }
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
