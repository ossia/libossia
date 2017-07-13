#include "zeroconf.hpp"
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#if defined(OSSIA_DNSSD)
#include <servus/servus.h>
#else
namespace servus
{
class Servus
{
};
}
#endif
namespace ossia
{
namespace net
{
bool zeroconf_supported()
{
  return true;
}

std::vector<minuit_connection_data> list_minuit_devices()
{
  std::vector<minuit_connection_data> cons;

#if defined(OSSIA_DNSSD)
  auto browser = new servus::Servus("_minuit._tcp");
  browser->beginBrowsing(servus::Servus::IF_ALL);
  for (int i = 0; i < 500; i++)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    browser->browse(0);
  }
  browser->endBrowsing();

  for (const auto& instance : browser->getInstances())
  {
    minuit_connection_data dat;
    dat.name = instance;
    dat.host = browser->get(instance, "servus_host");
    dat.local_port
        = boost::lexical_cast<int>(browser->get(instance, "servus_port"));
    cons.push_back(std::move(dat));
  }
#endif
  return cons;
}

std::vector<oscquery_connection_data> list_oscquery_devices()
{
  std::vector<oscquery_connection_data> cons;

#if defined(OSSIA_DNSSD)
  auto browser = new servus::Servus("_oscjson._tcp");
  browser->beginBrowsing(servus::Servus::IF_ALL);
  for (int i = 0; i < 500; i++)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    browser->browse(0);
  }
  browser->endBrowsing();

  for (const auto& instance : browser->getInstances())
  {
    oscquery_connection_data dat;
    dat.name = instance;
    dat.host = browser->get(instance, "servus_host");
    dat.port = boost::lexical_cast<int>(browser->get(instance, "servus_port"));
    cons.push_back(std::move(dat));
  }
#endif
  return cons;
}

zeroconf_server make_zeroconf_server(
    std::string description, std::string service, std::string local_name,
    int32_t local_port, int32_t remote_port)
{
#if defined(OSSIA_DNSSD)
  auto server = std::make_unique<servus::Servus>(service);
  server->set("LocalPort", std::to_string(local_port));
  server->set("LocalName", local_name);
  server->set("RemotePort", std::to_string(remote_port));
  server->set("Description", description);

  server->announce(local_port, description);

  return zeroconf_server{std::move(server)};
#else
  return zeroconf_server{};
#endif
}

zeroconf_server::zeroconf_server() = default;
zeroconf_server::zeroconf_server(zeroconf_server&&) = default;
zeroconf_server& zeroconf_server::operator=(zeroconf_server&&) = default;
zeroconf_server::~zeroconf_server() = default;

zeroconf_server::zeroconf_server(std::unique_ptr<servus::Servus> s)
    : server{std::move(s)}
{
}
}
}
