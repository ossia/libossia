#include <ossia/network/context.hpp>
#include <ossia/detail/thread.hpp>
#include <ossia/network/context_functions.hpp>

namespace ossia::net
{
std::shared_ptr<ossia::net::network_context> create_network_context()
{
  return std::make_shared<ossia::net::network_context>();
}

void poll_network_context(ossia::net::network_context& ctx)
{
  ctx.poll();
}

void run_network_context(ossia::net::network_context& ctx)
{
  ctx.run();
}

ossia::thread run_threaded_network_context(ossia::net::network_context& ctx)
{
  return ossia::thread{[&ctx] { ctx.run(); }};
}

void stop_network_context(ossia::net::network_context& ctx)
{
  ctx.context.stop();
}
}
