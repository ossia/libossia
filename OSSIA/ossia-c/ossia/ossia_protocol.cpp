// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_utils.hpp"

#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
extern "C" {
// MOVEME
void ossia_string_free(char* str)
{
  free(str); // Not delete because str has been allocated with strdup.
}

ossia_protocol_t ossia_protocol_local_create()
{
  return safe_function(__func__, [=] {
    return new ossia_protocol(new ossia::net::multiplex_protocol);
  });
}

void ossia_protocol_local_expose_to(
    ossia_protocol_t local, ossia_protocol_t other)
{
  if (!local)
  {
    ossia_log_error("ossia_protocol_local_expose_to: local is null");
    return;
  }
  if (!other)
  {
    ossia_log_error("ossia_protocol_local_expose_to: other is null");
    return;
  }

  return safe_function(__func__, [=] {
    auto ptr = dynamic_cast<ossia::net::multiplex_protocol*>(local->protocol);
    if (ptr)
    {
      ptr->expose_to(
          std::unique_ptr<ossia::net::protocol_base>(other->protocol));
    }
  });
}

ossia_protocol_t
ossia_protocol_osc_create(const char* ip, int in_port, int out_port)
{
  return safe_function(__func__, [=] {
    return new ossia_protocol{
        new ossia::net::osc_protocol(ip, in_port, out_port)};
  });
}

ossia_protocol_t ossia_protocol_minuit_create(
    const char* local_name, const char* ip, int in_port, int out_port)
{
  return safe_function(__func__, [=] {
    return new ossia_protocol{
        new ossia::net::minuit_protocol(local_name, ip, in_port, out_port)};
  });
}

ossia_protocol_t ossia_protocol_oscquery_server_create(
        int osc_port,
        int ws_port)
{
  return safe_function(__func__, [=] {
    return new ossia_protocol{
        new ossia::oscquery::oscquery_server_protocol(osc_port, ws_port)};
  });
}

ossia_protocol_t ossia_protocol_oscquery_mirror_create(
        const char* host)
{
  return safe_function(__func__, [=] {
    return new ossia_protocol{
        new ossia::oscquery::oscquery_mirror_protocol(host)};
  });
}

void ossia_protocol_free(ossia_protocol_t proto)
{
  return safe_function(__func__, [=] {
    if (proto)
      delete proto;
  });
}
}
