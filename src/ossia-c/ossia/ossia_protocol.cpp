// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_utils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/local/local.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <cstdlib>
extern "C" {
// MOVEME
void ossia_string_free(char* str)
{
  free(str);
}

ossia_protocol_t ossia_protocol_multiplex_create()
{
  return safe_function(
      __func__, [=] { return new ossia_protocol(new ossia::net::multiplex_protocol); });
}

void ossia_protocol_multiplex_expose_to(ossia_protocol_t local, ossia_protocol_t other)
{
  if(!local)
  {
    ossia_log_error("ossia_protocol_local_expose_to: local is null");
    return;
  }
  if(!other)
  {
    ossia_log_error("ossia_protocol_local_expose_to: other is null");
    return;
  }

  return safe_function(__func__, [=] {
    auto ptr = dynamic_cast<ossia::net::multiplex_protocol*>(local->protocol);
    if(ptr)
    {
      ptr->expose_to(std::unique_ptr<ossia::net::protocol_base>(other->protocol));
    }
  });
}

ossia_protocol_t ossia_protocol_osc_create(const char* ip, int in_port, int out_port)
{
#if defined(OSSIA_PROTOCOL_OSC)
  return safe_function(__func__, [=] {
    return new ossia_protocol{new ossia::net::osc_protocol(ip, in_port, out_port)};
  });
#else
  return nullptr;
#endif
}

ossia_protocol_t ossia_protocol_minuit_create(
    const char* local_name, const char* ip, int in_port, int out_port)
{
#if defined(OSSIA_PROTOCOL_MINUIT)
  return safe_function(__func__, [=] {
    return new ossia_protocol{
        new ossia::net::minuit_protocol(local_name, ip, in_port, out_port)};
  });
#else
  return nullptr;
#endif
}

ossia_protocol_t ossia_protocol_oscquery_server_create(int osc_port, int ws_port)
{
#if defined(OSSIA_PROTOCOL_OSCQUERY)
  return safe_function(__func__, [=] {
    return new ossia_protocol{
        new ossia::oscquery::oscquery_server_protocol(osc_port, ws_port)};
  });
#else
  return nullptr;
#endif
}

ossia_protocol_t ossia_protocol_oscquery_mirror_create(const char* host)
{
#if defined(OSSIA_PROTOCOL_OSCQUERY)
  return safe_function(__func__, [=] {
    return new ossia_protocol{new ossia::oscquery::oscquery_mirror_protocol(host)};
  });
#else
  return nullptr;
#endif
}

void ossia_protocol_free(ossia_protocol_t proto)
{
  return safe_function(__func__, [=] { delete proto; });
}

void ossia_oscquery_list_devices(
    int timeout_ms, char*** names, char*** hosts, int** ports, size_t* size)
{
  if(!names || !hosts || !ports || !size)
  {
    ossia_log_error("ossia_oscquery_list_devices: a parameter is null");
    return;
  }
  *names = nullptr;
  *hosts = nullptr;
  *ports = nullptr;
  *size = 0;

  return safe_function(__func__, [=] {
    const auto devices
        = ossia::net::list_oscquery_devices(std::chrono::milliseconds{timeout_ms});
    const auto n = devices.size();
    if(n == 0)
      return;

    *names = (char**)std::malloc(sizeof(char*) * n);
    *hosts = (char**)std::malloc(sizeof(char*) * n);
    *ports = (int*)std::malloc(sizeof(int) * n);
    for(std::size_t i = 0; i < n; i++)
    {
      (*names)[i] = copy_string(devices[i].name);
      (*hosts)[i] = copy_string(devices[i].host);
      (*ports)[i] = devices[i].port;
    }
    *size = n;
  });
}

void ossia_oscquery_devices_free(char** names, char** hosts, int* ports, size_t size)
{
  // The strings come from copy_string: freed as ossia_string_free frees them
  for(std::size_t i = 0; i < size; i++)
  {
    if(names)
      ossia_string_free(names[i]);
    if(hosts)
      ossia_string_free(hosts[i]);
  }
  std::free(names);
  std::free(hosts);
  std::free(ports);
}
}
