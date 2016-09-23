#pragma once
#include <string>
#include <vector>
#include <cinttypes>
#include <memory>
#include <ossia_export.h>

namespace KDNSSD
{
class PublicService;
}

namespace ossia
{
namespace net
{
struct minuit_connection_data
{
  std::string name;
  std::string host;
  int32_t remote_port{};
  int32_t local_port{};
};

struct zeroconf_server
{
  zeroconf_server();
  zeroconf_server(const zeroconf_server&) = delete;
  zeroconf_server(zeroconf_server&&);
  zeroconf_server& operator=(const zeroconf_server&) = delete;
  zeroconf_server& operator=(zeroconf_server&&);
  ~zeroconf_server();

  zeroconf_server(std::unique_ptr<KDNSSD::PublicService> s);

private:
  std::unique_ptr<KDNSSD::PublicService> server;
};

/**
 * @brief zeroconf_supported
 * @return true if the library was built with zeroconf support
 */
OSSIA_EXPORT bool zeroconf_supported();

/**
 * @brief list_minuit_devices
 *
 * @note This function blocks for five seconds.
 *
 * @return a list of the bonjour-enabled minuit clients
 */
OSSIA_EXPORT std::vector<minuit_connection_data> list_minuit_devices();

/**
 * @brief make_minuit_zeroconf_server
 *
 * The available services names are :
 * * Minuit: _ossia_minuit._tcp
 *
 * @return A running Zeroconf server exposing the Minuit device info.
 */
OSSIA_EXPORT zeroconf_server make_zeroconf_server(
    std::string description,
    std::string service,
    std::string local_name,
    int32_t local_port,
    int32_t remote_port);

}
}
