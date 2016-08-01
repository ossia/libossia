#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>

namespace ossia
{
namespace net
{
local_protocol::local_protocol()
{
}

local_protocol::~local_protocol()
{
}

bool local_protocol::pull(ossia::net::address_base& address)
{
  return false;
}

bool local_protocol::push(const ossia::net::address_base& address)
{
  bool b = true;
  for (auto& proto : mExposed)
    b &= proto->push(address);

  return b;
}

bool local_protocol::observe(ossia::net::address_base& address, bool enable)
{
  bool b = true;
  for (auto& proto : mExposed)
    b &= proto->observe_quietly(address, enable);

  return b;
}

bool local_protocol::update(ossia::net::node_base& node)
{
  return false;
}

void local_protocol::setDevice(device_base& dev)
{
  mDevice = dynamic_cast<ossia::net::generic_device*>(&dev);
}

static void observe_rec(protocol_base& proto, ossia::net::node_base& n)
{
  for(auto& cld : n.children())
  {
    if(auto addr = cld->getAddress())
    {
      if(!addr->callbacks().empty())
      {
        proto.observe_quietly(*addr, true);
      }
    }
    observe_rec(proto, *cld);
  }
}

void local_protocol::exposeTo(std::unique_ptr<protocol_base> p)
{
  if (p)
  {
    p->setDevice(*mDevice);

    // Expose all the adresses with callbacks
    observe_rec(*p, mDevice->getRootNode());

    mExposed.push_back(std::move(p));
  }
}

void local_protocol::stopExposeTo(const protocol_base& p)
{
  mExposed.erase(
      ossia::remove_if(
          mExposed, [&](const auto& ptr) { return ptr.get() == &p; }),
      mExposed.end());
}
}
}
