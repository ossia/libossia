#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/protocols/osc/osc_generic_protocol.hpp>

namespace ossia::net
{
template<typename OscVersion>
std::unique_ptr<ossia::net::protocol_base> make_osc_protocol_impl(network_context_ptr&& ctx, osc_protocol_configuration&& config)
{
  using conf = osc_protocol_configuration;

  switch(config.mode)
  {
    case conf::CLIENT:
    {
      using client_type = osc_protocol_client<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_protocol<client_type, udp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::TCP:
          return std::make_unique<osc_generic_protocol<client_type, tcp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::UNIX:
          return std::make_unique<osc_generic_protocol<client_type, unix_socket>>(std::move(ctx), std::get<fd_configuration>(config.configuration));
        case conf::SERIAL:
          return {}; // TODO
        case conf::WEBSOCKETS:
          return {}; // TODO
          break;
        default:
          break;
      }
      break;
    }
    case conf::SERVER:
    {
      using client_type = osc_protocol_server<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_protocol<client_type, udp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::TCP:
          return std::make_unique<osc_generic_protocol<client_type, tcp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::UNIX:
          return std::make_unique<osc_generic_protocol<client_type, unix_socket>>(std::move(ctx), std::get<fd_configuration>(config.configuration));
        case conf::SERIAL:
          return {}; // TODO
        case conf::WEBSOCKETS:
          return {}; // TODO
          break;
        default:
          break;
      }
      break;
    }
  }

  return{};
}

std::unique_ptr<ossia::net::protocol_base> make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config)
{
  using conf = osc_protocol_configuration;

  switch(config.version)
  {
    case conf::OSC1_0:
      return make_osc_protocol_impl<osc_1_0_policy>(std::move(ctx), std::move(config));
    case conf::OSC1_1:
      return make_osc_protocol_impl<osc_1_1_policy>(std::move(ctx), std::move(config));
    case conf::EXTENDED:
      return make_osc_protocol_impl<osc_extended_policy>(std::move(ctx), std::move(config));
    default:
      break;
  }

  return{};
}

}
