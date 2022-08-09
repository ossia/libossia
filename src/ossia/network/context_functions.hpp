#pragma once
#include <ossia/detail/config.hpp>

#include <memory>
#include <thread>

namespace ossia::net
{
struct network_context;
using network_context_ptr = std::shared_ptr<network_context>;

OSSIA_EXPORT
std::shared_ptr<ossia::net::network_context> create_network_context();

OSSIA_EXPORT
void run_network_context(ossia::net::network_context&);

OSSIA_EXPORT
void poll_network_context(ossia::net::network_context&);

OSSIA_EXPORT
std::thread run_threaded_network_context(ossia::net::network_context&);

OSSIA_EXPORT
void stop_network_context(ossia::net::network_context&);
}
