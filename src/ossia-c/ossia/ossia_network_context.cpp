#include "ossia_utils.hpp"

#include <ossia/network/context_functions.hpp>

#include <ossia-c/ossia-c.h>

extern "C" {

ossia_network_context_t ossia_network_context_create()
{
  return new ossia::net::network_context_ptr{ossia::net::create_network_context()};
}

void ossia_network_context_free(ossia_network_context_t ctx)
{
  safe_function(
      __func__, [=] { delete reinterpret_cast<ossia::net::network_context_ptr*>(ctx); });
}

void ossia_network_context_poll(ossia_network_context_t ctx)
{
  safe_function(__func__, [=] {
    ossia::net::poll_network_context(
        **reinterpret_cast<ossia::net::network_context_ptr*>(ctx));
  });
}
}
