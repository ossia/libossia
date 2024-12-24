#include "ZeroconfOscqueryListener.hpp"

#include "ossia-max.hpp"

#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/resolve.hpp>

namespace ossia
{
namespace max_binding
{
std::vector<std::shared_ptr<ossia::net::generic_device>>
    ZeroconfOscqueryListener::s_devices;
std::vector<std::pair<ZeroconfOscqueryListener::ConnectionEvent, std::string>>
    ZeroconfOscqueryListener::s_connection_events;
std::mutex ZeroconfOscqueryListener::s_mutex;

ZeroconfOscqueryListener::ZeroconfOscqueryListener()
    : service{"_oscjson._tcp"}
{
  for(const auto& i : service.getInstances())
    instanceAdded(i);

  service.addListener(this);
  service.beginBrowsing(servus::Interface::IF_ALL);
}

ZeroconfOscqueryListener::~ZeroconfOscqueryListener()
{
  service.removeListener(this);
  service.endBrowsing();
}

void ZeroconfOscqueryListener::instanceAdded(const std::string& instance)
{
  std::lock_guard<std::mutex> lock(ZeroconfOscqueryListener::s_mutex);
  s_connection_events.push_back(
      {ZeroconfOscqueryListener::ConnectionEvent::ADDED, instance});
}

void ZeroconfOscqueryListener::instanceRemoved(const std::string& instance)
{
  std::lock_guard<std::mutex> lock(ZeroconfOscqueryListener::s_mutex);
  s_connection_events.push_back(
      {ZeroconfOscqueryListener::ConnectionEvent::REMOVED, instance});
}

std::shared_ptr<ossia::net::generic_device>
ZeroconfOscqueryListener::find_device(const std::string& instance)
{
  std::lock_guard<std::mutex> lock(ZeroconfOscqueryListener::s_mutex);

  auto it = ossia::find_if(
      s_devices, [&](const auto& d) { return d->get_name() == instance; });

  if(it != s_devices.end())
  {
    return *it;
  }
  return nullptr;
}

void ZeroconfOscqueryListener::browse()
{
  {
    std::lock_guard lock(s_mutex);
    for(const auto& event : s_connection_events)
    {
      switch(event.first)
      {
        case ZeroconfOscqueryListener::ConnectionEvent::ADDED:
          addInstance(event.second);
          break;
        case ZeroconfOscqueryListener::ConnectionEvent::REMOVED:
          removeInstance(event.second);
          break;
      }
    }
    s_connection_events.clear();
  }

  try
  {
    service.browse(0);
  }
  catch(const std::exception& e)
  {
    std::cerr << "error while browsing  oscquery devices: " << e.what() << std::endl;
  }
}

void ZeroconfOscqueryListener::addInstance(const std::string& instance)
{
  for(const auto& dev : s_devices)
  {
    if(dev->get_name() == instance)
      return;
  }

  std::string ip = service.get(instance, "servus_ip");
  std::string port = service.get(instance, "servus_port");
  if(ip.empty())
  {
    ip = service.get(instance, "servus_host");
  }

  resolve_sync_v4(ip, port);
  if(ip.empty() || port.empty())
    return;

  try
  {
    auto clt = std::make_unique<ossia::net::generic_device>(
        std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
            "ws://" + ip + ":" + port),
        instance);
    clt->get_protocol().update(clt->get_root_node());
    s_devices.push_back(std::move(clt));
  }
  catch(...)
  {
    std::cerr << "Could not connect: "
              << "ws://" + ip + ":" + port << "\n";
  }
}

void ZeroconfOscqueryListener::removeInstance(const std::string& name)
{
  for(auto client : ossia_max::instance().clients.reference())
  {
    if(client->is_zeroconf() && client->m_device && client->m_device->get_name() == name)
    {
      ossia::max_binding::client::client::disconnect(client);
      clock_delay(client->m_clock, 1000); // hardcoded reconnection delay
    }
  }

  ossia::remove_erase_if(
      s_devices, [&](const auto& d) { return d->get_name() == name; });
}

}
} // namespace ossia
