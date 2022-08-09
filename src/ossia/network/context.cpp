#include <ossia/network/context.hpp>
#include <ossia/network/context_functions.hpp>

namespace ossia::net
{
std::shared_ptr<ossia::net::network_context> create_network_context()
{
  return std::make_shared<ossia::net::network_context>();
}

void poll_network_context(ossia::net::network_context& ctx)
{
  ctx.context.poll();
}

void run_network_context(ossia::net::network_context& ctx)
{
  ctx.run();
}

std::thread run_threaded_network_context(ossia::net::network_context& ctx)
{
  return std::thread{[&ctx] { ctx.run(); }};
}

void stop_network_context(ossia::net::network_context& ctx)
{
  ctx.context.stop();
}
}
