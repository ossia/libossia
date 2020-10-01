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
std::vector<std::shared_ptr<ossia::net::generic_device>> ZeroconfMinuitListener::s_devices;

std::mutex ZeroconfMinuitListener::s_mutex;
std::vector<std::pair<ZeroconfMinuitListener::ConnectionEvent, std::string>> ZeroconfMinuitListener::s_connection_events;

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
    std::lock_guard<std::mutex> lock(ZeroconfMinuitListener::s_mutex);
    s_connection_events.push_back({ZeroconfMinuitListener::ADDED, instance});
  }

  void ZeroconfMinuitListener::instanceRemoved(const std::string& instance)
  {
    std::lock_guard<std::mutex> lock(ZeroconfMinuitListener::s_mutex);
    s_connection_events.push_back({ZeroconfMinuitListener::REMOVED, instance});
  }

  void ZeroconfMinuitListener::addDevice(const std::string& instance)
  {
    for (const auto& dev : s_devices)
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
      ossia::net::minuit_connection_data data;
      data.name = instance;
      data.host = ip;
      data.remote_port = boost::lexical_cast<int>(port);

      auto clt = std::make_unique<ossia::net::generic_device>(
          std::make_unique<ossia::net::minuit_protocol>(
              instance, ip,
              boost::lexical_cast<int>(port), (rand() % 64512) + 1024),
          instance);

      s_devices.push_back(std::move(clt));
    }
    catch (...)
    {
      std::cerr << "Could not connect to Minuit device: "
                << instance + " on " + ip + ":" + port << "\n";
    }
  }

  std::shared_ptr<ossia::net::generic_device> ZeroconfMinuitListener::find_device(
      const std::string& instance)
  {
    std::lock_guard<std::mutex> lock(ZeroconfMinuitListener::s_mutex);
    {
      auto it = ossia::find_if(s_devices, [&](const auto& d) {
        return d->get_name() == instance + " Minuit server";
      });

      if (it != s_devices.end())
      {
        return *it;
      }
      s_mutex.unlock();
    }
    return {};

  }

  void ZeroconfMinuitListener::browse()
  {
    ZeroconfMinuitListener::s_mutex.lock();

    for(const auto& s : s_connection_events)
    {
      switch(s.first)
      {
        case ZeroconfMinuitListener::ConnectionEvent::ADDED:
          addDevice(s.second);
          break;
        case ZeroconfMinuitListener::ConnectionEvent::REMOVED:
          break;

          std::lock_guard<std::mutex> lock(ZeroconfMinuitListener::s_mutex);
          auto it = ossia::find_if(s_devices, [&](const auto& d) {
            return d->get_name() == s.second;
          });

          if (it != s_devices.end())
          {
            for (auto client : ossia_max::instance().clients.reference())
            {
              if(client->is_zeroconf() && client->m_device->get_name() == it->get()->get_name())
              {
                ossia::max::client::client::disconnect(client);
                clock_delay(client->m_clock, 1000); // hardcoded reconnection delay
              }
            }
          }
      }
    }
    s_connection_events.clear();

    ZeroconfMinuitListener::s_mutex.unlock();

    service.browse(0);
  }

}
} // namespace ossia
