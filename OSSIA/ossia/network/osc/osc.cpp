#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
#include <ossia/detail/logger.hpp>
#include <boost/algorithm/string.hpp>
namespace ossia
{
namespace net
{
osc_protocol::osc_protocol(
    std::string ip, uint16_t remote_port, uint16_t local_port)
  : mIp{ip}
  , mRemotePort{remote_port}
  , mLocalPort{local_port}
  , mSender{std::make_unique<osc::sender>(mLogger, ip, remote_port)}
  , mReceiver{std::make_unique<osc::receiver>(local_port, [=](const oscpack::ReceivedMessage& m,
                                              const oscpack::IpEndpointName& ip) {
  this->handleReceivedMessage(m, ip);
})}
{
  if(mReceiver->port() != local_port)
  {;
    throw ossia::connection_error{"osc_protocol::osc_protocol: "
                                  "Could not connect to port: " + std::to_string(local_port)};
  }
  mReceiver->run();
}

osc_protocol::~osc_protocol()
{
}

const std::string& osc_protocol::getIp() const
{
  return mIp;
}

osc_protocol& osc_protocol::setIp(std::string ip)
{
  mIp = ip;
  mSender = std::make_unique<osc::sender>(mLogger, mIp, mRemotePort);

  return *this;
}

uint16_t osc_protocol::getRemotePort() const
{
  return mRemotePort;
}

osc_protocol& osc_protocol::setRemotePort(uint16_t in_port)
{
  mRemotePort = in_port;
  mSender = std::make_unique<osc::sender>(mLogger, mIp, mRemotePort);

  return *this;
}

uint16_t osc_protocol::getLocalPort() const
{
  return mLocalPort;
}

osc_protocol& osc_protocol::setLocalPort(uint16_t out_port)
{
  mLocalPort = out_port;
  mReceiver = std::make_unique<osc::receiver>(out_port, [=](const oscpack::ReceivedMessage& m,
                                              const oscpack::IpEndpointName& ip) {
    this->handleReceivedMessage(m, ip);
  });
  return *this;
}

bool osc_protocol::getLearningStatus() const
{
  return mLearning;
}

osc_protocol& osc_protocol::setLearningStatus(bool newLearn)
{
  mLearning = newLearn;
  return *this;
}

bool osc_protocol::update(ossia::net::node_base& node)
{
  return false;
}

bool osc_protocol::pull(ossia::net::address_base& address)
{
  return false;
}

bool osc_protocol::push(const ossia::net::address_base& address)
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

bool osc_protocol::observe(ossia::net::address_base& address, bool enable)
{
  std::lock_guard<std::mutex> lock(mListeningMutex);

  if (enable)
    mListening.insert(
          std::make_pair(get_osc_address_as_string(address), &address));
  else
    mListening.erase(get_osc_address_as_string(address));

  return true;
}

void osc_protocol::handleReceivedMessage(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
  if(!mLearning)
  {
    auto addr_txt = m.AddressPattern();
    std::lock_guard<std::mutex> lock(mListeningMutex);
    auto it = mListening.find(addr_txt);
    if (it != mListening.end())
    {
      ossia::net::address_base& addr = *it->second;
      update_value(addr, m);
    }
    else
    {
      // We still want to save the value even if it is not listened to.
      if(auto n = find_node(mDevice->getRootNode(), addr_txt))
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
    handleLearn(m);
  }

  if(mLogger.inbound_logger)
    mLogger.inbound_logger->info("In: {0}", m);
}

template<std::size_t N>
static bool is_vec(std::vector<ossia::value>& t)
{
  if(t.size() != N)
    return false;

  for(auto& v : t)
  {
    switch(v.getType())
    {
      case val_type::INT:
      case val_type::FLOAT:
        continue;
      default:
        return false;
    }
  }

  return true;
}

void osc_protocol::handleLearn(const oscpack::ReceivedMessage& m)
{
  // Find-or-add algorithm
  ossia::string_view addr = m.AddressPattern();
  std::vector<std::string> v;
  boost::algorithm::split(v, addr, boost::algorithm::is_any_of("/"));

  // v is not empty.
  for(auto it = v.begin(); it != v.end(); )
  {
    if(!it->empty())
      ++it;
    else
      it = v.erase(it);
  }

  bool is_new = false;
  node_base* n = &mDevice->getRootNode();
  for(auto & part : v)
  {
    auto& cld = n->children();
    auto cld_it = ossia::find_if(cld, [&] (const auto& node) { return node->getName() == part; });
    if(cld_it != cld.end())
    {
      n = cld_it->get();
    }
    else
    {
      // Start adding
      n = n->createChild(part);
      is_new = true;
    }
  }

  if(!is_new)
    return;

  // Set-up address
  switch(m.ArgumentCount())
  {
    case 0:
    {
      n->createAddress();
      break;
    }
    case 1:
    {
      auto val = osc_utilities::create_value(m.ArgumentsBegin());
      auto addr = n->createAddress(val.getType());
      addr->setValue(val);
      break;
    }
    case 2:
    {
      auto val = osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount());
      if(is_vec<2>(val))
      {
        auto addr = n->createAddress(ossia::val_type::VEC2F);
        addr->setValue(convert<ossia::Vec2f>(val));
      }
      else
      {
        auto addr = n->createAddress(ossia::val_type::TUPLE);
        addr->setValue(osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    case 3:
    {
      auto val = osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount());
      if(is_vec<3>(val))
      {
        auto addr = n->createAddress(ossia::val_type::VEC3F);
        addr->setValue(convert<ossia::Vec2f>(val));
      }
      else
      {
        auto addr = n->createAddress(ossia::val_type::TUPLE);
        addr->setValue(osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    case 4:
    {
      auto val = osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount());
      if(is_vec<4>(val))
      {
        auto addr = n->createAddress(ossia::val_type::VEC4F);
        addr->setValue(convert<ossia::Vec2f>(val));
      }
      else
      {
        auto addr = n->createAddress(ossia::val_type::TUPLE);
        addr->setValue(osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    default:
    {
      auto addr = n->createAddress(ossia::val_type::TUPLE);
      addr->setValue(osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount()));
      break;
    }
  }
}

void osc_protocol::setDevice(device_base& dev)
{
  mDevice = &dev;
}
}
}
