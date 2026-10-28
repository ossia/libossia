#include <ossia/protocols/oscquery/oscquery_factory.hpp>
#include <ossia/protocols/oscquery/oscquery_generic_server.hpp>

namespace ossia::net
{
std::unique_ptr<ossia::net::protocol_base> make_oscquery_server_protocol(
    network_context_ptr ctx, oscquery_server_protocol_configuration config)
{
  struct
  {
    network_context_ptr&& ctx;
    oscquery_server_protocol_configuration& config;

    auto operator()(ossia::net::ws_server_configuration&& conf) const
        -> std::unique_ptr<protocol_base>
    {
      return std::make_unique<oscquery_generic_server>(std::move(ctx), conf);
    }
  } vis{std::move(ctx), config};

  using namespace std;
  return visit(vis, std::move(config.transport));
}
}
