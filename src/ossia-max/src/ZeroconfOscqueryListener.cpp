#include "ZeroconfOscqueryListener.hpp"
#include <asio/io_service.hpp>
#include <asio/ip/basic_resolver.hpp>
#include <asio/ip/tcp.hpp>

#include <ossia/network/oscquery/oscquery_mirror.hpp>

#include "ossia-max.hpp"

namespace ossia
{
namespace max
{
std::vector<std::shared_ptr<ossia::net::generic_device>> ZeroconfOscqueryListener::m_devices;
std::vector<std::string>  ZeroconfOscqueryListener::m_zombie_devices;
std::mutex ZeroconfOscqueryListener::m_mutex;

// TODO add support for Minuit discovery
  ZeroconfOscqueryListener::ZeroconfOscqueryListener()
    : service {"_oscjson._tcp"}
  {
    for (const auto& i : service.getInstances())
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
    std::lock_guard<std::mutex> lock(ZeroconfOscqueryListener::m_mutex);

    for (const auto& dev : m_devices)
    {
      if (dev->get_name() == instance)
        return;
    }

    std::string ip = service.get(instance, "servus_ip");
    std::string port = service.get(instance, "servus_port");
    if (ip.empty())
    {
      ip = service.get(instance, "servus_host");
    }

    try
    {
      asio::io_service io_service;
      asio::ip::tcp::resolver resolver(io_service);
      asio::ip::tcp::resolver::query query(ip, port);
      asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
      if (iter->endpoint().address().is_loopback())
      {
        ip = "localhost";
      }
    }
    catch (...)
    {
    }

    if (ip.empty() || port.empty())
    {
      return;
    }

    try
    {
      auto clt = std::make_unique<ossia::net::generic_device>(
          std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
              "ws://" + ip + ":" + port),
          instance);
      clt->get_protocol().update(clt->get_root_node());
      m_devices.push_back(std::move(clt));
    }
    catch (...)
    {
      std::cerr << "Could not connect: "
                << "ws://" + ip + ":" + port << "\n";
    }
  }

  void ZeroconfOscqueryListener::instanceRemoved(const std::string& instance)
  {
    std::lock_guard<std::mutex> lock(ZeroconfOscqueryListener::m_mutex);

    m_zombie_devices.push_back(instance);
  }

  ossia::net::generic_device* ZeroconfOscqueryListener::find_device(
      const std::string& instance)
  {
    std::lock_guard<std::mutex> lock(ZeroconfOscqueryListener::m_mutex);

    auto it = ossia::find_if(m_devices, [&](const auto& d) {
      return d->get_name() == instance;
    });

    if (it != m_devices.end())
    {
      return it->get();
    }
    return nullptr;
  }

  void ZeroconfOscqueryListener::browse()
  {
    m_mutex.lock();
    for(const auto& name : m_zombie_devices)
    {
      for (auto client : ossia_max::instance().clients.reference())
      {
        if(client->is_zeroconf() && client->m_device && client->m_device->get_name() == name)
        {
          ossia::max::client::client::disconnect(client);
          clock_delay(client->m_clock, 1000); // hardcoded reconnection delay
        }
      }

      ossia::remove_erase_if(m_devices, [&](const auto& d) {
          return d->get_name() == name;
      });
    }
    m_zombie_devices.clear();
    m_mutex.unlock();

    service.browse(0);
  }

}
} // namespace ossia
