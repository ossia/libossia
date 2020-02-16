// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ZeroconfMinuitListener.hpp"
#include <asio/io_service.hpp>
#include <asio/ip/basic_resolver.hpp>
#include <asio/ip/tcp.hpp>

#include <ossia/network/minuit/minuit.hpp>
#include <boost/lexical_cast.hpp>

#include "ossia-max.hpp"

namespace ossia
{
namespace max
{
std::vector<ossia::net::minuit_connection_data> ZeroconfMinuitListener::m_devices;
std::vector<std::vector<ossia::net::minuit_connection_data>::iterator> ZeroconfMinuitListener::m_zombie_devices;

std::mutex ZeroconfMinuitListener::m_mutex;
std::vector<std::string> ZeroconfMinuitListener::m_embryo_devices;

// TODO add support for Minuit discovery
  ZeroconfMinuitListener::ZeroconfMinuitListener()
    : service {"_minuit._tcp"}
  {
    for (const auto& i : service.getInstances())
      instanceAdded(i);

    service.addListener(this);
    service.beginBrowsing(servus::Interface::IF_ALL);
  }

  ZeroconfMinuitListener::~ZeroconfMinuitListener()
  {
    service.removeListener(this);
    service.endBrowsing();
  }

  void ZeroconfMinuitListener::instanceAdded(const std::string& instance)
  {
    std::lock_guard<std::mutex> lock(ZeroconfMinuitListener::m_mutex);
    m_embryo_devices.push_back(instance);
  }

  void ZeroconfMinuitListener::process_new_devices(const std::string& instance)
  {
    for (const auto& dev : m_devices)
    {
      if (dev.name == instance)
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
      ossia::net::minuit_connection_data dat;
      dat.name = instance;
      dat.host = ip;
      dat.remote_port = boost::lexical_cast<int>(port);

      m_devices.push_back(std::move(dat));
    }
    catch (...)
    {
      std::cerr << "Could not connect to Minuit device: "
                << instance + " on " + ip + ":" + port << "\n";
    }
  }

  void ZeroconfMinuitListener::instanceRemoved(const std::string& instance)
  {
    std::lock_guard<std::mutex> lock(ZeroconfMinuitListener::m_mutex);
    auto it = ossia::find_if(m_devices, [&](const auto& d) {
      return d.name == instance;
    });

    if (it != m_devices.end())
    {
      m_zombie_devices.push_back(it);
    }
  }

  ossia::net::minuit_connection_data* ZeroconfMinuitListener::find_device(
      const std::string& instance)
  {
    std::lock_guard<std::mutex> lock(ZeroconfMinuitListener::m_mutex);
    {
      auto it = ossia::find_if(m_devices, [&](const auto& d) {
        return d.name == instance + " Minuit server";
      });

      if (it != m_devices.end())
      {
        return &(*it);
      }
      m_mutex.unlock();
    }
    return nullptr;

  }

  void ZeroconfMinuitListener::browse()
  {
    ZeroconfMinuitListener::m_mutex.lock();

    // FIXME we should take receiving order into account
    // a device that is first removed, then added
    // will currently be removed
    for(const auto& s : m_embryo_devices)
    {
      process_new_devices(s);
    }
    m_embryo_devices.clear();

    for(auto it : m_zombie_devices)
    {
      for (auto client : ossia_max::instance().clients.reference())
      {
        if(client->is_zeroconf() && client->m_device->get_name() == it->name)
        {
          auto dev = client->m_device;
          ossia::max::client::client::disconnect(client);
          delete dev; // FIXME this is ugly
          clock_delay(client->m_clock, 1000); // hardcoded reconnection delay
        }
      }

      m_devices.erase(it);
    }
    m_zombie_devices.clear();
    ZeroconfMinuitListener::m_mutex.unlock();

    service.browse(0);
  }

}
} // namespace ossia
