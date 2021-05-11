#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/protocols/osc/osc_generic_protocol.hpp>

namespace ossia::net
{
static socket_configuration get_socket_configuration(osc_protocol_configuration&& conf)
{
  if(auto ptr = std::get_if<socket_configuration>(&conf.configuration))
    return std::move(*ptr);
  else
    throw std::runtime_error("Invalid configuration passed to osc_protocol_configuration, expected a socket_configuration");
}

static fd_configuration get_fd_configuration(osc_protocol_configuration&& conf)
{
  if(auto ptr = std::get_if<fd_configuration>(&conf.configuration))
    return std::move(*ptr);
  else
    throw std::runtime_error("Invalid configuration passed to osc_protocol_configuration, expected a fd_configuration");
}

template<typename OscVersion>
std::unique_ptr<osc_protocol_base> make_osc_protocol_impl(network_context_ptr&& ctx, osc_protocol_configuration&& config)
{
  using conf = osc_protocol_configuration;

  switch(config.mode)
  {
    case conf::MIRROR:
    {
      using client_type = osc_protocol_client<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_bidir_protocol<client_type, udp_socket>>(std::move(ctx), get_socket_configuration(std::move(config)));

        case conf::TCP:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<client_type, tcp_size_prefix_client>>(std::move(ctx), get_socket_configuration(std::move(config)));
          else
            return std::make_unique<osc_generic_client_protocol<client_type, tcp_slip_client>>(std::move(ctx), get_socket_configuration(std::move(config)));

#if defined(ASIO_HAS_LOCAL_SOCKETS)
        case conf::UNIX_DGRAM:
          return std::make_unique<osc_generic_bidir_protocol<client_type, unix_datagram_socket>>(std::move(ctx), get_fd_configuration(std::move(config)));
        case conf::UNIX_STREAM:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<client_type, unix_stream_size_prefix_client>>(std::move(ctx), get_fd_configuration(std::move(config)));
          else
            return std::make_unique<osc_generic_client_protocol<client_type, unix_stream_slip_client>>(std::move(ctx), get_fd_configuration(std::move(config)));
#endif

        case conf::SERIAL:
          return {}; // TODO

        case conf::WEBSOCKETS:
          return {}; // TODO

        default:
          break;
      }
      break;
    }
    case conf::HOST:
    {
      using client_type = osc_protocol_server<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_bidir_protocol<client_type, udp_socket>>(std::move(ctx), get_socket_configuration(std::move(config)));

        case conf::TCP:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_server_protocol<client_type, tcp_size_prefix_server>>(std::move(ctx), get_socket_configuration(std::move(config)));
          else
            return std::make_unique<osc_generic_server_protocol<client_type, tcp_slip_server>>(std::move(ctx), get_socket_configuration(std::move(config)));

#if defined(ASIO_HAS_LOCAL_SOCKETS)
        case conf::UNIX_DGRAM:
          return std::make_unique<osc_generic_bidir_protocol<client_type, unix_datagram_socket>>(std::move(ctx), get_fd_configuration(std::move(config)));
        case conf::UNIX_STREAM:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_server_protocol<client_type, unix_stream_size_prefix_server>>(std::move(ctx), get_fd_configuration(std::move(config)));
          else
            return std::make_unique<osc_generic_server_protocol<client_type, unix_stream_slip_server>>(std::move(ctx), get_fd_configuration(std::move(config)));

#endif

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

  return {};
}

std::unique_ptr<osc_protocol_base> make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config)
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

  return {};
}

}
